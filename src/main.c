#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qr_bitstream.h"
#include "qr_ec.h"
#include "qr_encoding.h"

int main(void) {
    const char* text = "Hello, world!👋 What a great day we're havin'!";
    const qr_ec_level_t ec_level = QR_EC_LEVEL_L;
    size_t len = strlen(text);

    // 1. Detect encoding mode
    qr_mode_t mode = qr_detect_mode((const uint8_t*)text, len);
    printf("Input: %s\n", text);
    printf("Mode: %s\n", qr_mode_to_str(mode));
    printf("Error Correction Level: %s\n\n", qr_ec_level_to_str(ec_level));

    // 2. Determine version
    int version = qr_detect_version(mode, ec_level, 1, len);

    printf("Version: %d\n\n", version);

    // 3. Create bitstream
    qr_bitstream_t bs;

    if (!qr_bitstream_init(&bs)) {
        fprintf(stderr, "bitstream init failed\n");
        qr_bitstream_free(&bs);
        return 1;
    }

    // 4. Append mode indicator (4 bits)
    uint8_t mode_indicator = qr_mode_indicator(mode);
    if (!qr_bitstream_append_bits(&bs, mode_indicator, 4)) {
        fprintf(stderr, "failed appending mode indicator\n");
        qr_bitstream_free(&bs);
        return 1;
    };

    // 5. Append character count indicator
    int cc_indicator_bits = qr_char_count_indicator_bits(mode, version);
    if (!qr_bitstream_append_bits(&bs, (uint32_t)len, cc_indicator_bits)) {
        fprintf(stderr, "failed appending char count\n");
        qr_bitstream_free(&bs);
        return 1;
    }

    // 6. Encode payload
    bool ok = false;
    switch (mode) {
        case QR_MODE_NUMERIC:
            ok = qr_numeric_mode_encode(&bs, text, len);
            break;

        case QR_MODE_ALPHANUMERIC:
            ok = qr_alphanumeric_mode_encode(&bs, text, len);
            break;

        case QR_MODE_BYTE:
            ok = qr_byte_mode_encode(&bs, text, len);
            break;

        default:
            fprintf(stderr, "unsupported mode\n");
            qr_bitstream_free(&bs);
            return 1;
    }

    if (!ok) {
        fprintf(stderr, "encoding failed\n");
        qr_bitstream_free(&bs);
        return 1;
    }

    // 7. Assemble data codewords
    ok = qr_assemble_data_codewords(&bs, ec_level, version);
    if (!ok) {
        fprintf(stderr, "data codewords assembling failed");
        qr_bitstream_free(&bs);
        return 1;
    }

    // 7. Print final bitstream
    printf("Final bit stream:\n");
    qr_bitstream_print(&bs);

    printf("\nTotal bits: %zu\n", bs.bit_len);

    // 8. Cleanup
    qr_bitstream_free(&bs);

    return 0;
}