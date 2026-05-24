#ifndef QR_BITSTREAM_H
#define QR_BITSTREAM_H

#include "qr_types.h"

bool qr_bitstream_init(qr_bitstream_t* bs);
void qr_bitstream_free(qr_bitstream_t* bs);

char* qr_bitstream_to_str(qr_bitstream_t* bs);
void qr_bitstream_print_str(qr_bitstream_t* bs);
void qr_bitstream_print_bytes(qr_bitstream_t* bs);

bool qr_bitstream_append_bit(qr_bitstream_t* bs, bool bit);
bool qr_bitstream_append_bits(qr_bitstream_t* bs, uint32_t value, int count);
bool qr_bitstream_append_byte(qr_bitstream_t* bs, uint8_t byte);

#endif
