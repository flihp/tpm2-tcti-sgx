#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <tss2/tss2_tpm2_types.h>
#include <tss2/tss2_tcti.h>

#include "tss2-tcti-sgx.h"

#include "enclave_t.h"

/*
 * printf equivalent for use in the enclave
 */
void enc_printf (const char *fmt, ...)
{
    char buf [BUFSIZ] = { '\0' };
    va_list ap;
    va_start (ap, fmt);
    vsnprintf (buf, BUFSIZ, fmt, ap);
    va_end (ap);
    print_string (buf);
}

uint32_t endian_conv_32(uint32_t value)
{
    return ((value & (0xff))       << 24) | \
           ((value & (0xff << 8))  << 8)  | \
           ((value & (0xff << 16)) >> 8)  | \
           ((value & (0xff << 24)) >> 24);
}

TSS2_RC tcti_init (TSS2_TCTI_CONTEXT **tcti_ctx)
{
    TSS2_RC rc = TSS2_RC_SUCCESS;
    size_t size = 0;

    rc = Tss2_Tcti_Sgx_Init (NULL, &size);
    if (rc != TSS2_RC_SUCCESS) {
        enc_printf ("%s: first Tss2_Tcti_Sgx_Init failed with RC: 0x%" PRIx32
                    "\n", __func__, rc);
        return rc;
    }

    *tcti_ctx = calloc (1, size);
    if (*tcti_ctx == NULL) {
        enc_printf ("%s: calloc failed with errno %d\n", __func__, errno);
        return rc;
    }

    rc = Tss2_Tcti_Sgx_Init (*tcti_ctx, &size);
    if (rc != TSS2_RC_SUCCESS) {
        enc_printf ("%s: second Tss2_Tcti_Sgx_Init failed with RC: 0x%"
                    PRIx32 "\n", __func__, rc);
        free (*tcti_ctx);
    }

    return rc;
}

TSS2_RC
execute_cmd_buf (TSS2_TCTI_CONTEXT *tcti_ctx,
                 const uint8_t *cmd_buf,
                 size_t cmd_size,
                 uint8_t *rsp_buf,
                 size_t *rsp_size)
{
    TSS2_RC rc;

    rc = Tss2_Tcti_Transmit (tcti_ctx, cmd_size, cmd_buf);
    if (rc != TSS2_RC_SUCCESS) {
        enc_printf ("%s: Tss2_Tcti_Transmit failed with RC: 0x%" PRIx32 "\n",
                    __func__, rc);
        return rc;
    }

    rc = Tss2_Tcti_Receive (tcti_ctx,
                            rsp_size,
                            rsp_buf,
                            TSS2_TCTI_TIMEOUT_BLOCK);
    if (rc != TSS2_RC_SUCCESS) {
        enc_printf ("%s: Tss2_Tcti_Receive failed with RC: 0x%" PRIx32 "\n",
                    __func__, rc);
    }

    return rc;
}

/*
 * the command buffer to invoke the GetCapability command to query for the
 * TPM_PT_MANUFACTURER property.
 */
static uint8_t cmd_buf [] = {
    0x80, 0x01, /* tag */
    0x00, 0x00, 0x00, 0x16, /* size: 22 */
    0x00, 0x00, 0x01, 0x7a, /* commandCode: TPM2_GetCapability */
    0x00, 0x00, 0x00, 0x06, /* capability: TPM_CAP_TPM_PROPERTIES */
    0x00, 0x00, 0x01, 0x05, /* property: TPM_PT_MANUFACTURER */
    0x00, 0x00, 0x00, 0x01  /* propertyCount: 1 */
};

/*
 * get the manufacturer ID from the TPM using the GetCapability command
 * NOTE:
 * We need to extract the value for the TPM_PT_MANUFACTURER property from
 * the response buffer. By manually unpacking the TPMS_CAPABILITY_DATA from
 * the response buffer we know it's a uint32_t @ offset 23. The expected
 * response buffer is:
 *     0x80, 0x01,             0:1   tag
 *     0x00, 0x00, 0x00, 0x1b, 2:5   size                 27
 *     0x00, 0x00, 0x00, 0x00, 6:9   responseCode         success
 *     0x01,                   10:   more data            yes
 *                             11:   TPMS_CAPABILITY_DATA
 *     0x00, 0x00, 0x00, 0x06, 11:14 capability           TPM_CAP_TPM_PROPERTIES
 *                             15:   TPMU_CAPABILITIES    TPML_TAGGED_TPM_PROPERTY
 *     0x00, 0x00, 0x00, 0x01, 15:18 count                1
 *                             19:   TPMS_TAGGED_PROPERTY
 *     0x00, 0x00, 0x01, 0x05, 19:22 property             TPM_PT_MANUFACTURER
 *     0x49, 0x42, 0x4d, 0x20, 23:27 value(UINT32)        0x49424d20
 */
uint32_t
getcap_manufacturer(void)
{
    TSS2_TCTI_CONTEXT *tcti_ctx;
    TSS2_RC rc;
    uint8_t rsp_buf [27] = { 0 };
    size_t size = sizeof (rsp_buf);
    uint32_t val = 0;

    rc = tcti_init (&tcti_ctx);
    if (rc != TSS2_RC_SUCCESS) {
        return 0;
    }

    rc = execute_cmd_buf (tcti_ctx, cmd_buf, sizeof (cmd_buf), rsp_buf, &size);
    if (rc != TSS2_RC_SUCCESS) {
        goto out;
    }

    memcpy (&val, &rsp_buf [23], sizeof (val));
    val = endian_conv_32 (val);

out:
    Tss2_Tcti_Finalize (tcti_ctx);
    if (tcti_ctx) {
       free (tcti_ctx);
    }

    return val;
}
