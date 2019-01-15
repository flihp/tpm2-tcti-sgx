/*
 * Copyright 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>
#include <cmocka.h>

#include "tcti-util.h"

void*
__wrap_calloc (size_t nmemb,
               size_t size)
{
    return mock_type (void*);
}

void
__wrap_free (void* mem)
{
    return;
}

TSS2_RC
__wrap_Tss2_Tcti_Mssim_Init (TSS2_TCTI_CONTEXT *context,
                             size_t *size,
                             const char *conf)
{
    *size = mock_type (size_t);
    return mock_type (TSS2_RC);
}

static void
mssim_tcti_init_first_fail (void **state)
{
    will_return (__wrap_Tss2_Tcti_Mssim_Init, 10);
    will_return (__wrap_Tss2_Tcti_Mssim_Init, 1);
    TSS2_TCTI_CONTEXT *ctx = mssim_tcti_init (NULL);
    assert_true (ctx == NULL);
}

static void
mssim_tcti_init_calloc_fail (void **state)
{
    will_return (__wrap_Tss2_Tcti_Mssim_Init, 10);
    will_return (__wrap_Tss2_Tcti_Mssim_Init, TSS2_RC_SUCCESS);
    will_return (__wrap_calloc, NULL);
    TSS2_TCTI_CONTEXT *ctx = mssim_tcti_init (NULL);
    assert_true (ctx == NULL);
}

#define TCTI_CTX 0x666
static void
mssim_tcti_init_second_fail (void **state)
{
    will_return (__wrap_Tss2_Tcti_Mssim_Init, 10);
    will_return (__wrap_Tss2_Tcti_Mssim_Init, TSS2_RC_SUCCESS);
    will_return (__wrap_calloc, TCTI_CTX);
    will_return (__wrap_Tss2_Tcti_Mssim_Init, 10);
    will_return (__wrap_Tss2_Tcti_Mssim_Init, 1);
    TSS2_TCTI_CONTEXT *ctx = mssim_tcti_init (NULL);
    assert_true (ctx == NULL);
}

static void
mssim_tcti_init_success (void **state)
{
    will_return (__wrap_Tss2_Tcti_Mssim_Init, 10);
    will_return (__wrap_Tss2_Tcti_Mssim_Init, TSS2_RC_SUCCESS);
    will_return (__wrap_calloc, TCTI_CTX);
    will_return (__wrap_Tss2_Tcti_Mssim_Init, 10);
    will_return (__wrap_Tss2_Tcti_Mssim_Init, TSS2_RC_SUCCESS);
    TSS2_TCTI_CONTEXT *ctx = mssim_tcti_init (NULL);
    assert_true (ctx == (void*)TCTI_CTX);
}

int
main (void)
{
    const struct CMUnitTest tests [] = {
        cmocka_unit_test (mssim_tcti_init_first_fail),
        cmocka_unit_test (mssim_tcti_init_calloc_fail),
        cmocka_unit_test (mssim_tcti_init_second_fail),
        cmocka_unit_test (mssim_tcti_init_success),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
