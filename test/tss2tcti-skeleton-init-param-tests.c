#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tpm20.h>
#include <tss2tcti-skeleton.h>
#include "tss2tcti-skeleton_priv.h"

/* when given an NULL context and a pointer to a size_t, set the size_t
 * parameter to the size of the TSS2_TCTI_SKELETON_CONTEXT structure.
 */
static void
tss2_tcti_skeleton_init_size_test (void **state)
{
    size_t tcti_size;

    tss2_tcti_skeleton_init (NULL, &tcti_size);
    assert_int_equal (tcti_size, sizeof (TSS2_TCTI_SKELETON_CONTEXT));
}
/* when given a NULL context and a pointer to size_t, the init function
 * returns TSS2_RC_SUCCESS
 */
static void
tss2_tcti_skeleton_init_success_return_value_test (void **state)
{
    size_t tcti_size;
    TSS2_RC ret;

    ret = tss2_tcti_skeleton_init (NULL, &tcti_size);
    assert_int_equal (ret, TSS2_RC_SUCCESS);
}
/* when given NULL pointers for both parameters the init function returns
 * an error indicating that the values are bad (TSS2_TCTI_RC_BAD_VALUE)
 */
static void
tss2_tcti_skeleton_init_allnull_is_bad_value (void **state)
{
    TSS2_RC ret;

    ret = tss2_tcti_skeleton_init (NULL, NULL);
    assert_int_equal (ret, TSS2_TCTI_RC_BAD_VALUE);
}

int
main(int argc, char* argv[])
{
    const UnitTest tests[] = {
        unit_test(tss2_tcti_skeleton_init_size_test),
        unit_test(tss2_tcti_skeleton_init_success_return_value_test),
        unit_test(tss2_tcti_skeleton_init_allnull_is_bad_value),
    };
    return run_tests(tests);
}
