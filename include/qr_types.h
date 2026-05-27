#ifndef QR_TYPES_H
#define QR_TYPES_H

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

typedef enum {
    QR_EC_LEVEL_L,
    QR_EC_LEVEL_M,
    QR_EC_LEVEL_Q,
    QR_EC_LEVEL_H,

    QR_EC_LEVEL_COUNT,
} qr_ec_level_t;

typedef struct {
    uint8_t* data;
    size_t bit_len;
    size_t byte_cap;
} qr_bitstream_t;

typedef struct {
    uint8_t blocks;
    uint8_t data_codewords_per_block;
} qr_ec_group_t;

typedef struct {
    uint8_t ec_codewords_per_block;
    qr_ec_group_t groups[2];
} qr_ec_block_info_t;

typedef struct {
    uint8_t* data_codewords;
    size_t data_len;

    uint8_t* ec_codewords;
    size_t ec_len;

    uint8_t group;
} qr_block_t;

typedef struct {
    size_t degree;
    const uint8_t* poly;
} qr_rs_poly_t;

#endif
