/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <stdio.h>
#include <unistd.h>

#include <tss2/tss2_tcti.h>

#include "tcti-sgx-mgr.h"
#include "tcti_sgx_u.h"

#define RAND_SRC "/dev/urandom"

/*
 * This code module contains the logic that we need to process 'ocalls'
 * from the enclave (enclaves?) using the SGX TCTI. The functions in the
 * TCTI that are envoked by the TSS2 result in the enclave interacting with
 * the outside world (where the TPM lives) via these 'ocalls'. Code in this
 * module reacts and responds to these ocalls.
 */
/*
 * Global mgr variable with file scope.
 * We use this like a singleton since we have to respond to
 * tcti_sgx_init_ocall by creating a new session and adding it to the mgr.
 * The hard part is passing a reference to this structure into the
 * tcti_sgx_init_ocall function. We must provide an implementation of this
 * function @ compile time and it's only ever envoked by enclave code so
 * we can't pass in any data from the aplication via parameters. So we fall
 * back on using global with singleton-ish semantics.
 *
 * The data in this structure should be treated as 'readonly' after
 * initialization with the exception of the GHashTable we use to track
 * connections from within the enclave. When interacting with this variable
 * the caller MUST hold the 'session_table_mutex'.
 */
static tcti_sgx_mgr_t *mgr_global = NULL;

/*
 * Function to initialize the application / untrusted library. The 'callback'
 * parameter is a caller provided function used to initialize a TCTI
 * instance used to communicate with the 'downstream' TPM. This allows for
 * flexible configuration by the application hosting the enclave.
 */
tcti_sgx_mgr_t*
tcti_sgx_mgr_init (downstream_tcti_init_cb callback,
                   gpointer user_data)
{
    if (mgr_global != NULL) {
        printf ("already initialized\n");
        return NULL;
    }
    if (callback == NULL) {
        printf ("callback parameter is required\n");
        return NULL;
    }
    mgr_global = calloc (1, sizeof (tcti_sgx_mgr_t));
    if (mgr_global == NULL) {
        perror ("calloc");
        goto out;
    }
    mgr_global->init_cb = callback;
    mgr_global->user_data = user_data;
    g_mutex_init (&mgr_global->session_table_mutex);
    mgr_global->session_table = g_hash_table_new (g_int64_hash, g_int64_equal);
out:
    return mgr_global;
}

void
tcti_sgx_mgr_finalize (void)
{
    if (mgr_global != NULL) {
        g_hash_table_unref (mgr_global->session_table);
        free (mgr_global);
        mgr_global = NULL;
    }
}

/*
 * function called by enclave to initialize new TCTI connection
 */
uint64_t
tcti_sgx_init_ocall ()
{
    tcti_sgx_session_t *session;
    gint fd;
    gboolean insert_result;

    if (mgr_global == NULL) {
        printf ("%s: tcti_sgx_mgr not initialized\n", __func__);
        return 0;
    }
    fd = open (RAND_SRC, O_RDONLY);
    if (fd == -1) {
        printf ("%s: failed to open %s: %s",
                __func__, RAND_SRC, strerror (errno));
        return 0;
    }
    session = calloc (1, sizeof (tcti_sgx_session_t));
    if (session == NULL)
        g_error ("failed to allocate memory for session structure: %s",
                 strerror (errno));
    g_mutex_lock (&mgr_global->session_table_mutex);
    if (read (fd, &session->id, sizeof (session->id)) != sizeof (session->id))
        g_error ("failed to read %d bytes from %s: %s",
                 sizeof (session->id),
                 RAND_SRC,
                 strerror (errno));
    session->tcti_context = mgr_global->init_cb (mgr_global->user_data);
    if (session->tcti_context == NULL)
        g_error ("tcti init callback failed to create a TCTI");
    g_mutex_init (session->mutex);
    insert_result = g_hash_table_insert (mgr_global->session_table,
                                         &session->id,
                                         session);
    if (insert_result != TRUE)
        g_error ("failed to insert session into session table");
    g_mutex_unlock (&mgr_global->session_table_mutex);
    return session->id;
}

TSS2_RC
tcti_sgx_transmit_ocall (uint64_t id,
                         size_t size,
                         const uint8_t *command)
{
    tcti_sgx_session_t *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    g_mutex_lock (session->mutex);
    ret = Tss2_Tcti_Transmit (session->tcti_context,
                              size,
                              command);
    g_mutex_unlock (session->mutex);
    return ret;
}

TSS2_RC
tcti_sgx_receive_ocall (uint64_t id,
                        size_t size,
                        uint8_t *response,
                        uint32_t timeout)
{
    tcti_sgx_session_t *session;
    TSS2_RC ret;

    /* we only support blocking calls currently */
    if (timeout != TSS2_TCTI_TIMEOUT_BLOCK)
        return TSS2_TCTI_RC_BAD_VALUE;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    g_mutex_lock (session->mutex);
    ret = Tss2_Tcti_Receive (session->tcti_context,
                             &size,
                             response,
                             timeout);
    g_mutex_unlock (session->mutex);

    return ret;
}

void
tcti_sgx_finalize_ocall (uint64_t id)
{
    tcti_sgx_session_t *session;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return;
    g_mutex_lock (session->mutex);
    Tss2_Tcti_Finalize (session->tcti_context);
    g_mutex_unlock (session->mutex);
}

TSS2_RC
tcti_sgx_cancel_ocall (uint64_t id)
{
    tcti_sgx_session_t *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    g_mutex_lock (session->mutex);
    ret = Tss2_Tcti_Cancel (session->tcti_context);
    g_mutex_unlock (session->mutex);
    return ret;
}

TSS2_RC
tcti_sgx_get_poll_handles (uint64_t id,
                           TSS2_TCTI_POLL_HANDLE *handles,
                           size_t *num_handles)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

TSS2_RC
tcti_sgx_set_locality_ocall (uint64_t id,
                             uint8_t locality)
{
    tcti_sgx_session_t *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    g_mutex_lock (session->mutex);
    ret = Tss2_Tcti_SetLocality (session->tcti_context, locality);
    g_mutex_unlock (session->mutex);
    return ret;
}
