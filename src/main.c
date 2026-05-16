#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qr_encoding.h"

int main(int argc, char* argv[]) {
    const char* str = "HELLO WORLD";
    size_t len = strlen(str);
    size_t count;

    char (*encoded)[12] = qr_alphanumeric_mode_encode(str, len, &count);
    if (!encoded) {
        fprintf(stderr, "encoding failed\n");
        return 1;
    }

    printf("Input: %s\n\n", str);

    for (size_t i = 0; i < count; i++) {
        printf("chunk %zu: %s\n", i, encoded[i]);
    }

    free(encoded);

    return 0;
}