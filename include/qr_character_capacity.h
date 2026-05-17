#ifndef QR_CHARACTER_CAPACITY_H
#define QR_CHARACTER_CAPACITY_H

#include "qr_ec.h"
#include "qr_encoding.h"

size_t qr_character_capacity(qr_mode_t mode, qr_ec_level_t ec_level, int version);

#endif
