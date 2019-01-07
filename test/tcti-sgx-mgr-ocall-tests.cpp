/*
 * Copyright 2018, Intel Corporation
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
extern "C" {
#include <cmocka.h>
}

#include "tcti-sgx-mgr_priv.h"
#include "tcti-sgx-mgr.h"

extern "C" {
gpointer
__wrap_g_hash_table_lookup (GHashTable *hash_table,
                            gconstpointer key);
}

gpointer
__wrap_g_hash_table_lookup (GHashTable *hash_table,
                            gconstpointer key)
{
    return mock_type (TctiSgxSession*);
}

static TSS2_RC
mock_transmit (TSS2_TCTI_CONTEXT *ctx,
               size_t size,
               uint8_t const *command)
{
    return mock_type (TSS2_RC);
}

static TSS2_RC
mock_receive (TSS2_TCTI_CONTEXT *ctx,
              size_t *size,
              uint8_t *response,
              int32_t timeout)
{
    return mock_type (TSS2_RC);
}

static TSS2_RC
mock_cancel (TSS2_TCTI_CONTEXT *ctx)
{
    return mock_type (TSS2_RC);
}

static TSS2_RC
mock_set_locality (TSS2_TCTI_CONTEXT *ctx,
                   uint8_t locality)
{
    return mock_type (TSS2_RC);
}

TSS2_TCTI_CONTEXT*
test_tcti_cb (void *user_data)
{
    TSS2_TCTI_CONTEXT_COMMON_V2 *ctx;

    ctx = (TSS2_TCTI_CONTEXT_COMMON_V2*)calloc (1, sizeof (TSS2_TCTI_CONTEXT_COMMON_V2));
    ctx->v1.version = 2;
    ctx->v1.transmit = mock_transmit;
    ctx->v1.receive = mock_receive;
    ctx->v1.cancel = mock_cancel;
    ctx->v1.setLocality = mock_set_locality;
    return  (TSS2_TCTI_CONTEXT*)ctx;
}

static int
tcti_transmit_setup( void **state)
{
    return tcti_sgx_mgr_init (test_tcti_cb, NULL);
}

static int
tcti_teardown (void **state)
{
    tcti_sgx_mgr_finalize ();
    return 0;
}

#define TRANSMIT_ID 0xf913038a09efdab5
static void
tcti_sgx_mgr_transmit_ocall_bad_id (void **state)
{
    TSS2_RC rc;

    will_return (__wrap_g_hash_table_lookup, NULL);
    rc = tcti_sgx_transmit_ocall (TRANSMIT_ID, 0, NULL);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_VALUE);
}

static void
tcti_sgx_mgr_transmit_ocall (void **state)
{
    TSS2_RC rc;
    TctiSgxSession session (TRANSMIT_ID, test_tcti_cb (NULL));
    uint8_t buf [12] = { 0 };

    will_return (__wrap_g_hash_table_lookup, &session);
    will_return (mock_transmit, TSS2_RC_SUCCESS);
    rc = tcti_sgx_transmit_ocall (TRANSMIT_ID, sizeof (buf), buf);
    assert_int_equal (rc, TSS2_RC_SUCCESS);
}

static void
tcti_sgx_mgr_receive_ocall_bad_timeout (void **state)
{
    TSS2_RC rc;

    rc = tcti_sgx_receive_ocall (TRANSMIT_ID, 0, NULL, 1);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_VALUE);
}

static void
tcti_sgx_mgr_receive_ocall_bad_id (void **state)
{
    TSS2_RC rc;

    will_return (__wrap_g_hash_table_lookup, NULL);
    rc = tcti_sgx_receive_ocall (TRANSMIT_ID, 0, NULL, TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_VALUE);
}

static void
tcti_sgx_mgr_receive_ocall (void **state)
{
    TSS2_RC rc;
    TctiSgxSession session (TRANSMIT_ID, test_tcti_cb (NULL));;
    uint8_t buf [10] = { 0 };

    will_return (__wrap_g_hash_table_lookup, &session);
    will_return (mock_receive, TSS2_RC_SUCCESS);
    rc = tcti_sgx_receive_ocall (TRANSMIT_ID, sizeof (buf), buf, TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (rc, TSS2_RC_SUCCESS);
}

static void
tcti_sgx_mgr_finalize_ocall_bad_id (void **state)
{
    will_return (__wrap_g_hash_table_lookup, NULL);
    tcti_sgx_finalize_ocall (TRANSMIT_ID);
}

static void
tcti_sgx_mgr_finalize_ocall (void **state)
{
    TctiSgxSession *session;
    uint8_t buf [10] = { 0 };

    session = new TctiSgxSession (TRANSMIT_ID, test_tcti_cb (NULL));
    will_return (__wrap_g_hash_table_lookup, session);
    tcti_sgx_finalize_ocall (TRANSMIT_ID);
}

static void
tcti_sgx_mgr_cancel_ocall_bad_id (void **state)
{
    TSS2_RC rc;

    will_return (__wrap_g_hash_table_lookup, NULL);
    rc = tcti_sgx_cancel_ocall (TRANSMIT_ID);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_VALUE);
}

static void
tcti_sgx_mgr_cancel_ocall (void **state)
{
    TSS2_RC rc;
    TctiSgxSession session (TRANSMIT_ID, test_tcti_cb (NULL));

    will_return (__wrap_g_hash_table_lookup, &session);
    will_return (mock_cancel, TSS2_RC_SUCCESS);
    rc = tcti_sgx_cancel_ocall (TRANSMIT_ID);
}

static void
tcti_sgx_mgr_get_poll_handles_ocall (void **state)
{
    TSS2_RC rc;
    TSS2_TCTI_POLL_HANDLE handles [2];
    size_t num_handles = sizeof (handles);

    rc = tcti_sgx_get_poll_handles_ocall (TRANSMIT_ID, handles, &num_handles);
    assert_int_equal (rc, TSS2_TCTI_RC_NOT_IMPLEMENTED);
}

static void
tcti_sgx_mgr_set_locality_ocall_bad_id (void **state)
{
    TSS2_RC rc;

    will_return (__wrap_g_hash_table_lookup, NULL);
    rc = tcti_sgx_set_locality_ocall (TRANSMIT_ID, 2);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_VALUE);
}

static void
tcti_sgx_mgr_set_locality_ocall (void **state)
{
    TSS2_RC rc;
    TctiSgxSession session (TRANSMIT_ID, test_tcti_cb (NULL));

    will_return (__wrap_g_hash_table_lookup, &session);
    will_return (mock_set_locality, TSS2_RC_SUCCESS);
    rc = tcti_sgx_set_locality_ocall (TRANSMIT_ID, 2);
    assert_int_equal (rc, TSS2_RC_SUCCESS);
}

int
main (void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_transmit_ocall_bad_id,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_transmit_ocall,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_receive_ocall_bad_timeout,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_receive_ocall_bad_id,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_receive_ocall,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_finalize_ocall_bad_id,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_finalize_ocall,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_cancel_ocall_bad_id,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_cancel_ocall,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_get_poll_handles_ocall,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_set_locality_ocall_bad_id,
                                         tcti_transmit_setup,
                                         tcti_teardown),
        cmocka_unit_test_setup_teardown (tcti_sgx_mgr_set_locality_ocall,
                                         tcti_transmit_setup,
                                         tcti_teardown),
    };

    return cmocka_run_group_tests (tests, NULL, NULL);
}
