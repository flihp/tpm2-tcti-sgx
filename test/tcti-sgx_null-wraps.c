/*
 * Copyright 2016 - 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
/* These are all mock implementations of the ocalls made by the
 * tss2-tcti-sgx library. Mocks of these are necessary for all cmocka tests
 * that instantiate the SGX TCTI. Your cmocka tests may want to implement
 * specific behavior for one of these and in that case you'll have to
 * implement the behavior elsewhere.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <sgx_error.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tss2_tpm2_types.h>

#include "util.h"

/*
 * This is a mock function for the initialization OCall.
 * Instead of actually calling out to the external RM/TAB
 * initialization function we pop a value off of the cmocka
 * stack for the ID (retval) and the sgx return status.
 */
sgx_status_t
__wrap_tcti_sgx_init_ocall (uint64_t *retval)
{
    *retval = (TSS2_RC)mock ();
    return (sgx_status_t)mock ();
}

sgx_status_t
__wrap_tcti_sgx_transmit_ocall (TSS2_RC *retval,
                                uint64_t id,
                                size_t size,
                                const uint8_t* command)
{
    UNUSED (id);
    UNUSED (size);
    UNUSED (command);

    *retval = (TSS2_RC)mock ();
    return (sgx_status_t)mock ();
}

sgx_status_t
__wrap_tcti_sgx_receive_ocall (TSS2_RC *retval,
                               uint64_t id,
                               size_t size,
                               uint8_t *response,
                               uint32_t timeout)
{
    UNUSED (id);
    UNUSED (size);
    UNUSED (response);
    UNUSED (timeout);

    *retval = (TSS2_RC)mock ();
    return (sgx_status_t)mock ();
}

void
__wrap_tcti_sgx_finalize_ocall (uint64_t *retval)
{ UNUSED (retval); }

sgx_status_t
__wrap_tcti_sgx_cancel_ocall (TSS2_RC *retval,
                              uint64_t id)
{
    UNUSED (id);

    *retval = (TSS2_RC)mock ();
    return (sgx_status_t)mock ();
}

sgx_status_t
__wrap_tcti_sgx_set_locality_ocall (TSS2_RC *retval,
                                    uint64_t id,
                                    uint8_t locality)
{
    UNUSED (id);
    UNUSED (locality);

    *retval = (TSS2_RC)mock ();
    return (sgx_status_t)mock ();
}
