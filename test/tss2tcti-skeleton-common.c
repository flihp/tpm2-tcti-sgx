#include <stdio.h>
#include <tss2/tpm20.h>
#include <tss2tcti-skeleton.h>
#include "tss2tcti-skeleton_priv.h"
#include "tss2tcti-skeleton-common.h"

void
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

void
tss2_tcti_struct_teardown (void **state)
{
    TSS2_TCTI_CONTEXT *context = *state;

    tss2_tcti_finalize (context);
    if (context)
        free (context);
}
