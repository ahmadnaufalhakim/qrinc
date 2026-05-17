#ifndef QR_ENCODING_H
#define QR_ENCODING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    QR_MODE_NUMERIC,
    QR_MODE_ALPHANUMERIC,
    QR_MODE_BYTE,
    QR_MODE_KANJI,

    QR_MODE_COUNT,
} qr_mode_t;

// Utilities

int8_t qr_alphanumeric_value(uint8_t c);
const char* qr_mode_to_str(qr_mode_t mode);
void print_binary(unsigned value, int bits, char* out);

// Character checks

bool qr_is_numeric(uint8_t c);
bool qr_is_alphanumeric(uint8_t c);
bool qr_is_kanji(uint8_t c);

// Mode detection

bool qr_mode_can_encode(const uint8_t* s, size_t len, qr_mode_t mode);
qr_mode_t qr_detect_mode(const uint8_t* s, size_t len);

// Encodings

char (*qr_numeric_mode_encode(const char* data, size_t len, size_t* count))[11];
char (*qr_alphanumeric_mode_encode(const char* data, size_t len, size_t* count))[12];
char (*qr_byte_mode_encode(const char* data, size_t len, size_t* count))[9];

#endif
