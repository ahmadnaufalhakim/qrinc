#ifndef QR_EC_H
#define QR_EC_H

#include <stddef.h>

#include "qr_encoding.h"

typedef enum {
    QR_EC_LEVEL_L,
    QR_EC_LEVEL_M,
    QR_EC_LEVEL_Q,
    QR_EC_LEVEL_H,

    QR_EC_LEVEL_COUNT,
} qr_ec_level_t;

int qr_detect_version(qr_mode_t mode, qr_ec_level_t ec_level, int min_version, size_t char_count);

#endif
