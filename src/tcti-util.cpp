/*
 * Copyright 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <tss2/tss2_tcti.h>
#include <tss2/tss2_tcti_mssim.h>

#include "tcti-util.h"

TSS2_TCTI_CONTEXT*
mssim_tcti_init (void *user_data)
{
    const char* conf_str = (const char*)user_data;
    TSS2_TCTI_CONTEXT* ctx = NULL;
    TSS2_RC rc = TSS2_RC_SUCCESS;
    size_t size = 0;

    rc = Tss2_Tcti_Mssim_Init (NULL, &size, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        printf ("%s: first call to Tss2_Tcti_Tabrmd_Init failed with RC 0x%"
                PRIx32 "\n", __func__, rc);
        return NULL;
    }
    ctx = (TSS2_TCTI_CONTEXT*)calloc (1, size);
    if (ctx == NULL) {
        printf ("%s: Failed to allocate context object: %s", __func__, strerror (errno));
        return NULL;
    }
    rc = Tss2_Tcti_Mssim_Init (ctx, &size, conf_str);
    if (rc != TSS2_RC_SUCCESS) {
        printf ("%s: second call to Tss2_Tcti_Tabrmd_Init failed with RC 0x%"
                PRIx32 "\n", __func__, rc);
        free (ctx);
        return NULL;
    }
    return ctx;
}
