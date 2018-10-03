/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <errno.h>
#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tss2_tpm2_types.h>

#include "tss2-tcti-sgx.h"
#include "tcti-sgx_priv.h"
#include "tcti-sgx-common.h"

/**
 * This module tests the state of the TSS2_TCTI_CONTEXT_SGX structure
 * after its initialization. This boils down to checking that the meta
 * data fields are the expected values and that the
 * TSS2_TCTI_CONTEXT_COMMON_V1 function pointers are pointing at the
 * correct functions.
 */

/* Ensure that after initialization the 'magic' value in the TCTI structure is
 * the one that we expect.
 */
static void
tcti_struct_magic_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (TSS2_TCTI_MAGIC (context), TCTI_SGX_MAGIC);
}

/* Ensure that after initialization the 'version' value in the TCTI structure is
 * the one that we expect.
 */
static void
tcti_struct_version_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (TSS2_TCTI_VERSION (context), 1);
}
/**
 * After initialization, ensure that the cancel function pointer in the
 * TCTI structure is set to the 'tcti_sgx_cancel' function.
 */
static void
tcti_struct_cancel_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (TSS2_TCTI_CANCEL (context),
                      tcti_sgx_cancel);
}
/**
 * After initialization, ensure that the finalize function pointer in the
 * TCTI structure is set to the 'tcti_sgx_finalize' function.
 */
static void
tcti_struct_finalize_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (TSS2_TCTI_FINALIZE (context),
                      tcti_sgx_finalize);
}
/**
 * After initialization, ensure that the getPollHandles function pointer
 * in the TCTI structure is set to the 'tcti_sgx_get_poll_handles'
 * function.
 */
static void
tcti_struct_get_poll_handles_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (TSS2_TCTI_GET_POLL_HANDLES (context),
                      tcti_sgx_get_poll_handles);
}
/**
 * After initialization, ensure that the receive function pointer in the
 * TCTI structure is set to the 'tcti_sgx_receive' function.
 */
static void
tcti_struct_receive_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (TSS2_TCTI_RECEIVE (context),
                      tcti_sgx_receive);
}
/**
 * After initialization, ensure that the setLocality function pointer in
 * the TCTI structure is set to the 'tcti_sgx_set_locality'
 * function.
 */
static void
tcti_struct_set_locality_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (TSS2_TCTI_SET_LOCALITY (context),
                      tcti_sgx_set_locality);
}
/**
 * After initialization, ensure that the transmit function pointer in
 * the TCTI structure is set to the 'tcti_sgx_transmit' function.
 */
static void
tcti_struct_transmit_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (TSS2_TCTI_TRANSMIT (context),
                      tcti_sgx_transmit);
}
/**
 * After initialization, ensure that the id field in the TCTI structure
 * is set the value 1. This value is specified in the initialization
 * function (though that's not ideal.
 */
static void
tcti_struct_id_test (void **state)
{
    TCTI_CONTEXT_SGX *context = *state;
    assert_int_equal (TCTI_SGX_ID (context), 1);
}
/**
 * After initialization, ensure that the state field in the TCTI structure
 * is set the value READY_TO_TRANSMIT.
 */
static void
tcti_struct_state_test (void **state)
{
    TCTI_CONTEXT_SGX *context = *state;
    assert_int_equal (TCTI_SGX_STATE (context), READY_TO_TRANSMIT);
}

int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown (tcti_struct_magic_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_version_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_cancel_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_finalize_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_get_poll_handles_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_receive_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_set_locality_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_transmit_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_id_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_struct_state_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
