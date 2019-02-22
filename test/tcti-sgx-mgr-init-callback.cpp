/*
 * Copyright 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>

extern "C" {
#include <cmocka.h>
}

#include "tcti-sgx-mgr_priv.h"
#include "util.h"

static TSS2_TCTI_CONTEXT*
tcti_init (void *user_data)
{
    UNUSED (user_data);
    return NULL;
}
static void
tcti_sgx_mgr_init_callback (void **state)
{
    UNUSED (state);
    int ret = tcti_sgx_mgr_init (tcti_init, NULL);
    TctiSgxMgr& mgr = TctiSgxMgr::get_instance (NULL, NULL);
    assert_int_equal (ret, 0);
    assert_true (mgr.init_cb == tcti_init);
}

int
main (void)
{
    const struct CMUnitTest tests [] = {
        cmocka_unit_test (tcti_sgx_mgr_init_callback),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
