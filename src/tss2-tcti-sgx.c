#include <tss2-tcti-sgx.h>
#include "tss2-tcti-sgx_priv.h"

static TSS2_RC
tss2_tcti_sgx_transmit (TSS2_TCTI_CONTEXT *tcti_context,
                             size_t size,
                             uint8_t *command)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

static TSS2_RC
tss2_tcti_sgx_receive (TSS2_TCTI_CONTEXT *tcti_context,
                            size_t *size,
                            uint8_t *response,
                            int32_t timeout)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

static void
tss2_tcti_sgx_finalize (TSS2_TCTI_CONTEXT *tcti_context)
{
}

static TSS2_RC
tss2_tcti_sgx_cancel (TSS2_TCTI_CONTEXT *tcti_context)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

static TSS2_RC
tss2_tcti_sgx_get_poll_handles (TSS2_TCTI_CONTEXT *tcti_context,
                                     TSS2_TCTI_POLL_HANDLE *handles,
                                     size_t *num_handles)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

static TSS2_RC
tss2_tcti_sgx_set_locality (TSS2_TCTI_CONTEXT *tcti_context, uint8_t locality)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

TSS2_RC
tss2_tcti_sgx_init (TSS2_TCTI_CONTEXT *tcti_context, size_t *size)
{
    if (tcti_context == NULL && size == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    if (tcti_context == NULL && size != NULL) {
        *size = sizeof (TSS2_TCTI_CONTEXT_SGX);
        return TSS2_RC_SUCCESS;
    }
    tss2_tcti_context_magic (tcti_context) = TSS2_TCTI_SGX_MAGIC;
    tss2_tcti_context_version (tcti_context) = 1;
    tss2_tcti_context_transmit (tcti_context) = tss2_tcti_sgx_transmit;
    tss2_tcti_context_receive (tcti_context) = tss2_tcti_sgx_receive;
    tss2_tcti_context_finalize (tcti_context) = tss2_tcti_sgx_finalize;
    tss2_tcti_context_cancel (tcti_context) = tss2_tcti_sgx_cancel;
    tss2_tcti_context_get_poll_handles (tcti_context) = tss2_tcti_sgx_get_poll_handles;
    tss2_tcti_context_set_locality (tcti_context) = tss2_tcti_sgx_set_locality;

    return TSS2_RC_SUCCESS;
}
