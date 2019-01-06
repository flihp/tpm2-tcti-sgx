/*
 * Copyright (c) 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TCTI_SGX_MGR_H
#define TCTI_SGX_MGR_H

#include <stdint.h>

#include <tss2/tss2_tpm2_types.h>
#include <tss2/tss2_tcti.h>

typedef TSS2_TCTI_CONTEXT* (*downstream_tcti_init_cb) (void *user_data);

int tcti_sgx_mgr_init (downstream_tcti_init_cb callback,
                       void *user_data);
void tcti_sgx_mgr_finalize (void);

#endif
