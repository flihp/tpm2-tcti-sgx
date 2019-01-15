/*
 * Copyright 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TCTI_UTIL_H
#define TCTI_UTIL_H

#include <tss2/tss2_tcti.h>

#if defined (__cplusplus)
extern "C" {
#endif

TSS2_TCTI_CONTEXT* mssim_tcti_init (void *user_data);

#if defined (__cplusplus)
}
#endif
#endif
