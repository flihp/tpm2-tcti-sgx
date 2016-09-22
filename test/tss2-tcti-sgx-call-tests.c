#include <errno.h>
#include <stdio.h>
#include <sgx_error.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tpm20.h>
#include <tss2-tcti-sgx.h>
#include "tss2-tcti-sgx_priv.h"
#include "tss2-tcti-sgx-common.h"

/* This test module makes calls to all functions in the TSS2_TCTI_CONTEXT
 * function pointer table. Since this is just a sgx implementation each
 * function should return TSS2_TCTI_RC_NOT_IMPLEMENTED.
 * We explicitly do not call the finalize function since it has no return
 * value.
 */
static void
tss2_tcti_call_transmit_success_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t command;

    will_return (__wrap_tss2_tcti_sgx_transmit_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tss2_tcti_sgx_transmit_ocall, SGX_SUCCESS);

    assert_int_equal (tss2_tcti_sgx_transmit (context, size, &command),
                      TSS2_RC_SUCCESS);
}
/**
 * In this case we force a failure in the transmit ocall function. In this
 * case the TSS2_RC value is garbage and won't be returned. Instead the
 * transmit function attempts to map the sgx_status_t value to a TSS2_RC.
 * Currently all SGX errors map to TSS2_TCTI_RC_GENERAL_FAILURE.
 */
static void
tss2_tcti_call_transmit_sgx_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t command;

    will_return (__wrap_tss2_tcti_sgx_transmit_ocall, TSS2_RC_SUCCESS);
    will_return (__wrap_tss2_tcti_sgx_transmit_ocall, SGX_ERROR_OUT_OF_EPC);

    assert_int_equal (tss2_tcti_sgx_transmit (context, size, &command),
                      TSS2_TCTI_RC_GENERAL_FAILURE);
}
/**
 * This test case uses the mock tss2_tcti_sgx_transmit function to cause
 * success in the SGX function, but failure in the TCTI layer. We're
 * actually using the wrong layer for the response code but the end result
 * is the same.
 */
static void
tss2_tcti_call_transmit_tcti_fail_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t command;

    will_return (__wrap_tss2_tcti_sgx_transmit_ocall, TSS2_SYS_RC_BAD_REFERENCE);
    will_return (__wrap_tss2_tcti_sgx_transmit_ocall, SGX_SUCCESS);

    assert_int_equal (tss2_tcti_sgx_transmit (context, size, &command),
                      TSS2_SYS_RC_BAD_REFERENCE);
}
/**
 * This test intentionally sets the state of the context to the
 * READY_TO_RECEIVE state, then calls the transmit function. This should
 * cause the function to give us a return code indicating a bad sequence.
 */
static void
tss2_tcti_call_transmit_bad_sequence_test (void **state)
{
    TSS2_TCTI_CONTEXT     *context     = *state;
    TSS2_TCTI_CONTEXT_SGX *sgx_context = *state;

    TSS2_TCTI_SGX_STATE (sgx_context) = READY_TO_RECEIVE;
    assert_int_equal (tss2_tcti_sgx_transmit (context, 0, NULL),
                      TSS2_TCTI_RC_BAD_SEQUENCE);
}
/**
 *
 */
static void
tss2_tcti_call_receive_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t response;
    uint32_t timeout;

    assert_int_equal (tss2_tcti_sgx_receive (context, &size, &response, timeout),
                      TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
static void
tss2_tcti_call_cancel_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;

    assert_int_equal (tss2_tcti_sgx_cancel (context),
                      TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
static void
tss2_tcti_call_get_poll_handles_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TSS2_TCTI_POLL_HANDLE handles;
    size_t num_handles;
    TSS2_RC rc;

    rc = tss2_tcti_sgx_get_poll_handles (context, &handles, &num_handles);
    assert_int_equal (rc, TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
static void
tss2_tcti_call_set_locality_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    uint8_t locality;

    assert_int_equal (tss2_tcti_sgx_set_locality (context, locality),
                      TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
int
main(int argc, char* argv[])
{
    const UnitTest tests[] = {
        unit_test_setup_teardown (tss2_tcti_call_transmit_success_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
        unit_test_setup_teardown (tss2_tcti_call_transmit_sgx_fail_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
        unit_test_setup_teardown (tss2_tcti_call_transmit_tcti_fail_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
        unit_test_setup_teardown (tss2_tcti_call_transmit_bad_sequence_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
        unit_test_setup_teardown (tss2_tcti_call_receive_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
        unit_test_setup_teardown (tss2_tcti_call_cancel_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
        unit_test_setup_teardown (tss2_tcti_call_get_poll_handles_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
        unit_test_setup_teardown (tss2_tcti_call_set_locality_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
    };
    return run_tests(tests);
}
