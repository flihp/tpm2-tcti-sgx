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
#include "tss2_tcti_sgx_u.h"

#define RAND_SRC "/dev/urandom"

/* callback to initialize new TCTI */
typedef TSS2_TCTI_CONTEXT* (*downstream_tcti_init_cb) (void *user_data);

typedef struct tss2_tcti_sgx_mgr {
    downstream_tcti_init_cb  init_cb;
    gpointer                 user_data;
    GHashTable              *session_table;
    GMutex                  *session_table_mutex;
} tss2_tcti_sgx_mgr_t;

typedef struct tss2_tcti_sgx_session {
    uint64_t           id;
    TSS2_TCTI_CONTEXT *tcti_context;
    GMutex            *mutex;
} tss2_tcti_sgx_session_t;

/* Global mgr variable with file scope.
 * We use this like a singleton since we have to respond to
 * tss2_tcti_sgx_init_ocall by creating a new session and adding it to the mgr
 * but we can't have the enclave pass us a reference to the mgr (I don't
 * think). This is a less desirable option.
 */
static tss2_tcti_sgx_mgr_t *mgr_global = NULL;

/* function to initialize the application / untrusted library
 */
tss2_tcti_sgx_mgr_t*
tss2_tcti_sgx_mgr_init (downstream_tcti_init_cb callback,
                        gpointer                user_data)
{
    if (mgr_global != NULL)
        g_error ("tss2_tcti_sgx_mgr already initialized");
    mgr_global = calloc (1, sizeof (tss2_tcti_sgx_mgr_t));
    if (mgr_global == NULL) {
        perror ("calloc");
        goto out;
    }
    mgr_global->init_cb = callback;
    mgr_global->user_data = user_data;
    g_mutex_init (mgr_global->session_table_mutex);
    mgr_global->session_table = g_hash_table_new (g_int64_hash, g_int64_equal);
out:
    return mgr_global;
}

/* function called by enclave to initialize new TCTI connection
 */
uint64_t
tss2_tcti_sgx_init_ocall ()
{
    tss2_tcti_sgx_session_t *session;
    gint fd;
    gboolean insert_result;

    if (mgr_global == NULL)
        g_error ("tss2_tcti_sgx_mgr not initialized");
    fd = open (RAND_SRC, O_RDONLY);
    if (fd == -1)
        g_error ("failed to open %s: %s", RAND_SRC, strerror (errno));
    session = calloc (1, sizeof (tss2_tcti_sgx_session_t));
    if (session == NULL)
        g_error ("failed to allocate memory for session structure: %s",
                 strerror (errno));
    g_mutex_lock (mgr_global->session_table_mutex);
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
    g_mutex_unlock (mgr_global->session_table_mutex);
    return session->id;
}

TSS2_RC
tss2_tcti_sgx_transmit_ocall (uint64_t         id,
                              size_t           size,
                              const uint8_t   *command)
{
    tss2_tcti_sgx_session_t *session;
    TSS2_RC ret;

    g_mutex_lock (mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (mgr_global->session_table_mutex);
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
tss2_tcti_sgx_receive_ocall (uint64_t   id,
                             size_t     size,
                             uint8_t   *response,
                             uint32_t   timeout)
{
    tss2_tcti_sgx_session_t *session;
    TSS2_RC ret;

    /* we only support blocking calls currently */
    if (timeout != TSS2_TCTI_TIMEOUT_BLOCK)
        return TSS2_TCTI_RC_BAD_VALUE;

    g_mutex_lock (mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (mgr_global->session_table_mutex);
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
tss2_tcti_sgx_finalize_ocall (uint64_t id)
{
    tss2_tcti_sgx_session_t *session;

    g_mutex_lock (mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (mgr_global->session_table_mutex);
    if (session == NULL)
        return;
    g_mutex_lock (session->mutex);
    Tss2_Tcti_Finalize (session->tcti_context);
    g_mutex_unlock (session->mutex);
}

TSS2_RC
tss2_tcti_sgx_cancel_ocall (uint64_t id)
{
    tss2_tcti_sgx_session_t *session;
    TSS2_RC ret;

    g_mutex_lock (mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    g_mutex_lock (session->mutex);
    ret = Tss2_Tcti_Cancel (session->tcti_context);
    g_mutex_unlock (session->mutex);
    return ret;
}

TSS2_RC
tss2_tcti_sgx_get_poll_handles (uint64_t               id,
                                TSS2_TCTI_POLL_HANDLE *handles,
                                size_t                *num_handles)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

TSS2_RC
tss2_tcti_sgx_set_locality_ocall (uint64_t id,
                                  uint8_t  locality)
{
    tss2_tcti_sgx_session_t *session;
    TSS2_RC ret;

    g_mutex_lock (mgr_global->session_table_mutex);
    session = g_hash_table_lookup (mgr_global->session_table, &id);
    g_mutex_unlock (mgr_global->session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    g_mutex_lock (session->mutex);
    ret = Tss2_Tcti_SetLocality (session->tcti_context, locality);
    g_mutex_unlock (session->mutex);
    return ret;
}
