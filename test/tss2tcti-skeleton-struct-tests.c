#include <errno.h>
#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tpm20.h>
#include <tss2tcti-skeleton.h>
#include "tss2tcti-skeleton_priv.h"

static void
tss2_tcti_struct_setup (void **state)
{
    TSS2_TCTI_CONTEXT *context = NULL;
    TSS2_RC ret = TSS2_RC_SUCCESS;
    size_t tcti_size = 0;

    ret = tss2_tcti_skeleton_init (NULL, &tcti_size);
    if (ret != TSS2_RC_SUCCESS) {
        printf ("tss2_tcti_skeleton_init failed: %d\n", ret);
        return;
    }
    context = calloc (1, tcti_size);
    if (context == NULL) {
        perror ("calloc");
        return;
    }
    ret = tss2_tcti_skeleton_init (context, 0);
    if (ret != TSS2_RC_SUCCESS) {
        printf ("tss2_tcti_skeleton_init failed: %d\n", ret);
        return;
    }
    *state = context;
}

static void
tss2_tcti_struct_teardown (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    if (context)
        free (context);
}

/* Ensure that after initialization the 'magic' value in the TCTI structure is
 * the one that we expect.
 */
static void
tss2_tcti_struct_magic_test (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;
    assert_int_equal (tss2_tcti_context_magic (context), TSS2_TCTI_SKELETON_MAGIC);
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
