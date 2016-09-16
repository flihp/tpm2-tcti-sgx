#include <tss2-tcti-sgx.h>
#include "tss2-tcti-sgx_priv.h"
#include "tss2_tcti_sgx_t.h"

/**
 * Simple accessor. Not sure it's any more simple than directly accessing
 * the size field.
 */
size_t
sizeof_sized_buf (const struct sized_buf *sbuf)
{
    return sbuf->size;
}
/**
 * This is the function that is hooked into the standard TSS2_TCTI_CONTEXT
 * transmit function pointer. This function will be invoked when:
 * - TSS2_TCTI_CONTEXT object is initialized for the SGX TCTI
 * - the tss2_tcti_transmit (ctx ...) is invoked with said context
 * It's also possible to invoke this function directly but that should be
 * very rare.
 * This function returns:
 * - TSS2_TCTI_RC_BAD_REFERENCE when the context is NULL
 * - TSS2_TCTI_RC_BAD_SEQUENCE  when the state machine is not in the
 *   READY_TO_TRANSMIT state
 * - TSS2_TCTI_RC_GENERAL_FAILURE when an SGX error occurs
 */
static TSS2_RC
tss2_tcti_sgx_transmit (TSS2_TCTI_CONTEXT *tcti_context,
                        size_t             size,
                        uint8_t           *command)
{
    sized_buf buf = { size, command };
    sgx_status_t status;
    TSS2_RC retval;

    if (tcti_context == NULL)
        return TSS2_TCTI_RC_BAD_REFERENCE;
    if (TSS2_TCTI_SGX_STATE (tcti_context) != READY_TO_TRANSMIT)
        return TSS2_TCTI_RC_BAD_SEQUENCE;

    status = tss2_tcti_sgx_transmit_ocall (&retval,
                                           TSS2_TCTI_SGX_ID (tcti_context),
                                           &buf);
    /**
     * Map SGX error codes to TSS2_RC error codes. If no SGX error return
     * the 'retval' parameter that contains the TSS2_RC value from outside
     * the enclave.
     */
    if (status == SGX_SUCCESS)
        return retval;
    else
        return TSS2_TCTI_RC_GENERAL_FAILURE;
}
/**
 * This is the function that is hooked into the standard TSS2_TCTI_CONTEXT
 * receive function pointer. This function will be invoked when:
 * - TSS2_TCTI_CONTEXT object is initialized for the SGX TCTI
 * - the tss2_tcti_receive (ctx ...) is invoked with said context
 * It's also possible to invoke this function directly but that should be
 * very rare.
 * This function returns:
 * - TSS2_TCTI_RC_BAD_REFERENCE when the context is NULL
 * - TSS2_TCTI_RC_BAD_SEQUENCE  when the state machine is not in the
 *   READY_TO_RECEIVE state
 * - TSS2_TCTI_RC_GENERAL_FAILURE when an SGX error occurs
 */
static TSS2_RC
tss2_tcti_sgx_receive (TSS2_TCTI_CONTEXT *tcti_context,
                       size_t            *size,
                       uint8_t           *response,
                       int32_t            timeout)
{
    sized_buf buf = { *size, response };
    sgx_status_t status;
    TSS2_RC retval;

    if (tcti_context == NULL)
        return TSS2_TCTI_RC_BAD_REFERENCE;
    if (TSS2_TCTI_SGX_STATE (tcti_context) != READY_TO_RECEIVE)
        return TSS2_TCTI_RC_BAD_SEQUENCE;

    status = tss2_tcti_sgx_receive_ocall (&retval,
                                          TSS2_TCTI_SGX_ID (tcti_context),
                                          &buf,
                                          timeout);
    if (status == SGX_SUCCESS)
        return retval;
    else
        return TSS2_TCTI_RC_GENERAL_FAILURE;
}
/**
 * This is the function that is hooked into the standard TSS2_TCTI_CONTEXT
 * finalize function pointer. This function will be invoked when:
 * - TSS2_TCTI_CONTEXT object is initialized for the SGX TCTI
 * - the tss2_tcti_finalize (ctx ...) is invoked with said context
 * It's also possible to invoke this function directly but that should be
 * very rare.
 * This function returns void so all errors are silently ignored (this is
 * required by the TSS spec).
 */
static void
tss2_tcti_sgx_finalize (TSS2_TCTI_CONTEXT *tcti_context)
{
    sgx_status_t status;

    if (tcti_context == NULL)
        return;

    status = tss2_tcti_sgx_finalize_ocall (TSS2_TCTI_SGX_ID (tcti_context));
}
/**
 * This is the function that is hooked into the standard TSS2_TCTI_CONTEXT
 * cancel function pointer. This function will be invoked when:
 * - TSS2_TCTI_CONTEXT object is initialized for the SGX TCTI
 * - the tss2_tcti_cancel (ctx ...) is invoked with said context
 * It's also possible to invoke this function directly but that should be
 * very rare.
 * This function returns:
 * - TSS2_TCTI_RC_BAD_REFERENCE when the context is NULL
 * - TSS2_TCTI_RC_BAD_SEQUENCE  when the state machine is not in the
 *   READY_TO_RECEIVE state
 * - TSS2_TCTI_RC_GENERAL_FAILURE when an SGX error occurs
 */
static TSS2_RC
tss2_tcti_sgx_cancel (TSS2_TCTI_CONTEXT *tcti_context)
{
    sgx_status_t status;
    TSS2_RC retval;

    if (tcti_context == NULL)
        return TSS2_TCTI_RC_BAD_REFERENCE;
    if (TSS2_TCTI_SGX_STATE (tcti_context) != READY_TO_RECEIVE)
        return TSS2_TCTI_RC_BAD_SEQUENCE;

    status = tss2_tcti_sgx_cancel_ocall (&retval,
                                         TSS2_TCTI_SGX_ID (tcti_context));

    if (status == SGX_SUCCESS)
        return retval;
    else
        return TSS2_TCTI_RC_GENERAL_FAILURE;
}
/**
 * This is the function that is hooked into the standard TSS2_TCTI_CONTEXT
 * getPollHandles function pointer. This function will be invoked when:
 * - TSS2_TCTI_CONTEXT object is initialized for the SGX TCTI
 * - the tss2_tcti_get_poll_handles (ctx ...) is invoked with said context
 * It's also possible to invoke this function directly but that should be
 * very rare.
 * This function returns:
 * - TSS2_TCTI_RC_BAD_REFERENCE when the context is NULL
 * - TSS2_TCTI_RC_BAD_SEQUENCE  when the state machine is not in the
 *   READY_TO_RECEIVE state
 * - TSS2_TCTI_RC_GENERAL_FAILURE when an SGX error occurs
 * NOTE: Asynchronous IO across the enclave boundary is something I haven't
 *       looked into seriously yet. The code on the other side of the
 *       enclave boundary will likely reply with TSS2_TCTI_RC_NOT_IMPLEMENTED
 *       while the ocall will still succeed.
 */
static TSS2_RC
tss2_tcti_sgx_get_poll_handles (TSS2_TCTI_CONTEXT     *tcti_context,
                                TSS2_TCTI_POLL_HANDLE *handles,
                                size_t                *num_handles)
{
    sgx_status_t status;
    TSS2_RC retval;

    if (tcti_context == NULL)
        return TSS2_TCTI_RC_BAD_REFERENCE;
    if (TSS2_TCTI_SGX_STATE (tcti_context) != READY_TO_RECEIVE)
        return TSS2_TCTI_RC_BAD_SEQUENCE;

    status =
        tss2_tcti_sgx_get_poll_handles_ocall (&retval,
                                              TSS2_TCTI_SGX_ID (tcti_context));
    if (status == SGX_SUCCESS)
        return retval;
    else
        return TSS2_TCTI_RC_GENERAL_FAILURE;
}
/**
 * This is the function that is hooked into the standard TSS2_TCTI_CONTEXT
 * setLocality function pointer. This function will be invoked when:
 * - TSS2_TCTI_CONTEXT object is initialized for the SGX TCTI
 * - the tss2_tcti_set_locality (ctx ...) is invoked with said context
 * It's also possible to invoke this function directly but that should be
 * very rare.
 * This function returns:
 * - TSS2_TCTI_RC_BAD_REFERENCE when the context is NULL
 * - TSS2_TCTI_RC_BAD_SEQUENCE  when the state machine is not in the
 *   READY_TO_TRANSMIT state
 * - TSS2_TCTI_RC_GENERAL_FAILURE when an SGX error occurs
 */
static TSS2_RC
tss2_tcti_sgx_set_locality (TSS2_TCTI_CONTEXT *tcti_context,
                            uint8_t            locality)
{
    sgx_status_t status;
    TSS2_RC retval;

    if (tcti_context == NULL)
        return TSS2_TCTI_RC_BAD_REFERENCE;
    if (TSS2_TCTI_SGX_STATE (tcti_context) != READY_TO_TRANSMIT)
        return TSS2_TCTI_RC_BAD_SEQUENCE;

    status = tss2_tcti_sgx_set_locality_ocall (&retval,
                                               TSS2_TCTI_SGX_ID (tcti_context),
                                               locality);

    if (status == SGX_SUCCESS)
        return retval;
    else
        return TSS2_TCTI_RC_GENERAL_FAILURE;
}
/**
 * This is the initialization function for the SGX TCTI. It inplements a
 * protocol similar to the TSS SAPI that enables the user to obtain the
 * size of the context required by the TCTI using the following algorithm:
 *   if   the 'tcti_context' parameter is NULL and the 'size' parameter is
 *        not
 *   then the 'size' parameter will be set to the required memory size fot
 *        the context object.
 *
 * When called with a non NULL 'tcti_context' this function sets up all of the
 * necessary function pointers for the generic TCTI structure, calls the
 * initialization ocall to get an ID from the resource manager / tpm2
 * access broker outside the enclave and then sets the state to
 * READY_TO_TRANSMIT.
 * This function returns:
 * - TSS2_TCTI_RC_BAD_VALUE: when the 'tcti_context' parameter is NULL or
 *   when the 'size' parameter is NULL.
 * - TSS2_TCTI_RC_GENERAL_FAILURE: when the init ocall fails for any reason
 * - TSS2_RC_SUCCESS: when initialization completes successfully or when
 *   the caller passes in a null context object and a non null size. In this
 *   case the required size for the context object is returned through the
 *   'size' parameter.
 */
TSS2_RC
tss2_tcti_sgx_init (TSS2_TCTI_CONTEXT *tcti_context,
                    size_t            *size)
{
    sgx_status_t status;

    if (tcti_context == NULL && size == NULL)
        return TSS2_TCTI_RC_BAD_VALUE;
    if (tcti_context == NULL && size != NULL) {
        *size = sizeof (TSS2_TCTI_CONTEXT_SGX);
        return TSS2_RC_SUCCESS;
    }
    TSS2_TCTI_MAGIC (tcti_context) = TSS2_TCTI_SGX_MAGIC;
    TSS2_TCTI_VERSION (tcti_context) = 1;
    TSS2_TCTI_TRANSMIT (tcti_context) = tss2_tcti_sgx_transmit;
    TSS2_TCTI_RECEIVE (tcti_context) = tss2_tcti_sgx_receive;
    TSS2_TCTI_FINALIZE (tcti_context) = tss2_tcti_sgx_finalize;
    TSS2_TCTI_CANCEL (tcti_context) = tss2_tcti_sgx_cancel;
    TSS2_TCTI_GET_POLL_HANDLES (tcti_context) = tss2_tcti_sgx_get_poll_handles;
    TSS2_TCTI_SET_LOCALITY (tcti_context) = tss2_tcti_sgx_set_locality;

    status = tss2_tcti_sgx_init_ocall (&TSS2_TCTI_SGX_ID (tcti_context));
    if (status != SGX_SUCCESS)
        return TSS2_TCTI_RC_GENERAL_FAILURE;
    /**
     * Only set state to READY_TO_TRANSMIT after ocall to initialize
     * connection completes successfully
     */
    TSS2_TCTI_SGX_STATE (tcti_context) = READY_TO_TRANSMIT;
    return TSS2_RC_SUCCESS;
}
