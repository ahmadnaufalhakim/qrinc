#ifndef QR_POLY_H
#define QR_POLY_H

#include "qr_types.h"

bool qr_poly_init(qr_poly_t* p, size_t degree);
void qr_poly_free(qr_poly_t* p);

bool qr_poly_add(qr_poly_t* p, const qr_poly_t* a, const qr_poly_t* b);
bool qr_poly_mul(qr_poly_t* p, const qr_poly_t* a, const qr_poly_t* b);
bool qr_poly_mod(qr_poly_t* p, const qr_poly_t* a, const qr_poly_t* b);

#endif
