#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qr_ec.h"
#include "qr_encoding.h"

int main(int argc, char* argv[]) {
    const char* text = "Hello, world! 123";
    size_t len = strlen(text);

    // 1. Detect encoding mode
    qr_mode_t mode = qr_detect_mode((const uint8_t*)text, len);
    printf("Input: %s\n", text);
    printf("Mode: %s\n\n", qr_mode_to_str(mode));

    // 2. Encode input string
    size_t data_count = 0;

    // We'll store encoded chunks generically as char**
    char** data_bits = NULL;

    switch (mode) {
        case QR_MODE_NUMERIC: {
            char (*chunks)[11] =
                qr_numeric_mode_encode(text, len, &data_count);

            if (!chunks) {
                fprintf(stderr, "numeric encoding failed\n");
                return 1;
            }

            data_bits = malloc(data_count * sizeof(*data_bits));
            if (!data_bits) {
                free(chunks);
                return 1;
            }

            for (size_t i = 0; i < data_count; i++) {
                data_bits[i] = chunks[i];
            }

            break;
        }

        case QR_MODE_ALPHANUMERIC: {
            char (*chunks)[12] =
                qr_alphanumeric_mode_encode(text, len, &data_count);

            if (!chunks) {
                fprintf(stderr, "alphanumeric encoding failed\n");
                return 1;
            }

            data_bits = malloc(data_count * sizeof(*data_bits));
            if (!data_bits) {
                free(chunks);
                return 1;
            }

            for (size_t i = 0; i < data_count; i++) {
                data_bits[i] = chunks[i];
            }

            break;
        }

        case QR_MODE_BYTE: {
            char (*chunks)[9] =
                qr_byte_mode_encode(text, len, &data_count);

            if (!chunks) {
                fprintf(stderr, "byte encoding failed\n");
                return 1;
            }

            data_bits = malloc(data_count * sizeof(*data_bits));
            if (!data_bits) {
                free(chunks);
                return 1;
            }

            for (size_t i = 0; i < data_count; i++) {
                data_bits[i] = chunks[i];
            }

            break;
        }

        default:
            fprintf(stderr, "unsupported mode\n");
            return 1;
    }

    // 3. Determine version
    int version = qr_detect_version(mode, QR_EC_LEVEL_L, 29, data_count);

    printf("Version: %d\n\n", version);

    // 4. Construct bit strings

    const char* mode_indicator =
        qr_mode_indicator(mode);

    const char* char_count_indicator =
        qr_char_count_indicator(mode, version, len);

    if (!mode_indicator || !char_count_indicator) {
        fprintf(stderr, "indicator generation failed\n");
        free(data_bits);
        return 1;
    }

    printf("Mode indicator:       %s\n", mode_indicator);
    printf("Char count indicator: %s\n\n", char_count_indicator);

    printf("Data bits:\n");

    for (size_t i = 0; i < data_count; i++) {
        printf("chunk %zu: %s\n", i, data_bits[i]);
    }

    // Example: assemble final bit stream
    printf("\nFinal bit stream:\n");

    printf("%s", mode_indicator);
    printf("%s", char_count_indicator);

    for (size_t i = 0; i < data_count; i++) {
        printf("%s", data_bits[i]);
    }

    printf("\n");

    // Cleanup
    free((void*)char_count_indicator);

    // IMPORTANT:
    // data_bits points into the original chunks allocation.
    // We free only the original allocation once.

    if (mode == QR_MODE_NUMERIC) {
        free(data_bits[0]);
    } else if (mode == QR_MODE_ALPHANUMERIC) {
        free(data_bits[0]);
    } else if (mode == QR_MODE_BYTE) {
        free(data_bits[0]);
    }

    free(data_bits);

    return 0;
}