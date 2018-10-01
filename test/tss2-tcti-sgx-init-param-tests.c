#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include <sgx_error.h>

#include <tss2/tss2_tpm2_types.h>

#include <tss2-tcti-sgx.h>
#include "tss2-tcti-sgx_priv.h"

/* when given an NULL context and a pointer to a size_t, set the size_t
 * parameter to the size of the TSS2_TCTI_CONTEXT_SGX structure.
 */
static void
tss2_tcti_sgx_init_size_test (void **state)
{
    size_t tcti_size;

    tss2_tcti_sgx_init (NULL, &tcti_size);
    assert_int_equal (tcti_size, sizeof (TSS2_TCTI_CONTEXT_SGX));
}
/* when given a NULL context and a pointer to size_t, the init function
 * returns TSS2_RC_SUCCESS
 */
static void
tss2_tcti_sgx_init_success_return_value_test (void **state)
{
    size_t tcti_size;
    TSS2_RC ret;

    ret = tss2_tcti_sgx_init (NULL, &tcti_size);
    assert_int_equal (ret, TSS2_RC_SUCCESS);
}
/* when given NULL pointers for both parameters the init function returns
 * an error indicating that the values are bad (TSS2_TCTI_RC_BAD_VALUE)
 */
static void
tss2_tcti_sgx_init_allnull_is_bad_value (void **state)
{
    TSS2_RC ret;

    ret = tss2_tcti_sgx_init (NULL, NULL);
    assert_int_equal (ret, TSS2_TCTI_RC_BAD_VALUE);
}
/**
 * When given a non-NULL context the init function should set up various
 * bits of data in the context structure and return a success indicator.
 * This requires an ocall to register the context with the RM/TAB in the
 * host application. This is mocked to return success.
 * NOTE: may be useful to check the internals of the context structure
 *       to verify the init function set things up right.
 */
static void
tss2_tcti_sgx_init_success_mock_ocall (void **state)
{
    TSS2_RC ret;
    TSS2_TCTI_CONTEXT *ctx = calloc (1, sizeof (TSS2_TCTI_CONTEXT_SGX));

    assert_non_null (ctx);
    will_return (__wrap_tss2_tcti_sgx_init_ocall, 1);
    will_return (__wrap_tss2_tcti_sgx_init_ocall, SGX_SUCCESS);

    ret = tss2_tcti_sgx_init (ctx, NULL);
    assert_int_equal (ret, TSS2_RC_SUCCESS);
}
int
main(int argc, char* argv[])
{
    const UnitTest tests[] = {
        unit_test(tss2_tcti_sgx_init_size_test),
        unit_test(tss2_tcti_sgx_init_success_return_value_test),
        unit_test(tss2_tcti_sgx_init_allnull_is_bad_value),
        unit_test(tss2_tcti_sgx_init_success_mock_ocall),
    };
    return run_tests(tests);
}
