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
#include "tcti-util.h"

#include "tcti-util.h"

#include <iostream>
#include <list>

using namespace std;

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
TctiSgxMgr::TctiSgxMgr (downstream_tcti_init_cb init_cb,
                        gpointer user_data)
: init_cb (init_cb), user_data (user_data) {}
TctiSgxMgr::~TctiSgxMgr () {}

TctiSgxSession*
TctiSgxMgr::session_lookup (uint64_t id)
{
    list <TctiSgxSession*>::const_iterator itr;

    cout << __func__ << ": looking up TctiSgxSession with id: " << id << endl;
    for (itr = this->sessions.begin (); itr != this->sessions.end (); ++itr)
    {
        if ((*itr)->id == id) {
            return *itr;
        }
    }
    return NULL;
}

void
TctiSgxMgr::session_remove (uint64_t id)
{
    list <TctiSgxSession*>::const_iterator itr;

    for (itr = this->sessions.begin (); itr != this->sessions.end (); ++itr)
    {
        if ((*itr)->id == id) {
            delete *itr;
            this->sessions.erase (itr);
            return;
        }
    }
    return;
}

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
    if (callback == NULL) {
        callback = tabrmd_tcti_init;
    }
    TctiSgxMgr::get_instance (callback, user_data);
    return 0;
}

/*
 * function called by enclave to initialize new TCTI connection
 */
uint64_t
tcti_sgx_init_ocall ()
{
    TctiSgxSession *session;
    TctiSgxMgr& mgr = TctiSgxMgr::get_instance (tabrmd_tcti_init, NULL);
    gint fd;
    uint64_t id;
    TSS2_TCTI_CONTEXT *tcti_context;

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
    tcti_context = mgr.init_cb (mgr.user_data);
    if (tcti_context == NULL) {
        cout << __func__ << ": tcti init callback failed to create a TCTI" << endl;
        return 0;
    }
    session = new TctiSgxSession (id, tcti_context);
    g_mutex_lock (&mgr.session_table_mutex);
    mgr.sessions.push_front (session);
    g_mutex_unlock (&mgr.session_table_mutex);
    return session->id;
}

TSS2_RC
tcti_sgx_transmit_ocall (uint64_t id,
                         size_t size,
                         const uint8_t *command)
{
    TctiSgxMgr& mgr = TctiSgxMgr::get_instance (NULL, NULL);
    TctiSgxSession *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr.session_table_mutex);
    session = mgr.session_lookup (id);
    g_mutex_unlock (&mgr.session_table_mutex);
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
    TctiSgxMgr& mgr = TctiSgxMgr::get_instance (NULL, NULL);
    TctiSgxSession *session;
    TSS2_RC ret;

    /* we only support blocking calls currently */
    if (timeout != TSS2_TCTI_TIMEOUT_BLOCK)
        return TSS2_TCTI_RC_BAD_VALUE;

    g_mutex_lock (&mgr.session_table_mutex);
    session = mgr.session_lookup (id);
    g_mutex_unlock (&mgr.session_table_mutex);
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
    TctiSgxMgr& mgr = TctiSgxMgr::get_instance (NULL, NULL);
    TctiSgxSession *session;

    g_mutex_lock (&mgr.session_table_mutex);
    mgr.session_remove (id);
    g_mutex_unlock (&mgr.session_table_mutex);
}

TSS2_RC
tcti_sgx_cancel_ocall (uint64_t id)
{
    TctiSgxMgr& mgr = TctiSgxMgr::get_instance (NULL, NULL);
    TctiSgxSession *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr.session_table_mutex);
    session = mgr.session_lookup (id);
    g_mutex_unlock (&mgr.session_table_mutex);
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
    TctiSgxMgr& mgr = TctiSgxMgr::get_instance (NULL, NULL);
    TctiSgxSession *session;
    TSS2_RC ret;

    g_mutex_lock (&mgr.session_table_mutex);
    session = mgr.session_lookup (id);
    g_mutex_unlock (&mgr.session_table_mutex);
    if (session == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    session->lock ();
    ret = session->set_locality (locality);
    session->unlock ();
    return ret;
}
