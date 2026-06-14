#ifndef QR_GF_H
#define QR_GF_H

#include <stdint.h>

uint8_t qr_gf_exp(uint8_t x);
uint8_t qr_gf_log(uint8_t x);
uint8_t qr_gf_add(uint8_t x, uint8_t y);
uint8_t qr_gf_mul(uint8_t x, uint8_t y);

#endif
