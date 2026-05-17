#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qr_encoding.h"

#define TEST(name) printf("RUNNING: %s\n", name)

void test_detect_numeric_mode() {
    const char* s = "0123456789";
    assert(qr_detect_mode((const uint8_t*)s, strlen(s)) == QR_MODE_NUMERIC);
}

void test_detect_alphanumeric_mode() {
    const char* s = "HELLO WORLD +0123456789";
    assert(qr_detect_mode((const uint8_t*)s, strlen(s)) == QR_MODE_ALPHANUMERIC);
}

void test_detect_byte_mode() {
    const char* s = "Hello, World! I hope you're having a great day🥰";
    assert(qr_detect_mode((const uint8_t*)s, strlen(s)) == QR_MODE_BYTE);
}

void test_detect_kanji_mode() {
    const char* s = "学校";
    // TODO: implement kanji encoding mode checking
    assert(qr_detect_mode((const uint8_t*)s, strlen(s)) == QR_MODE_KANJI);
}

void test_qr_char_count_indicator() {
    {
        // Numeric mode, version 1-9 => 10 bits
        char* bits = qr_char_count_indicator(
            QR_MODE_NUMERIC,
            1,
            867);

        assert(bits != NULL);
        assert(strcmp(bits, "1101100011") == 0);

        free(bits);
    }

    {
        // Alphanumeric mode, version 1-9 => 9 bits
        // 11 => 000001011
        char* bits = (char*)qr_char_count_indicator(
            QR_MODE_ALPHANUMERIC,
            5,
            11);

        assert(bits != NULL);
        assert(strcmp(bits, "000001011") == 0);

        free(bits);
    }

    {
        // Byte mode, version 10-26 => 16 bits
        // 13 => 0000000000001101
        char* bits = (char*)qr_char_count_indicator(
            QR_MODE_BYTE,
            10,
            13);

        assert(bits != NULL);
        assert(strcmp(bits, "0000000000001101") == 0);

        free(bits);
    }

    {
        // Kanji mode, version 27-40 => 12 bits
        // 42 => 000000101010
        char* bits = (char*)qr_char_count_indicator(
            QR_MODE_KANJI,
            30,
            42);

        assert(bits != NULL);
        assert(strcmp(bits, "000000101010") == 0);

        free(bits);
    }
}

void test_print_binary() {
    char out[16];

    print_binary(0, 4, out);
    assert(strcmp(out, "0000") == 0);

    print_binary(7, 4, out);
    assert(strcmp(out, "0111") == 0);

    print_binary(69, 7, out);
    assert(strcmp(out, "1000101") == 0);

    print_binary(420, 10, out);
    assert(strcmp(out, "0110100100") == 0);
}

void test_qr_numeric_mode_encode() {
    {
        const char* s = "8675309";
        size_t len = strlen(s);
        size_t count;

        char (*chunks)[11] = qr_numeric_mode_encode(s, len, &count);
        assert(chunks != NULL);
        assert(count == 3);

        /*
         * 867 -> 1101100011 (10 bits)
         * 530 -> 1000010010 (10 bits)
         * 9   -> 1001       (4 bits)
         */

        assert(strcmp(chunks[0], "1101100011") == 0);
        assert(strcmp(chunks[1], "1000010010") == 0);
        assert(strcmp(chunks[2], "1001") == 0);

        free(chunks);
    }

    {
        const char* s = "01234567";
        size_t len = strlen(s);
        size_t count;

        char (*chunks)[11] = qr_numeric_mode_encode(s, len, &count);
        assert(chunks != NULL);
        assert(count == 3);

        /*
         * 012 -> 0000001100 (10 bits)
         * 345 -> 0101011001 (10 bits)
         * 67  -> 1000011    (7 bits)
         */

        assert(strcmp(chunks[0], "0000001100") == 0);
        assert(strcmp(chunks[1], "0101011001") == 0);
        assert(strcmp(chunks[2], "1000011") == 0);

        free(chunks);
    }

    {
        const char* s = "4267";
        size_t len = strlen(s);
        size_t count;

        char (*chunks)[11] = qr_numeric_mode_encode(s, len, &count);
        assert(chunks != NULL);
        assert(count == 2);

        /*
         * 426 -> 0110101010 (10 bits)
         * 7   -> 0111       (4 bits)
         */

        assert(strcmp(chunks[0], "0110101010") == 0);
        assert(strcmp(chunks[1], "0111") == 0);

        free(chunks);
    }
}

void test_qr_alphanumeric_mode_encode() {
    const char* s = "HELLO WORLD";
    size_t len = strlen(s);
    size_t count;

    char (*chunks)[12] = qr_alphanumeric_mode_encode(s, len, &count);
    assert(chunks != NULL);
    assert(count == 6);

    assert(strcmp(chunks[0], "01100001011") == 0);
    assert(strcmp(chunks[1], "01111000110") == 0);
    assert(strcmp(chunks[2], "10001011100") == 0);
    assert(strcmp(chunks[3], "10110111000") == 0);
    assert(strcmp(chunks[4], "10011010100") == 0);
    assert(strcmp(chunks[5], "001101") == 0);

    free(chunks);
}

void test_qr_byte_mode_encode() {
    const char* s = "Hello, world!";
    size_t len = strlen(s);
    size_t count;

    char (*chunks)[9] = qr_byte_mode_encode(s, len, &count);
    assert(chunks != NULL);
    assert(count == 13);

    assert(strcmp(chunks[0], "01001000") == 0);
    assert(strcmp(chunks[1], "01100101") == 0);
    assert(strcmp(chunks[2], "01101100") == 0);
    assert(strcmp(chunks[3], "01101100") == 0);
    assert(strcmp(chunks[4], "01101111") == 0);
    assert(strcmp(chunks[5], "00101100") == 0);
    assert(strcmp(chunks[6], "00100000") == 0);
    assert(strcmp(chunks[7], "01110111") == 0);
    assert(strcmp(chunks[8], "01101111") == 0);
    assert(strcmp(chunks[9], "01110010") == 0);
    assert(strcmp(chunks[10], "01101100") == 0);
    assert(strcmp(chunks[11], "01100100") == 0);
    assert(strcmp(chunks[12], "00100001") == 0);

    free(chunks);
}

void test_qr_kanji_mode_encode() {
    return;
}

void test_encoding() {
    TEST("detect numeric mode");
    test_detect_numeric_mode();

    TEST("detect alphanumeric mode");
    test_detect_alphanumeric_mode();

    TEST("detect byte mode");
    test_detect_byte_mode();

    // TEST("detect kanji mode");
    // test_detect_kanji_mode();

    TEST("print binary");
    test_print_binary();

    TEST("QR char count indicator");
    test_qr_char_count_indicator();

    TEST("QR numeric mode encode");
    test_qr_numeric_mode_encode();

    TEST("QR alphanumeric mode encode");
    test_qr_alphanumeric_mode_encode();

    TEST("QR byte mode encode");
    test_qr_byte_mode_encode();

    // TEST("QR kanji mode encode");
    // test_qr_kanji_mode_encode();

    printf("ALL ENCODING TESTS PASSED\n");
    return;
}
