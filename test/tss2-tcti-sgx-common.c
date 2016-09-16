#include <stdio.h>
#include <sgx_error.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tpm20.h>
#include <tss2-tcti-sgx.h>
#include "tss2-tcti-sgx_priv.h"
#include "tss2-tcti-sgx-common.h"

void
tss2_tcti_struct_setup (void **state)
{
    TSS2_TCTI_CONTEXT *context = NULL;
    TSS2_RC ret = TSS2_RC_SUCCESS;
    size_t tcti_size = 0;

    ret = tss2_tcti_sgx_init (NULL, &tcti_size);
    if (ret != TSS2_RC_SUCCESS) {
        printf ("tss2_tcti_sgx_init failed: %d\n", ret);
        return;
    }
    context = calloc (1, tcti_size);
    if (context == NULL) {
        perror ("calloc");
        return;
    }
    /**
     * prime data for mock ocall:
     *   OCall returns an ID of 1
     *   OCall return value indicates success
     */
    will_return (__wrap_tss2_tcti_sgx_init_ocall, 1);
    will_return (__wrap_tss2_tcti_sgx_init_ocall, SGX_SUCCESS);
    ret = tss2_tcti_sgx_init (context, 0);
    if (ret != TSS2_RC_SUCCESS) {
        printf ("tss2_tcti_sgx_init failed: %d\n", ret);
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
