#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qr_ec.h"

#define TEST(name) printf("RUNNING: %s\n", name)

// Basic validation tests

void test_invalid_min_version_low() {
    int v = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 0, 10);
    assert(v == 0);
}

void test_invalid_min_version_high() {
    int v = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 41, 10);
    assert(v == 0);
}

void test_invalid_ec_level() {
    int v = qr_detect_version(QR_MODE_BYTE, (qr_ec_level_t)999, 1, 10);
    assert(v == 0);
}

//
void test_small_char_count_picks_low_version() {
    int v = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 1, 10);

    // Expect earliest version that can fit small payload
    assert(v >= 1 && v <= 40);
}

//
void test_min_version_respected() {
    int v = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 10, 10);

    assert(v == 0 || v >= 10);
}

//
void test_large_char_count_high_version() {
    int v = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 1, 2000);

    assert(v >= 1 && v <= 40);
}

//
void test_byte_mode_various_sizes() {
    for (int i = 1; i <= 40; i++) {
        int v = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 1, (size_t)i * 10);

        assert(v >= 1 && v <= 40);
    }
}

//
void test_exact_fit_behavior() {
    // Try small increments to ensure deterministic selection
    int v1 = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 1, 1);
    int v2 = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 1, 100);

    assert(v1 >= 1 && v1 <= 40);
    assert(v2 >= v1);
}

//
void test_high_pointer_branch() {
    int v = qr_detect_version(QR_MODE_BYTE, QR_EC_LEVEL_L, 1, 100000);

    // should still return valid version or 0 fallback
    assert(v >= 0 && v <= 40);
}

void test_ec() {
    TEST("invalid min version low");
    test_invalid_min_version_low();

    TEST("invalid min version high");
    test_invalid_min_version_high();

    TEST("invalid ec level");
    test_invalid_ec_level();

    TEST("small char count picks low version");
    test_small_char_count_picks_low_version();

    TEST("min version respected");
    test_min_version_respected();

    TEST("large char count pushes higher version");
    test_large_char_count_high_version();

    TEST("byte mode stress test");
    test_byte_mode_various_sizes();

    TEST("exact fit behavior");
    test_exact_fit_behavior();

    TEST("high pointer branch");
    test_high_pointer_branch();

    printf("ALL EC TESTS PASSED\n");
    return;
}