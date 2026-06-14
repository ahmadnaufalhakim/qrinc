#include <assert.h>
#include <stdio.h>

#include "qr_gf.h"

#define TEST(name) printf("RUNNING: %s\n", name)

void test_qr_gf_exp() {
    assert(qr_gf_exp(0) == 1);
    assert(qr_gf_exp(1) == 2);
    assert(qr_gf_exp(2) == 4);
    assert(qr_gf_exp(8) == 29);
    assert(qr_gf_exp(254) == 142);
}

void test_qr_gf_log() {
    assert(qr_gf_log(1) == 0);
    assert(qr_gf_log(2) == 1);
    assert(qr_gf_log(4) == 2);
    assert(qr_gf_log(29) == 8);
    assert(qr_gf_log(142) == 254);
}

void test_qr_gf_exp_log_inverse() {
    for (int i = 1; i < 256; i++) {
        assert(qr_gf_exp(qr_gf_log((uint8_t)i)) == (uint8_t)i);
    }
}

void test_qr_gf_add() {
    assert(qr_gf_add(0, 0) == 0);
    assert(qr_gf_add(1, 1) == 0);
    assert(qr_gf_add(5, 3) == (5 ^ 3));
    assert(qr_gf_add(0xAA, 0x55) == 0xFF);
    assert(qr_gf_add(0xFF, 0xFF) == 0);
}

void test_qr_gf_mul_zero() {
    assert(qr_gf_mul(0, 0) == 0);
    assert(qr_gf_mul(0, 123) == 0);
    assert(qr_gf_mul(123, 0) == 0);
}

void test_qr_gf_mul_identity() {
    for (int i = 1; i < 256; i++) {
        assert(qr_gf_mul(1, (uint8_t)i) == (uint8_t)i);
        assert(qr_gf_mul((uint8_t)i, 1) == (uint8_t)i);
    }
}

void test_qr_gf_mul_known_values() {
    /*
     * Since:
     *   2 = α^1
     *   4 = α^2
     * Then:
     *   2 * 4 = α^(1+2) = α^3 = 8
     */
    assert(qr_gf_mul(2, 4) == 8);

    /*
     * 29 = α^8
     * 2  = α^1
     * α^(8+1) = α^9 = 58
     */
    assert(qr_gf_mul(29, 2) == 58);

    /*
     * 142 = α^254
     * 2   = α^1
     * α^(254+1) = α^0 = 1
     */
    assert(qr_gf_mul(142, 2) == 1);
}

void test_qr_gf_mul_commutative() {
    uint8_t values[] = {
        1, 2, 3, 5, 17, 29, 87, 142, 255};

    size_t n = sizeof(values) / sizeof(values[0]);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            assert(
                qr_gf_mul(values[i], values[j]) ==
                qr_gf_mul(values[j], values[i]));
        }
    }
}

void test_gf() {
    TEST("GF exp");
    test_qr_gf_exp();

    TEST("GF log");
    test_qr_gf_log();

    TEST("GF exp-log inverse identity");
    test_qr_gf_exp_log_inverse();

    TEST("GF add");
    test_qr_gf_add();

    TEST("GF multiply by zero");
    test_qr_gf_mul_zero();

    TEST("GF multiply identity");
    test_qr_gf_mul_identity();

    TEST("GF multiply known values");
    test_qr_gf_mul_known_values();

    TEST("GF multiply commutative");
    test_qr_gf_mul_commutative();

    printf("ALL GF TESTS PASSED\n");
}
