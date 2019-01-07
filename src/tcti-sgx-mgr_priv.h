/*
 * Copyright (c) 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TCTI_SGX_MGR_PRIV_H
#define TCTI_SGX_MGR_PRIV_H

#include <glib.h>

#include "tcti-sgx-mgr.h"

typedef struct tcti_sgx_mgr {
    downstream_tcti_init_cb  init_cb;
    gpointer user_data;
    GHashTable *session_table;
    GMutex session_table_mutex;
} tcti_sgx_mgr_t;

class TctiSgxSession {
    TSS2_TCTI_CONTEXT *tcti_context;
    GMutex mutex;
public:
    uint64_t id;
    TctiSgxSession (uint64_t id,
                    TSS2_TCTI_CONTEXT *tcti_context);
    ~TctiSgxSession ();
    void lock ();
    void unlock ();
    TSS2_RC transmit (size_t size, uint8_t const *command);
    TSS2_RC receive (size_t *size, uint8_t *response, int32_t timeout);
    TSS2_RC cancel ();
    TSS2_RC set_locality (uint8_t locality);
};

#if defined (__cplusplus)
extern "C" {
#endif

uint64_t tcti_sgx_init_ocall ();
TSS2_RC tcti_sgx_transmit_ocall (uint64_t id,
                                 size_t size,
                                 const uint8_t *command);
TSS2_RC tcti_sgx_receive_ocall (uint64_t id,
                                size_t size,
                                uint8_t *response,
                                uint32_t timeout);
void tcti_sgx_finalize_ocall (uint64_t id);
TSS2_RC tcti_sgx_cancel_ocall (uint64_t id);
TSS2_RC tcti_sgx_get_poll_handles_ocall (uint64_t id,
                                         TSS2_TCTI_POLL_HANDLE *handles,
                                         size_t *num_handles);
TSS2_RC tcti_sgx_set_locality_ocall (uint64_t id,
                                     uint8_t locality);
#if defined (__cplusplus)
}
#endif
#endif
