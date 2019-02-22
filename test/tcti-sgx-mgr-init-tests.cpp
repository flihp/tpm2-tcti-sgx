/*
 * Copyright 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>

extern "C" {
#include <cmocka.h>
}

#include "tcti-sgx-mgr_priv.h"
#include "tcti-sgx-mgr.h"

enum calloc_flag {
    null,
    passthrough
};

extern "C" {
void*
__real_calloc (size_t nmemb,
               size_t size);
void*
__wrap_calloc (size_t nmemb,
               size_t size);
void
__real_free (void* mem);
void
__wrap_free (void* mem);
int
__real_open (const char *pathname,
             int flags);
int
__wrap_open (const char *pathname,
             int flags);
ssize_t
__real_read (int fd,
             void *buf,
             size_t count);
ssize_t
__wrap_read (int fd,
             void *buf,
             size_t count);
}

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
        assert_true (false);
        return NULL;
    }
}

#define TEST_CTX (TSS2_TCTI_CONTEXT*)0x666
void
__wrap_free (void* mem) {
    if (mem == TEST_CTX) {
        printf ("%s: mem %p\n", __func__, mem);
        return;
    } else {
        __real_free (mem);
    }
}

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

#define TEST_FD (int)0xa3c18ae2
ssize_t
__wrap_read (int fd,
             void *buf,
             size_t count)
{
    uint64_t id;

    if (fd == TEST_FD) {
        printf ("%s: mock read\n", __func__);
        errno = mock_type (int);
        id = mock_type (uint64_t);
        memcpy (buf, &id, sizeof (id));
        return mock_type (ssize_t);
    } else {
        printf ("%s: real read\n", __func__);
        return __real_read (fd, buf, count);
    }
}

TSS2_TCTI_CONTEXT*
callback_ctx (void *user_data)
{
    printf ("%s: floop\n", __func__);
    return mock_type (TSS2_TCTI_CONTEXT*);
}

static int
tcti_sgx_mgr_init_setup(void **state)
{
    return tcti_sgx_mgr_init (callback_ctx, NULL);
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
tcti_sgx_mgr_init_ocall_read_fail (void **state)
{
    will_return (__wrap_open, 0);
    will_return (__wrap_open, TEST_FD);
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
    will_return (__wrap_read, 0);
    will_return (__wrap_read, TEST_ID);
    will_return (__wrap_read, sizeof (uint64_t));
    will_return (callback_ctx, NULL);
    uint64_t id = tcti_sgx_init_ocall ();
    assert_int_equal (id, 0);
}

static void
tcti_sgx_mgr_init_ocall_success (void **state)
{
    will_return (__wrap_open, 0);
    will_return (__wrap_open, TEST_FD);
    will_return (__wrap_read, 0);
    will_return (__wrap_read, TEST_ID);
    will_return (__wrap_read, sizeof (uint64_t));
    will_return (callback_ctx, TEST_CTX);
    uint64_t id = tcti_sgx_init_ocall ();
    assert_int_equal (id, TEST_ID);
}

int
main (void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup (tcti_sgx_mgr_init_ocall_open_fail,
                                tcti_sgx_mgr_init_setup),
        cmocka_unit_test_setup (tcti_sgx_mgr_init_ocall_read_fail,
                                tcti_sgx_mgr_init_setup),
        cmocka_unit_test_setup (tcti_sgx_mgr_init_ocall_cb_fail,
                                tcti_sgx_mgr_init_setup),
        cmocka_unit_test_setup (tcti_sgx_mgr_init_ocall_success,
                                tcti_sgx_mgr_init_setup),
    };

    return cmocka_run_group_tests (tests, NULL, NULL);
}
