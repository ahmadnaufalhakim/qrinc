#ifndef QR_BITSTREAM_H
#define QR_BITSTREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t* data;
    size_t bit_len;
    size_t byte_cap;
} qr_bitstream_t;

bool qr_bitstream_init(qr_bitstream_t* bs);
void qr_bitstream_free(qr_bitstream_t* bs);

char* qr_bitstream_to_str(qr_bitstream_t* bs);
void qr_bitstream_print(qr_bitstream_t* bs);

bool qr_bitstream_append_bit(qr_bitstream_t* bs, bool bit);
bool qr_bitstream_append_bits(qr_bitstream_t* bs, uint32_t value, int count);
bool qr_bitstream_append_byte(qr_bitstream_t* bs, uint8_t byte);

#endif