#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qr_bitstream.h"
#include "qr_encoding.h"

#define TEST(name) printf("RUNNING: %s\n", name)

static void assert_bitstream_eq(
    qr_bitstream_t* bs,
    const char* expected) {
    char* s = qr_bitstream_to_str(bs);

    assert(s != NULL);
    assert(strcmp(s, expected) == 0);

    free(s);
}

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

void test_qr_char_count_indicator_bits() {
    /* Numeric mode */
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 1) == 10);
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 9) == 10);
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 10) == 12);
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 26) == 12);
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 27) == 14);
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 40) == 14);

    /* Alphanumeric mode */
    assert(qr_char_count_indicator_bits(QR_MODE_ALPHANUMERIC, 1) == 9);
    assert(qr_char_count_indicator_bits(QR_MODE_ALPHANUMERIC, 9) == 9);
    assert(qr_char_count_indicator_bits(QR_MODE_ALPHANUMERIC, 10) == 11);
    assert(qr_char_count_indicator_bits(QR_MODE_ALPHANUMERIC, 26) == 11);
    assert(qr_char_count_indicator_bits(QR_MODE_ALPHANUMERIC, 27) == 13);
    assert(qr_char_count_indicator_bits(QR_MODE_ALPHANUMERIC, 40) == 13);

    /* Byte mode */
    assert(qr_char_count_indicator_bits(QR_MODE_BYTE, 1) == 8);
    assert(qr_char_count_indicator_bits(QR_MODE_BYTE, 9) == 8);
    assert(qr_char_count_indicator_bits(QR_MODE_BYTE, 10) == 16);
    assert(qr_char_count_indicator_bits(QR_MODE_BYTE, 26) == 16);
    assert(qr_char_count_indicator_bits(QR_MODE_BYTE, 27) == 16);
    assert(qr_char_count_indicator_bits(QR_MODE_BYTE, 40) == 16);

    /* Kanji mode */
    assert(qr_char_count_indicator_bits(QR_MODE_KANJI, 1) == 8);
    assert(qr_char_count_indicator_bits(QR_MODE_KANJI, 9) == 8);
    assert(qr_char_count_indicator_bits(QR_MODE_KANJI, 10) == 10);
    assert(qr_char_count_indicator_bits(QR_MODE_KANJI, 26) == 10);
    assert(qr_char_count_indicator_bits(QR_MODE_KANJI, 27) == 12);
    assert(qr_char_count_indicator_bits(QR_MODE_KANJI, 40) == 12);

    /* Invalid versions */
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 0) == -1);
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 41) == -1);
    assert(qr_char_count_indicator_bits(QR_MODE_NUMERIC, 999) == -1);
}

void test_qr_numeric_mode_encode() {
    qr_bitstream_t bs;
    qr_bitstream_init(&bs);

    {
        const char* s = "8675309";
        assert(qr_numeric_mode_encode(&bs, s, strlen(s)));

        assert_bitstream_eq(&bs,
                            "1101100011"
                            "1000010010"
                            "1001");
    }

    qr_bitstream_free(&bs);
}

void test_qr_alphanumeric_mode_encode() {
    qr_bitstream_t bs;
    qr_bitstream_init(&bs);

    {
        const char* s = "HELLO WORLD";
        assert(qr_alphanumeric_mode_encode(&bs, s, strlen(s)));

        assert_bitstream_eq(&bs,
                            "01100001011"
                            "01111000110"
                            "10001011100"
                            "10110111000"
                            "10011010100"
                            "001101");
    }

    qr_bitstream_free(&bs);
}

void test_qr_byte_mode_encode() {
    qr_bitstream_t bs;
    qr_bitstream_init(&bs);

    {
        const char* s = "Hello, world!";
        assert(qr_byte_mode_encode(&bs, s, strlen(s)));

        assert_bitstream_eq(&bs,
                            "01001000"
                            "01100101"
                            "01101100"
                            "01101100"
                            "01101111"
                            "00101100"
                            "00100000"
                            "01110111"
                            "01101111"
                            "01110010"
                            "01101100"
                            "01100100"
                            "00100001");
    }

    qr_bitstream_free(&bs);
}

void test_qr_kanji_mode_encode() {
    return;
}

void test_qr_assemble_data_codewords_empty() {
    qr_bitstream_t bs;
    qr_bitstream_init(&bs);

    assert(qr_assemble_data_codewords(&bs, QR_EC_LEVEL_L, 1));
    assert(bs.bit_len == 152);

    char* s = qr_bitstream_to_str(&bs);
    assert(s != NULL);
    assert(strncmp(s, "00000000", 8) == 0);

    free(s);
    qr_bitstream_free(&bs);
}

void test_qr_assemble_data_codewords_pad_pattern() {
    qr_bitstream_t bs;
    qr_bitstream_init(&bs);

    assert(qr_bitstream_append_byte(&bs, 0xAB));
    assert(qr_assemble_data_codewords(&bs, QR_EC_LEVEL_L, 1));
    assert(bs.bit_len == 152);

    char* s = qr_bitstream_to_str(&bs);
    assert(s != NULL);
    /*
     * 0xAB
     * terminator => 0000
     * alignment => 0000
     * then alternating EC 11 EC 11 ...
     */
    assert(strncmp(s, "10101011000000001110110000010001", 32) == 0);

    free(s);
    qr_bitstream_free(&bs);
}

void test_qr_assemble_data_codewords_full_capacity() {
    qr_bitstream_t bs;
    qr_bitstream_init(&bs);

    for (int i = 0; i < 19; i++) {
        assert(qr_bitstream_append_byte(&bs, 0xAA));
    }
    assert(bs.bit_len == 152);

    assert(qr_assemble_data_codewords(&bs, QR_EC_LEVEL_L, 1));
    assert(bs.bit_len == 152);

    qr_bitstream_free(&bs);
}

void test_qr_assemble_data_codewords_overflow() {
    qr_bitstream_t bs;
    qr_bitstream_init(&bs);

    for (int i = 0; i < 20; i++) {
        assert(qr_bitstream_append_byte(&bs, 0xAA));
    }

    assert(!qr_assemble_data_codewords(&bs, QR_EC_LEVEL_L, 1));

    qr_bitstream_free(&bs);
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

    TEST("QR char count indicator bits");
    test_qr_char_count_indicator_bits();

    TEST("QR numeric mode encode");
    test_qr_numeric_mode_encode();

    TEST("QR alphanumeric mode encode");
    test_qr_alphanumeric_mode_encode();

    TEST("QR byte mode encode");
    test_qr_byte_mode_encode();

    // TEST("QR kanji mode encode");
    // test_qr_kanji_mode_encode();

    TEST("QR assemble data codewords empty");
    test_qr_assemble_data_codewords_empty();

    TEST("QR assemble data codewords pad pattern");
    test_qr_assemble_data_codewords_pad_pattern();

    TEST("QR assemble data codewords full capacity");
    test_qr_assemble_data_codewords_full_capacity();

    TEST("QR assemble data codewords overflow");
    test_qr_assemble_data_codewords_overflow();

    printf("ALL ENCODING TESTS PASSED\n");
    return;
}
