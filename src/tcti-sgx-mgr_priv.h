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

typedef struct tcti_sgx_session {
    uint64_t id;
    TSS2_TCTI_CONTEXT *tcti_context;
    GMutex mutex;
} tcti_sgx_session_t;

#endif
