#include "qr_encoding.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int8_t qr_alphanumeric_value(uint8_t c) {
    if (!initialized) {
        qr_init_alphanumeric_table();
    }

    return qr_alphanumeric_table[c];
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
            return "UNKNOWN";
    }
}

const char* qr_mode_indicator(qr_mode_t mode) {
    switch (mode) {
        case QR_MODE_NUMERIC:
            return "0001";
        case QR_MODE_ALPHANUMERIC:
            return "0010";
        case QR_MODE_BYTE:
            return "0100";
        case QR_MODE_KANJI:
            return "1000";
        default:
            return "UNKNOWN";
    }
}

void print_binary(unsigned value, int bits, char* out) {
    for (int i = bits - 1; i >= 0; i--) {
        *out++ = (1 & (value >> i)) ? '1' : '0';
    }
    *out = '\0';
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

char (*qr_numeric_mode_encode(const char* data, size_t len, size_t* count))[11] {
    size_t n = (len + 2) / 3;
    if (count) {
        *count = n;
    }

    char (*chunks)[11] = (char (*)[11])malloc(n * sizeof(*chunks));
    if (!chunks) {
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        char chunk[4] = {0};
        strncpy(chunk, data + (i * 3), 3);

        size_t digits = strlen(chunk);
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
                free(chunks);
                return NULL;
        }

        print_binary(
            (unsigned)atoi(chunk),
            bits,
            chunks[i]);
    }

    return chunks;
}

unsigned qr_alphanumeric_pair_value(uint8_t c_a, uint8_t c_b) {
    unsigned value = (unsigned)qr_alphanumeric_value(c_a);

    if (c_b != '\0') {
        value = 45u * value + (unsigned)qr_alphanumeric_value(c_b);
    }

    return value;
}

char (*qr_alphanumeric_mode_encode(const char* data, size_t len, size_t* count))[12] {
    size_t n = (len + 1) / 2;
    if (count) {
        *count = n;
    }

    char (*chunks)[12] = (char (*)[12])malloc(n * sizeof(*chunks));
    if (!chunks) {
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        char chunk[3] = {0};
        strncpy(chunk, data + (i * 2), 2);

        size_t n_chars = strlen(chunk);
        int bits;
        switch (n_chars) {
            case 1:
                bits = 6;
                break;
            case 2:
                bits = 11;
                break;
            default:
                free(chunks);
                return NULL;
        }

        print_binary(
            qr_alphanumeric_pair_value(
                (uint8_t)chunk[0],
                (uint8_t)chunk[1]),
            bits,
            chunks[i]);
    }

    return chunks;
}

char (*qr_byte_mode_encode(const char* data, size_t len, size_t* count))[9] {
    size_t n = len;
    if (count) {
        *count = n;
    }

    char (*chunks)[9] = malloc(n * sizeof(*chunks));
    if (!chunks) {
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        print_binary(
            (unsigned)(uint8_t)data[i],
            8,
            chunks[i]);
    }

    return chunks;
}
