#include <inttypes.h>
#include <sgx_error.h>
#include <sgx_urts.h>
#include <stdio.h>

#include <tss2/tss2-tcti-tabrmd.h>

#include "tcti-sgx-mgr.h"
#include "enclave_u.h"

TSS2_TCTI_CONTEXT*
tcti_init_cb (void *user_data)
{
    const char* conf_str = (const char*)user_data;
    TSS2_TCTI_CONTEXT* ctx = NULL;
    TSS2_RC rc = TSS2_RC_SUCCESS;
    size_t size = 0;

    rc = Tss2_Tcti_Tabrmd_Init (NULL, &size, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        printf ("%s: first call to Tss2_Tcti_Tabrmd_Init failed with RC 0x%"
                PRIx32 "\n", __func__, rc);
        return NULL;
    }
    ctx = calloc (1, size);
    rc = Tss2_Tcti_Tabrmd_Init (ctx, &size, conf_str);
    if (rc != TSS2_RC_SUCCESS) {
        printf ("%s: second call to Tss2_Tcti_Tabrmd_Init failed with RC 0x%"
                PRIx32 "\n", __func__, rc);
        free (ctx);
        return NULL;
    }
    return ctx;
}

int
main (int argc,
      char *argv[])
{
    sgx_launch_token_t token = { 0 };
    sgx_enclave_id_t enclave_id = 0;
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    uint32_t manufact_id = 0;

    if (argc != 2) {
        printf ("Usage: %s /path/to/signed/enclave.so\n", argv[0]);
        return 1;
    }

    if (tcti_sgx_mgr_init (tcti_init_cb, "bus_type=session") == 1) {
        printf ("%s: failed to initialize SGX TCTI Mgr\n", __func__);
        return 1;
    }
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
