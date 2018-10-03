/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>
#include <cmocka.h>

#include <sgx_error.h>

#include <tss2/tss2_tpm2_types.h>

#include "tss2-tcti-sgx.h"
#include "tcti-sgx_priv.h"

/*
 * when given an NULL context and a pointer to a size_t, set the size_t
 * parameter to the size of the TSS2_TCTI_CONTEXT_SGX structure.
 */
static void
tcti_sgx_init_size_test (void **state)
{
    size_t tcti_size;

    Tss2_Tcti_Sgx_Init (NULL, &tcti_size);
    assert_int_equal (tcti_size, sizeof (TCTI_CONTEXT_SGX));
}
/*
 * when given a NULL context and a pointer to size_t, the init function
 * returns TSS2_RC_SUCCESS
 */
static void
tcti_sgx_init_success_return_value_test (void **state)
{
    size_t tcti_size;
    TSS2_RC ret;

    ret = Tss2_Tcti_Sgx_Init (NULL, &tcti_size);
    assert_int_equal (ret, TSS2_RC_SUCCESS);
}
/*
 * when given NULL pointers for both parameters the init function returns
 * an error indicating that the values are bad (TSS2_TCTI_RC_BAD_VALUE)
 */
static void
tcti_sgx_init_allnull_is_bad_value (void **state)
{
    TSS2_RC ret;

    ret = Tss2_Tcti_Sgx_Init (NULL, NULL);
    assert_int_equal (ret, TSS2_TCTI_RC_BAD_VALUE);
}
/*
 * When given a non-NULL context the init function should set up various
 * bits of data in the context structure and return a success indicator.
 * This requires an ocall to register the context with the RM/TAB in the
 * host application. This is mocked to return success.
 * NOTE: may be useful to check the internals of the context structure
 *       to verify the init function set things up right.
 */
static void
tcti_sgx_init_success_mock_ocall (void **state)
{
    TSS2_RC ret;
    TSS2_TCTI_CONTEXT *ctx = calloc (1, sizeof (TCTI_CONTEXT_SGX));

    assert_non_null (ctx);
    will_return (__wrap_tcti_sgx_init_ocall, 1);
    will_return (__wrap_tcti_sgx_init_ocall, SGX_SUCCESS);

    ret = Tss2_Tcti_Sgx_Init (ctx, NULL);
    assert_int_equal (ret, TSS2_RC_SUCCESS);
}
int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test (tcti_sgx_init_size_test),
        cmocka_unit_test (tcti_sgx_init_success_return_value_test),
        cmocka_unit_test (tcti_sgx_init_allnull_is_bad_value),
        cmocka_unit_test (tcti_sgx_init_success_mock_ocall),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
