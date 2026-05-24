#include "qr_bitstream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool qr_bitstream_reserve(qr_bitstream_t* bs, size_t needed_bits) {
    size_t needed_bytes = (needed_bits + 7) / 8;

    if (needed_bytes <= bs->byte_cap) {
        return true;
    }

    size_t new_byte_cap = bs->byte_cap
                              ? bs->byte_cap * 2
                              : 16;

    while (new_byte_cap < needed_bytes) {
        new_byte_cap *= 2;
    }

    uint8_t* p = realloc(bs->data, new_byte_cap);
    if (!p) {
        return false;
    }

    memset(
        p + bs->byte_cap,
        0,
        new_byte_cap - bs->byte_cap);

    bs->data = p;
    bs->byte_cap = new_byte_cap;

    return true;
}

bool qr_bitstream_init(qr_bitstream_t* bs) {
    bs->data = NULL;
    bs->bit_len = 0;
    bs->byte_cap = 0;
    return true;
}

void qr_bitstream_free(qr_bitstream_t* bs) {
    free(bs->data);
    bs->data = NULL;
    bs->bit_len = 0;
    return;
}

char* qr_bitstream_to_str(qr_bitstream_t* bs) {
    char* s = malloc(bs->bit_len + 1);
    if (!s) {
        return NULL;
    }

    for (size_t i = 0; i < bs->bit_len; i++) {
        size_t byte_idx = i / 8;
        int bit_offset = 7 - (i % 8);

        int bit = (bs->data[byte_idx] >> bit_offset) & 1;

        s[i] = bit ? '1' : '0';
    }
    s[bs->bit_len] = '\0';

    return s;
}

void qr_bitstream_print_str(qr_bitstream_t* bs) {
    char* s = qr_bitstream_to_str(bs);
    if (!s) {
        return;
    }

    puts(s);

    free(s);
}

void qr_bitstream_print_bytes(qr_bitstream_t* bs) {
    size_t byte_len = (bs->bit_len + 7) / 8;

    for (size_t i = 0; i < byte_len; i++) {
        printf("%02X ", bs->data[i]);
    }
    printf("\n");
}

bool qr_bitstream_append_bit(qr_bitstream_t* bs, bool bit) {
    if (!qr_bitstream_reserve(bs, bs->bit_len + 1)) {
        return false;
    }

    if (bit) {
        size_t byte_idx = bs->bit_len / 8;
        int bit_offset = 7 - (bs->bit_len % 8);

        bs->data[byte_idx] |= (uint8_t)(1u << bit_offset);
    }

    bs->bit_len++;

    return true;
}

bool qr_bitstream_append_bits(qr_bitstream_t* bs, uint32_t value, int count) {
    for (int i = count - 1; i >= 0; i--) {
        bool bit = (value >> i) & 1u;

        if (!qr_bitstream_append_bit(bs, bit)) {
            return false;
        }
    }

    return true;
}

bool qr_bitstream_append_byte(qr_bitstream_t* bs, uint8_t byte) {
    return qr_bitstream_append_bits(bs, byte, 8);
}
