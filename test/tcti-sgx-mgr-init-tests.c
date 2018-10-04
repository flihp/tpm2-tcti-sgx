/*
 * Copyright 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>
#include <cmocka.h>

#include "tcti-sgx-mgr.h"

enum calloc_flag {
    null,
    passthrough
};

void*
__real_calloc (size_t nmemb,
               size_t size);
void*
__wrap_calloc (size_t nmemb,
               size_t size)
{
    enum calloc_flag flag = (enum calloc_flag)mock ();
    switch (flag) {
    case null:
        printf ("%s: returning NULL\n", __func__);
        return NULL;
    case passthrough:
        printf ("%s: invoking 'real' calloc\n", __func__);
        return __real_calloc (nmemb, size);
    default:
        assert_true (FALSE);
    }
}

static int
tcti_sgx_mgr_init_teardown (void **state)
{
    tcti_sgx_mgr_finalize ();
    return 0;
}

static void
tcti_sgx_mgr_init_null_callback (void **state)
{
    tcti_sgx_mgr_t *mgr = tcti_sgx_mgr_init (NULL, NULL);
    assert_null (mgr);
}

TSS2_TCTI_CONTEXT*
callback (void *user_data)
{
    return NULL;
}

static void
tcti_sgx_mgr_init_calloc_fail (void **state)
{
    will_return (__wrap_calloc, null);
    tcti_sgx_mgr_t *mgr = tcti_sgx_mgr_init (callback, NULL);
    assert_null (mgr);
}

static void
tcti_sgx_mgr_init_null_data (void **state)
{
    will_return (__wrap_calloc, passthrough);

    tcti_sgx_mgr_t *mgr = tcti_sgx_mgr_init (callback, NULL);
    assert_non_null (mgr);
    assert_int_equal (mgr->init_cb, callback);
    assert_null (mgr->user_data);
}
int
main (void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown (tcti_sgx_mgr_init_null_callback,
                                   tcti_sgx_mgr_init_teardown),
        cmocka_unit_test_teardown (tcti_sgx_mgr_init_calloc_fail,
                                   tcti_sgx_mgr_init_teardown),
        cmocka_unit_test_teardown (tcti_sgx_mgr_init_null_data,
                                   tcti_sgx_mgr_init_teardown),
    };

    return cmocka_run_group_tests (tests, NULL, NULL);
}
