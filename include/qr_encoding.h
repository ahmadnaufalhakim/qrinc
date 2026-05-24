#ifndef QR_ENCODING_H
#define QR_ENCODING_H

#include "qr_types.h"

// Utilities

int8_t qr_alphanumeric_value(uint8_t c);
unsigned qr_alphanumeric_pair_value(uint8_t c_a, uint8_t c_b);
const char* qr_mode_to_str(qr_mode_t mode);
uint8_t qr_mode_indicator(qr_mode_t mode);
int qr_char_count_indicator_bits(qr_mode_t mode, int version);

// Character checks

bool qr_is_numeric(uint8_t c);
bool qr_is_alphanumeric(uint8_t c);
bool qr_is_kanji(uint8_t c);

// Mode detection

bool qr_mode_can_encode(const uint8_t* s, size_t len, qr_mode_t mode);
qr_mode_t qr_detect_mode(const uint8_t* s, size_t len);

// Encodings

bool qr_numeric_mode_encode(
    qr_bitstream_t* bs,
    const char* data,
    size_t len);
bool qr_alphanumeric_mode_encode(
    qr_bitstream_t* bs,
    const char* data,
    size_t len);
bool qr_byte_mode_encode(
    qr_bitstream_t* bs,
    const char* data,
    size_t len);

#endif
