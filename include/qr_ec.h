#ifndef QR_EC_H
#define QR_EC_H

#include "qr_types.h"

const char* qr_ec_level_to_str(qr_ec_level_t ec_level);

int qr_detect_version(
    qr_mode_t mode,
    qr_ec_level_t ec_level,
    int min_version,
    size_t char_count);

#endif
