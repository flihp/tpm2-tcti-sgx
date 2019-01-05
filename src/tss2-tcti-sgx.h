/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TSS2_TCTI_SGX
#define TSS2_TCTI_SGX

#include <tss2/tss2_tpm2_types.h>
#include <tss2/tss2_tcti.h>

#if defined (__cplusplus)
extern "C" {
#endif

TSS2_RC Tss2_Tcti_Sgx_Init (TSS2_TCTI_CONTEXT *context, size_t *size);

#if defined (__cplusplus)
}
#endif
#endif /* TSS2_TCTI_SGX */
