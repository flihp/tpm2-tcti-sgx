/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <sgx_error.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tss2_tpm2_types.h>

#include "tss2-tcti-sgx.h"
#include "tcti-sgx_priv.h"
#include "tcti-sgx-common.h"

int
tcti_struct_setup (void **state)
{
    TSS2_TCTI_CONTEXT *context = NULL;
    TSS2_RC ret = TSS2_RC_SUCCESS;
    size_t tcti_size = 0;

    ret = Tss2_Tcti_Sgx_Init (NULL, &tcti_size);
    if (ret != TSS2_RC_SUCCESS) {
        printf ("Tss2_Tcti_Sgx_Init failed: %d\n", ret);
        return 1;
    }
    context = calloc (1, tcti_size);
    if (context == NULL) {
        perror ("calloc");
        return 1;
    }
    /*
     * prime data for mock ocall:
     *   OCall returns an ID of 1
     *   OCall return value indicates success
     */
    will_return (__wrap_tcti_sgx_init_ocall, 1);
    will_return (__wrap_tcti_sgx_init_ocall, SGX_SUCCESS);
    ret = Tss2_Tcti_Sgx_Init (context, 0);
    if (ret != TSS2_RC_SUCCESS) {
        printf ("%s: tcti_sgx_init failed with RC 0x%x\n", __func__, ret);
        return 1;
    }
    *state = context;
    return 0;
}

int
tcti_struct_teardown (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;

    Tss2_Tcti_Finalize (context);
    if (context)
        free (context);
    return 0;
}
