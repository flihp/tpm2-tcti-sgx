/*
 * Copyright (c) 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TCTI_SGX_MGR_H
#define TCTI_SGX_MGR_H

#include <glib.h>
#include <stdint.h>

#include <tss2/tss2_tpm2_types.h>
#include <tss2/tss2_tcti.h>

typedef TSS2_TCTI_CONTEXT* (*downstream_tcti_init_cb) (void *user_data);

typedef struct tcti_sgx_mgr {
    downstream_tcti_init_cb  init_cb;
    gpointer user_data;
    GHashTable *session_table;
    GMutex session_table_mutex;
} tcti_sgx_mgr_t;

typedef struct tcti_sgx_session {
    uint64_t id;
    TSS2_TCTI_CONTEXT *tcti_context;
    GMutex *mutex;
} tcti_sgx_session_t;

tcti_sgx_mgr_t* tcti_sgx_mgr_init (downstream_tcti_init_cb callback,
                                   gpointer user_data);
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
TSS2_RC tcti_sgx_get_poll_handles (uint64_t id,
                                   TSS2_TCTI_POLL_HANDLE *handles,
                                   size_t *num_handles);
TSS2_RC tcti_sgx_set_locality_ocall (uint64_t id,
                                     uint8_t locality);

#endif