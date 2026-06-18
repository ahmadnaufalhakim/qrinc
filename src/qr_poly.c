#include "qr_poly.h"

#include <stdint.h>
#include <stdlib.h>

#include "qr_gf.h"

bool qr_poly_init(qr_poly_t* p, size_t degree) {
    uint8_t* buf = calloc(degree + 1, sizeof(uint8_t));
    if (!buf) return false;

    p->len = degree + 1;
    p->coeffs = buf;
    return true;
}

void qr_poly_free(qr_poly_t* p) {
    free(p->coeffs);
    p->len = 0;
    p->coeffs = NULL;
    return;
}

bool qr_poly_add(qr_poly_t* p, const qr_poly_t* a, const qr_poly_t* b) {
    if (!qr_poly_init(p, (a->len > b->len) ? a->len - 1 : b->len - 1)) {
        return false;
    }

    for (size_t i = 0; i < p->len; i++) {
        p->coeffs[p->len - i - 1] = qr_gf_add(
            (i < a->len) ? a->coeffs[a->len - i - 1] : 0,
            (i < b->len) ? b->coeffs[b->len - i - 1] : 0);
    }

    return true;
}

bool qr_poly_mul(qr_poly_t* p, const qr_poly_t* a, const qr_poly_t* b) {
    if (!qr_poly_init(p, a->len + b->len - 2)) {
        return false;
    }

    for (size_t i = 0; i < a->len; i++) {
        for (size_t j = 0; j < b->len; j++) {
            p->coeffs[i + j] = qr_gf_add(
                p->coeffs[i + j],
                qr_gf_mul(
                    a->coeffs[i],
                    b->coeffs[j]));
        }
    }

    return true;
}

bool qr_poly_mod(qr_poly_t* p, const qr_poly_t* a, const qr_poly_t* b) {
    size_t m = a->len;
    size_t n = b->len;

    // division by zero
    if (n == 0 || (n == 1 && b->coeffs[0] == 0)) return false;

    // remainder degree must be < divisor's degree
    if (!qr_poly_init(p, n - 2)) {
        return false;
    }

    // dividend = a (copy)
    qr_poly_t dividend;
    if (!qr_poly_init(&dividend, m - 1)) return false;
    for (size_t i = 0; i < m; i++) {
        dividend.coeffs[i] = a->coeffs[i];
    }

    // divisor = b (working buffer, will be shifted/scaled conceptually)
    qr_poly_t divisor;
    if (!qr_poly_init(&divisor, n - 1)) {
        qr_poly_free(&dividend);
        return false;
    }
    for (size_t i = 0; i < n; i++) {
        divisor.coeffs[i] = b->coeffs[i];
    }

    for (size_t i = 0; i < m; i++) {
        // skip leading zero in dividend
        if (dividend.coeffs[0] == 0) {
            for (size_t k = 1; k < dividend.len; k++) {
                dividend.coeffs[k - 1] = dividend.coeffs[k];
            }
            dividend.coeffs[dividend.len - 1] = 0;
            continue;
        }

        uint8_t a_lead_coef_exp = qr_gf_log(dividend.coeffs[0]);
        uint8_t b_lead_coef_exp = qr_gf_log(b->coeffs[0]);

        uint16_t a = (uint16_t)a_lead_coef_exp;
        uint16_t b = (uint16_t)b_lead_coef_exp;
        uint16_t multiplierExp = (uint16_t)(255u + a - b);
        uint8_t scale = qr_gf_exp((uint8_t)(multiplierExp % 255));

        // scaled divisor
        qr_poly_t scaled;
        if (!qr_poly_init(&scaled, divisor.len - 1)) {
            qr_poly_free(&dividend);
            qr_poly_free(&divisor);
            return false;
        }
        for (size_t j = 0; j < divisor.len; j++) {
            scaled.coeffs[j] = qr_gf_mul(divisor.coeffs[j], scale);
        }

        // dividend = dividend + scaled
        size_t min_len = (dividend.len < scaled.len) ? dividend.len : scaled.len;
        for (size_t j = 0; j < min_len; j++) {
            dividend.coeffs[j] = qr_gf_add(dividend.coeffs[j], scaled.coeffs[j]);
        }
        qr_poly_free(&scaled);

        // shift left
        for (size_t k = 1; k < dividend.len; k++) {
            dividend.coeffs[k - 1] = dividend.coeffs[k];
        }
        dividend.coeffs[dividend.len - 1] = 0;
    }

    // result = dividend remainder
    for (size_t i = 0; i < p->len && i < dividend.len; i++) {
        p->coeffs[i] = dividend.coeffs[i];
    }

    qr_poly_free(&dividend);
    qr_poly_free(&divisor);

    return true;
}
