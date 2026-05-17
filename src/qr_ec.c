#include "qr_ec.h"

#include "qr_char_capacity.h"

int qr_detect_version(qr_mode_t mode, qr_ec_level_t ec_level, int min_version, size_t char_count) {
    switch (ec_level) {
        case QR_EC_LEVEL_L:
        case QR_EC_LEVEL_M:
        case QR_EC_LEVEL_Q:
        case QR_EC_LEVEL_H:
            if (min_version < 1 || min_version > 40) {
                return 0;
            }
            int total = 40 - (min_version - 1);

            for (int i = 0; i < total; i++) {
                int version = i + min_version;
                size_t low_char_capacity = qr_char_capacity(mode, ec_level, version);
                size_t high_char_capacity = qr_char_capacity(mode, ec_level, 40 - version - 1);

                if (low_char_capacity >= char_count) {
                    return version;
                }
                if (high_char_capacity < char_count) {
                    if (version != 0) {
                        return 40 - version;
                    } else {
                        return 0;
                    }
                }
            }

            return 0;
        default:
            return 0;
    }
}
