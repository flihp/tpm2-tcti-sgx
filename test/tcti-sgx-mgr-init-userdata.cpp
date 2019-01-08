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

static void
tcti_sgx_mgr_init_userdata (void **state)
{
    int tmp = 0;
    int ret = tcti_sgx_mgr_init (NULL, &tmp);
    TctiSgxMgr& mgr = TctiSgxMgr::get_instance (NULL, NULL);
    assert_int_equal (ret, 0);
    assert_true (mgr.user_data == &tmp);
}

int
main (void)
{
    const struct CMUnitTest tests [] = {
        cmocka_unit_test (tcti_sgx_mgr_init_userdata),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
