#include "qr_encoding.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qr_bitstream.h"
#include "qr_ec_block_info.h"

static int8_t qr_alphanumeric_table[256];
static int initialized;

static void qr_init_alphanumeric_table(void) {
    for (int i = 0; i < 256; i++) {
        qr_alphanumeric_table[i] = -1;
    }

    const char* alphabet =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

    for (int i = 0; alphabet[i] != '\0'; i++) {
        qr_alphanumeric_table[(uint8_t)alphabet[i]] = (int8_t)i;
    }

    initialized = 1;
}

static const int qr_char_count_indicator_bits_table[QR_MODE_COUNT][3] = {
    [QR_MODE_NUMERIC] = {10, 12, 14},
    [QR_MODE_ALPHANUMERIC] = {9, 11, 13},
    [QR_MODE_BYTE] = {8, 16, 16},
    [QR_MODE_KANJI] = {8, 10, 12},
};

int8_t qr_alphanumeric_value(uint8_t c) {
    if (!initialized) {
        qr_init_alphanumeric_table();
    }

    return qr_alphanumeric_table[c];
}

unsigned qr_alphanumeric_pair_value(uint8_t c_a, uint8_t c_b) {
    unsigned value = (unsigned)qr_alphanumeric_value(c_a);

    if (c_b != '\0') {
        value = 45u * value + (unsigned)qr_alphanumeric_value(c_b);
    }

    return value;
}

const char* qr_mode_to_str(qr_mode_t mode) {
    switch (mode) {
        case QR_MODE_NUMERIC:
            return "NUMERIC";
        case QR_MODE_ALPHANUMERIC:
            return "ALPHANUMERIC";
        case QR_MODE_BYTE:
            return "BYTE";
        case QR_MODE_KANJI:
            return "KANJI";
        default:
            return NULL;
    }
}

uint8_t qr_mode_indicator(qr_mode_t mode) {
    switch (mode) {
        case QR_MODE_NUMERIC:
            return 0x1;
        case QR_MODE_ALPHANUMERIC:
            return 0x2;
        case QR_MODE_BYTE:
            return 0x4;
        case QR_MODE_KANJI:
            return 0x8;
        default:
            return 0;
    }
}

int qr_char_count_indicator_bits(qr_mode_t mode, int version) {
    if (version < 1 || version > 40) {
        return -1;
    }

    int group;

    if (version <= 9) {
        group = 0;
    } else if (version <= 26) {
        group = 1;
    } else if (version <= 40) {
        group = 2;
    }

    return qr_char_count_indicator_bits_table[mode][group];
}

bool qr_is_numeric(uint8_t c) {
    return c >= '0' && c <= '9';
}

bool qr_is_alphanumeric(uint8_t c) {
    return qr_alphanumeric_value(c) != -1;
}

bool qr_mode_can_encode(const uint8_t* s, size_t len, qr_mode_t mode) {
    switch (mode) {
        case QR_MODE_NUMERIC:
            for (size_t i = 0; i < len; i++) {
                if (!qr_is_numeric(s[i])) {
                    return false;
                }
            }
            return true;
        case QR_MODE_ALPHANUMERIC:
            for (size_t i = 0; i < len; i++) {
                if (!qr_is_alphanumeric(s[i])) {
                    return false;
                }
            }
            return true;
        case QR_MODE_BYTE:
            return true;
        case QR_MODE_KANJI:
            // TODO: implement kanji encoding mode checking
            return false;
        default:
            return false;
    }
}

qr_mode_t qr_detect_mode(const uint8_t* s, size_t len) {
    bool numeric_ok = true,
         alphanumeric_ok = true,
         kanji_ok = true;

    for (size_t i = 0; i < len; i++) {
        if (numeric_ok && !qr_is_numeric(s[i])) {
            numeric_ok = false;
        }

        if (alphanumeric_ok && !qr_is_alphanumeric(s[i])) {
            alphanumeric_ok = false;
        }

        // TODO: implement kanji encoding mode checking
        kanji_ok = false;
    }

    if (numeric_ok) {
        return QR_MODE_NUMERIC;
    }
    if (alphanumeric_ok) {
        return QR_MODE_ALPHANUMERIC;
    }
    if (kanji_ok) {
        return QR_MODE_KANJI;
    }
    return QR_MODE_BYTE;
}

bool qr_numeric_mode_encode(qr_bitstream_t* bs, const char* data, size_t len) {
    for (size_t i = 0; i < len; i += 3) {
        char chunk[4] = {0};
        size_t digits = len - i;
        if (digits > 3) {
            digits = 3;
        }
        memcpy(chunk, data + i, digits);

        unsigned value = (unsigned)atoi(chunk);

        int bits;
        switch (digits) {
            case 1:
                bits = 4;
                break;
            case 2:
                bits = 7;
                break;
            case 3:
                bits = 10;
                break;
            default:
                return false;
        }

        if (!qr_bitstream_append_bits(bs, value, bits)) {
            return false;
        }
    }

    return true;
}

bool qr_alphanumeric_mode_encode(qr_bitstream_t* bs, const char* data, size_t len) {
    for (size_t i = 0; i < len; i += 2) {
        char chunk[3] = {0};
        size_t n_chars = len - i;
        if (n_chars > 2) {
            n_chars = 2;
        }
        memcpy(chunk, data + i, n_chars);

        unsigned value =
            qr_alphanumeric_pair_value(
                (uint8_t)chunk[0],
                (uint8_t)chunk[1]);

        int bits;
        switch (n_chars) {
            case 1:
                bits = 6;
                break;
            case 2:
                bits = 11;
                break;
            default:
                return false;
        }

        if (!qr_bitstream_append_bits(bs, value, bits)) {
            return false;
        }
    }

    return true;
}

bool qr_byte_mode_encode(qr_bitstream_t* bs, const char* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (!qr_bitstream_append_byte(bs, (uint8_t)data[i])) {
            return false;
        }
    }

    return true;
}

bool qr_assemble_data_codewords(
    qr_bitstream_t* bs,
    qr_ec_level_t ec_level,
    int version) {
    qr_ec_block_info_t ec = qr_ec_block_info(ec_level, version);

    int total_data_codewords =
        ec.groups[0].blocks * ec.groups[0].data_codewords_per_block +
        ec.groups[1].blocks * ec.groups[1].data_codewords_per_block;
    int total_data_bits = total_data_codewords * 8;

    int remaining_bits = total_data_bits - (int)bs->bit_len;
    if (remaining_bits < 0) {
        fprintf(stderr, "input bits exceed data capacity");
        return false;
    }

    int terminator_bits = remaining_bits < 4 ? remaining_bits : 4;
    if (!qr_bitstream_append_bits(bs, 0, terminator_bits)) {
        return false;
    }

    int remainder_bits = (8 - (int)bs->bit_len % 8) % 8;
    if (!qr_bitstream_append_bits(bs, 0, remainder_bits)) {
        return false;
    }

    static const uint8_t pads[2] = {0xEC, 0x11};

    int total_pad_bytes = (total_data_bits - (int)bs->bit_len) / 8;
    for (int i = 0; i < total_pad_bytes; i++) {
        if (!qr_bitstream_append_byte(bs, pads[i % 2])) {
            return false;
        }
    }

    return true;
}
