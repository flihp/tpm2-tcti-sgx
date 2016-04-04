#include <tss2tcti-skeleton.h>
#include "tss2tcti-skeleton_priv.h"

static TSS2_RC
tss2_tcti_skeleton_transmit (TSS2_TCTI_CONTEXT *tcti_context,
                             size_t size,
                             uint8_t *command)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

static TSS2_RC
tss2_tcti_skeleton_receive (TSS2_TCTI_CONTEXT *tcti_context,
                            size_t *size,
                            uint8_t *response,
                            int32_t timeout)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

static void
tss2_tcti_skeleton_finalize (TSS2_TCTI_CONTEXT *tcti_context)
{
}

static TSS2_RC
tss2_tcti_skeleton_cancel (TSS2_TCTI_CONTEXT *tcti_context)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

static TSS2_RC
tss2_tcti_skeleton_get_poll_handles (TSS2_TCTI_CONTEXT *tcti_context,
                                     TSS2_TCTI_POLL_HANDLE *handles,
                                     size_t *num_handles)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

static TSS2_RC
tss2_tcti_skeleton_set_locality (TSS2_TCTI_CONTEXT *tcti_context, uint8_t locality)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

TSS2_RC
tss2_tcti_skeleton_init (TSS2_TCTI_CONTEXT *tcti_context, size_t *size)
{
    if (tcti_context == NULL && size == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    if (tcti_context == NULL && size != NULL) {
        *size = sizeof (TSS2_TCTI_SKELETON_CONTEXT);
        return TSS2_RC_SUCCESS;
    }
    tss2_tcti_context_magic (tcti_context) = TSS2_TCTI_SKELETON_MAGIC;
    tss2_tcti_context_version (tcti_context) = 1;
    tss2_tcti_context_transmit (tcti_context) = tss2_tcti_skeleton_transmit;
    tss2_tcti_context_receive (tcti_context) = tss2_tcti_skeleton_receive;
    tss2_tcti_context_finalize (tcti_context) = tss2_tcti_skeleton_finalize;
    tss2_tcti_context_cancel (tcti_context) = tss2_tcti_skeleton_cancel;
    tss2_tcti_context_get_poll_handles (tcti_context) = tss2_tcti_skeleton_get_poll_handles;
    tss2_tcti_context_set_locality (tcti_context) = tss2_tcti_skeleton_set_locality;

    return TSS2_RC_SUCCESS;
}
