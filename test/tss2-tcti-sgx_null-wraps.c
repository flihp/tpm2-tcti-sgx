/* These are all mock implementations of the ocalls made by the
 * tss2-tcti-sgx library. Mocks of these are necessary for all cmocka tests
 * that instantiate the SGX TCTI. Your cmocka tests may want to implement
 * specific behavior for one of these and in that case you'll have to
 * implement the behavior elsewhere.
 */
#include <stdint.h>
#include <sgx_error.h>

sgx_status_t
__wrap_tss2_tcti_sgx_init_ocall (uint64_t *retval)
{
    return SGX_SUCCESS;
}

sgx_status_t
__wrap_tss2_tcti_sgx_transmit_ocall (uint64_t *retval)
{
    return SGX_SUCCESS;
}

sgx_status_t
__wrap_tss2_tcti_sgx_receive_ocall (uint64_t *retval)
{
    return SGX_SUCCESS;
}

void
__wrap_tss2_tcti_sgx_finalize_ocall (uint64_t *retval)
{}

sgx_status_t
__wrap_tss2_tcti_sgx_cancel_ocall (uint64_t *retval)
{
    return SGX_SUCCESS;
}

sgx_status_t
__wrap_tss2_tcti_sgx_get_poll_handles_ocall (uint64_t *retval)
{
    return SGX_SUCCESS;
}

sgx_status_t
__wrap_tss2_tcti_sgx_set_locality_ocall (uint64_t *retval)
{
    return SGX_SUCCESS;
}
