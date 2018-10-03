/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <errno.h>
#include <stdio.h>
#include <sgx_error.h>

#include <setjmp.h>
#include <cmocka.h>

#include "tss2-tcti-sgx.h"
#include "tcti-sgx_priv.h"
#include "tcti-sgx-common.h"

/* This test module makes calls to all functions in the TSS2_TCTI_CONTEXT
 * function pointer table. Since this is just a sgx implementation each
 * function should return TSS2_TCTI_RC_NOT_IMPLEMENTED.
 * We explicitly do not call the finalize function since it has no return
 * value.
 */
static void
tcti_call_transmit_success_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t command;

    will_return (__wrap_tcti_sgx_transmit_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tcti_sgx_transmit_ocall, SGX_SUCCESS);

    assert_int_equal (tcti_sgx_transmit (context, size, &command),
                      TSS2_RC_SUCCESS);
}
/**
 * In this case we force a failure in the transmit ocall function. In this
 * case the TSS2_RC value is garbage and won't be returned. Instead the
 * transmit function attempts to map the sgx_status_t value to a TSS2_RC.
 * Currently all SGX errors map to TSS2_TCTI_RC_GENERAL_FAILURE.
 */
static void
tcti_call_transmit_sgx_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t command;

    will_return (__wrap_tcti_sgx_transmit_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tcti_sgx_transmit_ocall, SGX_ERROR_OUT_OF_EPC);

    assert_int_equal (tcti_sgx_transmit (context, size, &command),
                      TSS2_TCTI_RC_GENERAL_FAILURE);
}
/**
 * This test case uses the mock tcti_sgx_transmit function to cause
 * success in the SGX function, but failure in the TCTI layer. We're
 * actually using the wrong layer for the response code but the end result
 * is the same.
 */
static void
tcti_call_transmit_tcti_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t command;

    will_return (__wrap_tcti_sgx_transmit_ocall, TSS2_SYS_RC_BAD_REFERENCE);
    will_return (__wrap_tcti_sgx_transmit_ocall, SGX_SUCCESS);

    assert_int_equal (tcti_sgx_transmit (context, size, &command),
                      TSS2_SYS_RC_BAD_REFERENCE);
}
/**
 * This test intentionally sets the state of the context to the
 * READY_TO_RECEIVE state, then calls the transmit function. This should
 * cause the function to give us a return code indicating a bad sequence.
 */
static void
tcti_call_transmit_bad_sequence_test (void **state)
{
    TSS2_TCTI_CONTEXT     *context     = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;

    TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    assert_int_equal (tcti_sgx_transmit (context, 0, NULL),
                      TSS2_TCTI_RC_BAD_SEQUENCE);
}
/**
 * This test sets up the common path in the receive function. This is
 * defined by both the SGX ocall and the external TCTI returning success
 * codes.
 */
static void
tcti_call_receive_success_test (void **state)
{
    TSS2_TCTI_CONTEXT     *context     = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;
    size_t   size;
    uint8_t  response;
    uint32_t timeout;
    TSS2_RC  rc;

    will_return (__wrap_tcti_sgx_receive_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tcti_sgx_receive_ocall, SGX_SUCCESS);

    TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    rc = tcti_sgx_receive (context, &size, &response, timeout);
    assert_int_equal (rc, TSS2_RC_SUCCESS);
}
/**
 * In this test we force the ocall to return an SGX error. In this case
 * we should get a generic TCTI error code.
 */
static void
tcti_call_receive_sgx_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;
    size_t   size;
    uint8_t  response;
    uint32_t timeout;
    TSS2_RC  rc;

    will_return (__wrap_tcti_sgx_receive_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tcti_sgx_receive_ocall, SGX_ERROR_OUT_OF_EPC);

    TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    rc = tcti_sgx_receive (context, &size, &response, timeout);
    assert_int_equal (rc, TSS2_TCTI_RC_GENERAL_FAILURE);
}
/**
 * This test forces a failure status code to be returned in the TCTI RC
 * that comes from the TCTI outside the enclave while the ocall succeeds.
 * This should cause the call to _receive to return the RC.
 */
static void
tcti_call_receive_tcti_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;
    size_t size;
    uint8_t response;
    uint32_t timeout;
    TSS2_RC rc;

    will_return (__wrap_tcti_sgx_receive_ocall, TSS2_TCTI_RC_BAD_REFERENCE);
    will_return (__wrap_tcti_sgx_receive_ocall, SGX_SUCCESS);

    TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    rc = tcti_sgx_receive (context, &size, &response, timeout);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_REFERENCE);
}
/**
 * The TCTI context must be READY_TO_RECEIVE when we call the _receive
 * function. In this test we don't set this value so it's the default
 * value from the init function which is READY_TO_TRANSMIT. So a call
 * to _receive after init should cause a BAD_SEQUENCE error.
 */
static void
tcti_call_receive_bad_sequence_test (void **state)
{
    TSS2_TCTI_CONTEXT     *context     = *state;
    size_t size;
    uint8_t response;
    uint32_t timeout;
    TSS2_RC rc;

    rc = tcti_sgx_receive (context, &size, &response, timeout);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_SEQUENCE);
}
/**
 * This tests the default case for the cancel command. The mock cancel
 * function will return success for both the SGX command and the external
 * TCTI. We also set the context state to READY_TO_RECEIVE indicating
 * that a command has been sent and can be canceled.
 */
static void
tcti_call_cancel_success_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;
    TSS2_RC rc;

    will_return (__wrap_tcti_sgx_cancel_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tcti_sgx_cancel_ocall, SGX_SUCCESS);

    TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    rc = tcti_sgx_cancel (context);
    assert_int_equal (rc, TSS2_RC_SUCCESS);
}
/**
 * This tests the code path for handling errors from the SGX ocall for
 * the cancel command. In this case the SGX error is transport specific
 * and is mapped to a generic TCTI error code.
 */
static void
tcti_call_cancel_sgx_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;
    TSS2_RC rc;

    will_return (__wrap_tcti_sgx_cancel_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tcti_sgx_cancel_ocall, SGX_ERROR_UNEXPECTED);

    TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    rc = tcti_sgx_cancel (context);
    assert_int_equal (rc, TSS2_TCTI_RC_GENERAL_FAILURE);
}
/**
 * This tests the code path for handling errors from the TCTI outside the
 * enclave. In this case the SGX ocall succeeds. The returned TCTI error
 * code should be the same one passed to the mock ocall.
 */
static void
tcti_call_cancel_tcti_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;
    TSS2_RC rc;

    will_return (__wrap_tcti_sgx_cancel_ocall, TSS2_TCTI_RC_NOT_IMPLEMENTED);
    will_return (__wrap_tcti_sgx_cancel_ocall, SGX_SUCCESS);

    TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    rc = tcti_sgx_cancel (context);
    assert_int_equal (rc, TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
/**
 * This tests the code path for handling an error condition where the caller
 * invokes the cancel command without having a command to cancel. The context
 * must be in the READY_TO_RECEIVE state for a cancel call to be make. Here
 * the context is freshly initialized and so it's in the READY_TO_TRANSMIT
 * state. Calling cancel in this state should cause the function to return
 * a BAD_SEQUENCE error code.
 */
static void
tcti_call_cancel_bad_sequence_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;
    TSS2_RC rc;

    rc = tcti_sgx_cancel (context);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_SEQUENCE);
}
/**
 * The concept of polling (like POSIX 'select' / 'poll') doesn't work
 * across the enclave boundary. It's not implemented.
 */
static void
tcti_call_get_poll_handles_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TSS2_TCTI_POLL_HANDLE handles;
    size_t num_handles;
    TSS2_RC rc;

    rc = tcti_sgx_get_poll_handles (context, &handles, &num_handles);
    assert_int_equal (rc, TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
/**
 * This tests the common case for the cancel command. The mock function
 * is set to return success for both SGX and the external TCTI. The context
 * state is left in the READY_TO_RECEIVE state.
 */
static void
tcti_call_set_locality_success_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    uint8_t locality;
    TSS2_RC rc;

    will_return (__wrap_tcti_sgx_set_locality_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tcti_sgx_set_locality_ocall, SGX_SUCCESS);
    rc = tcti_sgx_set_locality (context, locality);
    assert_int_equal (rc, TSS2_RC_SUCCESS);
}
/**
 * This tests the return path for errors originating from the SGX ocall.
 * Just like the other ocalls, setLocality maps all SGX error codes to the
 * generic TSS2_TCTI_RC_GENERAL_FAILURE so any SGX erorr code (that we
 * induce with the mock ocall) should produce this TSS2_RC.
 */
static void
tcti_call_set_locality_sgx_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    uint8_t locality;
    TSS2_RC rc;

    will_return (__wrap_tcti_sgx_set_locality_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tcti_sgx_set_locality_ocall, SGX_ERROR_OCALL_NOT_ALLOWED);
    rc = tcti_sgx_set_locality (context, locality);
    assert_int_equal (rc, TSS2_TCTI_RC_GENERAL_FAILURE);
}
/**
 * This tests the return path for errors originating from the TCTI that
 * are propagated across the enclave boundary back to us from the external
 * TCTI. This requires that we setup the mock function to get a success
 * from SGX but an error from the TCTI.
 */
static void
tcti_call_set_locality_tcti_fail_test (void **state)
{

    TSS2_TCTI_CONTEXT *context = *state;
    uint8_t locality;
    TSS2_RC rc;

    will_return (__wrap_tcti_sgx_set_locality_ocall, TSS2_TCTI_RC_NOT_PERMITTED);
    will_return (__wrap_tcti_sgx_set_locality_ocall, SGX_SUCCESS);
    rc = tcti_sgx_set_locality (context, locality);
    assert_int_equal (rc, TSS2_TCTI_RC_NOT_PERMITTED);
}
/**
 * This tests the TCTI state machine: when we set the locality we should
 * not be waiting for a response to a previous command. So the state should
 * be READY_TO_TRANSMIT.
 */
static void
tcti_call_set_locality_bad_sequence_test (void **state)
{

    TSS2_TCTI_CONTEXT     *context     = *state;
    TCTI_CONTEXT_SGX *sgx_context = *state;
    uint8_t locality;
    TSS2_RC rc;

    TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    rc = tcti_sgx_set_locality (context, locality);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_SEQUENCE);
}
int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown (tcti_call_transmit_success_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_transmit_sgx_fail_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_transmit_tcti_fail_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_transmit_bad_sequence_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_receive_success_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_receive_sgx_fail_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_receive_tcti_fail_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_receive_bad_sequence_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_cancel_success_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_cancel_sgx_fail_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_cancel_tcti_fail_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_cancel_bad_sequence_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_get_poll_handles_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_set_locality_success_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_set_locality_sgx_fail_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_set_locality_tcti_fail_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
        cmocka_unit_test_setup_teardown (tcti_call_set_locality_bad_sequence_test,
                                         tcti_struct_setup,
                                         tcti_struct_teardown),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
