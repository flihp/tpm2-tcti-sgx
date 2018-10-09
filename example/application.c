#include <sgx_error.h>
#include <sgx_urts.h>
#include <stdio.h>

#include "enclave_u.h"

int
main (int argc,
      char *argv[])
{
    sgx_launch_token_t token = { 0 };
    sgx_enclave_id_t enclave_id = 0;
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    uint32_t manufact_id = 0;

    printf ("%s: loading enclave %s with debug %u\n",
            __func__, argv[1], SGX_DEBUG_FLAG);
    ret = sgx_create_enclave (argv [1],
                              SGX_DEBUG_FLAG,
                              &token,
                              &updated,
                              &enclave_id,
                              NULL);
    if (ret != SGX_SUCCESS) {
        printf ("%s: sgx_create_enclave failed with sgx_status_t 0x%x\n",
                __func__, ret);
        return 1;
    }

    ret = getcap_manufacturer (enclave_id, &manufact_id);
    if (ret != SGX_SUCCESS) {
        printf ("%s: getcap_manufacturer for eid 0x%x failed with sgx_status_t "
                "0x%x\n", __func__, enclave_id, ret);
        return 1;
    }

    printf ("EnclaveID 0x%x TPM2 ManufacturerID 0x%x\n",
            enclave_id, manufact_id);
    return 0;
}
