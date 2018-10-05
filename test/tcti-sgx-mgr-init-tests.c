/*
 * Copyright 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int
__real_open (const char *pathname,
             int flags);
int
__wrap_open (const char *pathname,
             int flags)
{
     if (strcmp ("/dev/urandom", pathname) == 0) {
         printf ("%s: mock open\n", __func__);
         errno = mock_type (int);
         return mock_type (int);
     } else {
         printf ("%s: real open\n", __func__);
         return __real_open (pathname, flags);
     }
}

#define TEST_FD 0xa3c18ae2
ssize_t
__real_read (int fd,
             void *buf,
             size_t count);
ssize_t
__wrap_read (int fd,
             void *buf,
             size_t count)
{
    if (fd == TEST_FD) {
        tcti_sgx_session_t *session = (tcti_sgx_session_t*)buf;
        printf ("%s: mock read\n", __func__);
        errno = mock_type (int);
        session->id = mock_type (uint64_t);
        return mock_type (ssize_t);
    } else {
        printf ("%s: real read\n", __func__);
        return __real_read (fd, buf, count);
    }
}

TSS2_TCTI_CONTEXT*
callback (void *user_data)
{
    return NULL;
}

static int
tcti_sgx_mgr_init_setup (void **state)
{
    will_return (__wrap_calloc, passthrough);
    *state = tcti_sgx_mgr_init (callback, NULL);
    return 0;
}

#define TEST_CTX (TSS2_TCTI_CONTEXT*)0x666
TSS2_TCTI_CONTEXT*
callback_ctx (void *user_data)
{
    return TEST_CTX;
}

#define TEST_CTX (TSS2_TCTI_CONTEXT*)0x666
static int
tcti_sgx_mgr_init_setup_ctx (void **state)
{
    will_return (__wrap_calloc, passthrough);
    *state = tcti_sgx_mgr_init (callback_ctx, NULL);
    return 0;
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

static void
tcti_sgx_mgr_init_twice (void **state)
{
    tcti_sgx_mgr_t *mgr = tcti_sgx_mgr_init (callback, NULL);
    assert_null (mgr);
}

static void
tcti_sgx_mgr_init_ocall_no_init (void **state)
{
    uint64_t id = tcti_sgx_init_ocall ();
    assert_int_equal (id, 0);
}

static void
tcti_sgx_mgr_init_ocall_open_fail (void **state)
{
    will_return (__wrap_open, EACCES);
    will_return (__wrap_open, -1);
    uint64_t id = tcti_sgx_init_ocall ();
    assert_int_equal (id, 0);
}

static void
tcti_sgx_mgr_init_ocall_calloc_fail (void **state)
{
     will_return (__wrap_open, 0);
     will_return (__wrap_open, TEST_FD);
     will_return (__wrap_calloc, null);
     uint64_t id = tcti_sgx_init_ocall ();
     assert_int_equal (id, 0);
}

static void
tcti_sgx_mgr_init_ocall_read_fail (void **state)
{
    will_return (__wrap_open, 0);
    will_return (__wrap_open, TEST_FD);
    will_return (__wrap_calloc, passthrough);
    will_return (__wrap_read, EACCES);
    will_return (__wrap_read, 0);
    will_return (__wrap_read, -1);
    uint64_t id = tcti_sgx_init_ocall ();
    assert_int_equal (id, 0);
}

#define TEST_ID  0xe981fd3173ea6a9c
static void
tcti_sgx_mgr_init_ocall_cb_fail (void **state)
{
    will_return (__wrap_open, 0);
    will_return (__wrap_open, TEST_FD);
    will_return (__wrap_calloc, passthrough);
    will_return (__wrap_read, 0);
    will_return (__wrap_read, TEST_ID);
    will_return (__wrap_read, sizeof (uint64_t));
    uint64_t id = tcti_sgx_init_ocall ();
    assert_int_equal (id, 0);
}

gboolean
__wrap_g_hash_table_insert (GHashTable *hash_table,
                            gpointer key,
                            gpointer value)
{
    printf ("%s:\n", __func__);
    return mock_type (gboolean);
}

static void
tcti_sgx_mgr_init_ocall_insert_fail (void **state)
{
    will_return (__wrap_open, 0);
    will_return (__wrap_open, TEST_FD);
    will_return (__wrap_calloc, passthrough);
    will_return (__wrap_read, 0);
    will_return (__wrap_read, TEST_ID);
    will_return (__wrap_read, sizeof (uint64_t));
    will_return (__wrap_g_hash_table_insert, FALSE);
    uint64_t id = tcti_sgx_init_ocall ();
    assert_int_equal (id, 0);
}

static void
tcti_sgx_mgr_init_ocall_success (void **state)
{
    will_return (__wrap_open, 0);
    will_return (__wrap_open, TEST_FD);
    will_return (__wrap_calloc, passthrough);
    will_return (__wrap_read, 0);
    will_return (__wrap_read, TEST_ID);
    will_return (__wrap_read, sizeof (uint64_t));
    will_return (__wrap_g_hash_table_insert, TRUE);
    uint64_t id = tcti_sgx_init_ocall ();
    assert_int_equal (id, TEST_ID);
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
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_init_twice,
                                         tcti_sgx_mgr_init_setup,
                                         tcti_sgx_mgr_init_teardown),
        cmocka_unit_test (tcti_sgx_mgr_init_ocall_no_init),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_init_ocall_open_fail,
                                         tcti_sgx_mgr_init_setup,
                                         tcti_sgx_mgr_init_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_init_ocall_calloc_fail,
                                         tcti_sgx_mgr_init_setup,
                                         tcti_sgx_mgr_init_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_init_ocall_read_fail,
                                         tcti_sgx_mgr_init_setup,
                                         tcti_sgx_mgr_init_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_init_ocall_cb_fail,
                                         tcti_sgx_mgr_init_setup,
                                         tcti_sgx_mgr_init_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_init_ocall_insert_fail,
                                         tcti_sgx_mgr_init_setup_ctx,
                                         tcti_sgx_mgr_init_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_init_ocall_success,
                                         tcti_sgx_mgr_init_setup_ctx,
                                         tcti_sgx_mgr_init_teardown),
    };

    return cmocka_run_group_tests (tests, NULL, NULL);
}
