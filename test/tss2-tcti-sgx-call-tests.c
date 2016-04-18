#include <errno.h>
#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tpm20.h>
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
tss2_tcti_call_transmit_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t command;

    assert_int_equal (tss2_tcti_transmit (context, size, &command),
                      TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
static void
tss2_tcti_call_receive_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    size_t size;
    uint8_t response;
    uint32_t timeout;

    assert_int_equal (tss2_tcti_receive (context, &size, &response, timeout),
                      TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
static void
tss2_tcti_call_cancel_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;

    assert_int_equal (tss2_tcti_cancel (context),
                      TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
static void
tss2_tcti_call_get_poll_handles_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    TSS2_TCTI_POLL_HANDLE handles;
    size_t num_handles;

    assert_int_equal (tss2_tcti_get_poll_handles (context, &handles, &num_handles),
                      TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
static void
tss2_tcti_call_set_locality_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    uint8_t locality;

    assert_int_equal (tss2_tcti_set_locality (context, locality),
                      TSS2_TCTI_RC_NOT_IMPLEMENTED);
}
int
main(int argc, char* argv[])
{
    const UnitTest tests[] = {
        unit_test_setup_teardown (tss2_tcti_call_transmit_test,
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
