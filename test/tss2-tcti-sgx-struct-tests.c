#include <errno.h>
#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tpm20.h>
#include <tss2-tcti-sgx.h>
#include "tss2-tcti-sgx_priv.h"
#include "tss2-tcti-sgx-common.h"

/* Ensure that after initialization the 'magic' value in the TCTI structure is
 * the one that we expect.
 */
static void
tss2_tcti_struct_magic_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (tss2_tcti_context_magic (context), TSS2_TCTI_SGX_MAGIC);
}

/* Ensure that after initialization the 'version' value in the TCTI structure is
 * the one that we expect.
 */
static void
tss2_tcti_struct_version_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (tss2_tcti_context_version (context), 1);
}

int
main(int argc, char* argv[])
{
    const UnitTest tests[] = {
        unit_test_setup_teardown (tss2_tcti_struct_magic_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
        unit_test_setup_teardown (tss2_tcti_struct_version_test,
                                  tss2_tcti_struct_setup,
                                  tss2_tcti_struct_teardown),
    };
    return run_tests(tests);
}
