/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TSS2_TCTI_SGX_PRIV_H
#define TSS2_TCTI_SGX_PRIV_H

/*
 * generate your own:
 * cat /dev/random | tr -dc 'a-f0-9' | fold -w 16 | head -n 1
 */
#define TCTI_SGX_MAGIC 0x4e50bc1dcdb7623c
#define TCTI_SGX_ID(context) ((TCTI_CONTEXT_SGX*)context)->id
#define TCTI_SGX_STATE(context) ((TCTI_CONTEXT_SGX*)context)->state

/*
 * There is a small state machine maintained by this TCTI. It is used to
 * prevent callers from making nonsensical function calls like trying to
 * call the receive function before calling transmit.
 *
 * After a successful call to the initialization function the context will
 * be in the 'READY_TO_TRANSMIT' state. If the call to initialize fails for any
 * reason, the state will be indeterminate.
 * A successful call to Tss2_Tcti_Transmit will set the state to
 * READY_TO_RECEIVE. If the context is not in the READY_TO_TRANSMIT state
 * when transmit is called then BAD_SEQUENCE will be returned.
 * A successful call to Tss2_Tcti_Receive will set the state to
 * READY_TO_TRANSMIT. If the context is not in the READY_TO_RECEIVE state
 * when receive is called then BAD_SEQUENCE will be returned.
 */
typedef enum {
    READY_TO_RECEIVE,
    READY_TO_TRANSMIT,
} tcti_sgx_state_t;

/*
 * This is our private TCTI structure. We're required by the spec to have
 * the same structure as the non-opaque area defined by the
 * TSS2_TCTI_CONTEXT_COMMON_V1 structure. Anything after this data is opaque
 * and private to our implementation. See section 7.3 of the SAPI / TCTI spec
 * for the details.
 */
typedef struct {
    TSS2_TCTI_CONTEXT_COMMON_V1 common;
    uint64_t                    id;
    tcti_sgx_state_t state;
} TCTI_CONTEXT_SGX;

TSS2_RC tcti_sgx_transmit (TSS2_TCTI_CONTEXT *tcti_context,
                           size_t size,
                           uint8_t const *command);
TSS2_RC tcti_sgx_receive (TSS2_TCTI_CONTEXT *tcti_context,
                          size_t *size,
                          uint8_t *response,
                          int32_t timeout);
void tcti_sgx_finalize (TSS2_TCTI_CONTEXT *tcti_context);
TSS2_RC tcti_sgx_cancel (TSS2_TCTI_CONTEXT *tcti_context);
TSS2_RC tcti_sgx_get_poll_handles (TSS2_TCTI_CONTEXT *tcti_context,
                                   TSS2_TCTI_POLL_HANDLE *handles,
                                   size_t *num_handles);
TSS2_RC tcti_sgx_set_locality (TSS2_TCTI_CONTEXT *tcti_context,
                               uint8_t locality);

#endif /* TSS2_TCTI_SGX_PRIV_H */
