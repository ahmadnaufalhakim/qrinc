#ifndef QR_EC_BLOCK_INFO_H
#define QR_EC_BLOCK_INFO_H

#include "qr_ec.h"

typedef struct {
    uint8_t blocks;
    uint8_t data_codewords_per_block;
} qr_ec_group_t;

typedef struct {
    uint8_t ec_codewords_per_block;
    qr_ec_group_t groups[2];
} qr_ec_block_info_t;

qr_ec_block_info_t qr_ec_block_info(qr_ec_level_t ec_level, int version);

#endif
