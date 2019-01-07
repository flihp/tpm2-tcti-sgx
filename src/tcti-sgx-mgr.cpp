/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <tss2/tss2_tcti.h>
#include <tss2/tss2-tcti-tabrmd.h>

#include "tcti-sgx-mgr_priv.h"
#include "tcti-sgx-mgr.h"

#include <iostream>

using namespace std;

#define RAND_SRC "/dev/urandom"

/*
 * This code module contains the logic that we need to process 'ocalls'
 * from the enclave (enclaves?) using the SGX TCTI. The functions in the
 * TCTI that are envoked by the TSS2 result in the enclave interacting with
 * the outside world (where the TPM lives) via these 'ocalls'. Code in this
 * module reacts and responds to these ocalls.
 */
TSS2_TCTI_CONTEXT*
tabrmd_tcti_init (void *user_data)
{
    const char* conf_str = (const char*)user_data;
    TSS2_TCTI_CONTEXT* ctx = NULL;
    TSS2_RC rc = TSS2_RC_SUCCESS;
    size_t size = 0;

    rc = Tss2_Tcti_Tabrmd_Init (NULL, &size, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        printf ("%s: first call to Tss2_Tcti_Tabrmd_Init failed with RC 0x%"
                PRIx32 "\n", __func__, rc);
        return NULL;
    }
    ctx = (TSS2_TCTI_CONTEXT*)calloc (1, size);
    rc = Tss2_Tcti_Tabrmd_Init (ctx, &size, conf_str);
    if (rc != TSS2_RC_SUCCESS) {
        printf ("%s: second call to Tss2_Tcti_Tabrmd_Init failed with RC 0x%"
                PRIx32 "\n", __func__, rc);
        free (ctx);
        return NULL;
    }
    return ctx;
}

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

TctiSgxSession::TctiSgxSession (uint64_t id,
                                TSS2_TCTI_CONTEXT *tcti_context)
: tcti_context (tcti_context), id (id)
{
    g_mutex_init (&mutex);
}

TctiSgxSession::~TctiSgxSession ()
{
    Tss2_Tcti_Finalize (this->tcti_context);
    g_mutex_clear (&this->mutex);
    free (this->tcti_context);
}

void
TctiSgxSession::lock ()
{
    g_mutex_lock (&this->mutex);
}
void
TctiSgxSession::unlock ()
{
    g_mutex_unlock (&this->mutex);
}

TSS2_RC
TctiSgxSession::transmit (size_t size, uint8_t const *command)
{
    return Tss2_Tcti_Transmit (this->tcti_context, size, command);
}
TSS2_RC
TctiSgxSession::receive (size_t *size, uint8_t *response, int32_t timeout)
{
    return Tss2_Tcti_Receive (this->tcti_context, size, response, timeout);
}
TSS2_RC
TctiSgxSession::cancel ()
{
    return Tss2_Tcti_Cancel (this->tcti_context);
}
TSS2_RC
TctiSgxSession::set_locality (uint8_t locality)
{
    return Tss2_Tcti_SetLocality (this->tcti_context, locality);
}

/*
 * Function to initialize the application / untrusted library. The 'callback'
 * parameter is a caller provided function used to initialize a TCTI
 * instance used to communicate with the 'downstream' TPM. This allows for
 * flexible configuration by the application hosting the enclave.
 */
int
tcti_sgx_mgr_init (downstream_tcti_init_cb callback,
                   gpointer user_data)
{
    if (mgr_global != NULL) {
        cout << "already initialized" << endl;
        return 0;
    }
    if (callback == NULL) {
        callback = tabrmd_tcti_init;
    }
    mgr_global = (tcti_sgx_mgr_t*)calloc (1, sizeof (tcti_sgx_mgr_t));
    if (mgr_global == NULL) {
        perror ("calloc");
        return 1;
    }
    mgr_global->init_cb = callback;
    mgr_global->user_data = user_data;
    g_mutex_init (&mgr_global->session_table_mutex);
    mgr_global->session_table = g_hash_table_new (g_int64_hash, g_int64_equal);
    return 0;
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
    TctiSgxSession *session;
    gint fd;
    gboolean insert_result;
    uint64_t id;
    TSS2_TCTI_CONTEXT *tcti_context;

    if (mgr_global == NULL) {
        cout << __func__ << ": tcti_sgx_mgr not initialized" << endl;
        return 0;
    }
    fd = open (RAND_SRC, O_RDONLY);
    if (fd == -1) {
        cout << __func__ << ": failed to open " << RAND_SRC << ": "
            << strerror (errno) << endl;
        return 0;
    }
    if (read (fd, &id, sizeof (id)) != sizeof (id))
    {
        cout << __func__ << ": failed to read " << sizeof (session->id)
            << " bytes from " << RAND_SRC << ": " << strerror (errno) << endl;
        return 0;
    }
    tcti_context = mgr_global->init_cb (mgr_global->user_data);
    if (tcti_context == NULL) {
        cout << __func__ << ": tcti init callback failed to create a TCTI" << endl;
        return 0;
    }
    session = new TctiSgxSession (id, tcti_context);
    g_mutex_lock (&mgr_global->session_table_mutex);
    insert_result = g_hash_table_insert (mgr_global->session_table,
                                         &session->id,
                                         session);
    if (insert_result != TRUE) {
        cout << __func__ << ": failed to insert session into session table" << endl;
        g_mutex_unlock (&mgr_global->session_table_mutex);
        free (session);
        return 0;
    }
    g_mutex_unlock (&mgr_global->session_table_mutex);
    return session->id;
}

TSS2_RC
tcti_sgx_transmit_ocall (uint64_t id,
                         size_t size,
                         const uint8_t *command)
{
    TctiSgxSession *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = (TctiSgxSession*)g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    session->lock ();
    ret = session->transmit (size, command);
    session->unlock ();
    return ret;
}

TSS2_RC
tcti_sgx_receive_ocall (uint64_t id,
                        size_t size,
                        uint8_t *response,
                        uint32_t timeout)
{
    TctiSgxSession *session;
    TSS2_RC ret;

    /* we only support blocking calls currently */
    if (timeout != TSS2_TCTI_TIMEOUT_BLOCK)
        return TSS2_TCTI_RC_BAD_VALUE;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = (TctiSgxSession*)g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    session->lock ();
    ret = session->receive (&size, response, timeout);
    session->unlock ();

    return ret;
}

void
tcti_sgx_finalize_ocall (uint64_t id)
{
    TctiSgxSession *session;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = (TctiSgxSession*)g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return;
    delete session;
}

TSS2_RC
tcti_sgx_cancel_ocall (uint64_t id)
{
    TctiSgxSession *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = (TctiSgxSession*)g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    session->lock ();
    ret = session->cancel ();
    session->unlock ();
    return ret;
}

TSS2_RC
tcti_sgx_get_poll_handles_ocall (uint64_t id,
                                 TSS2_TCTI_POLL_HANDLE *handles,
                                 size_t *num_handles)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

TSS2_RC
tcti_sgx_set_locality_ocall (uint64_t id,
                             uint8_t locality)
{
    TctiSgxSession *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr_global->session_table_mutex);
    session = (TctiSgxSession*)g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (&mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    session->lock ();
    ret = session->set_locality (locality);
    session->unlock ();
    return ret;
}
