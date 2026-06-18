#include <assert.h>
#include <stdio.h>

#include "qr_gf.h"
#include "qr_poly.h"

#define TEST(name) printf("RUNNING: %s\n", name)

static void assert_poly_eq(qr_poly_t* p, uint8_t* expected, size_t len) {
    assert(p->len == len);
    for (size_t i = 0; i < len; i++) {
        assert(p->coeffs[i] == expected[i]);
    }
}

void test_qr_poly_init_free() {
    qr_poly_t p;

    assert(qr_poly_init(&p, 3));
    assert(p.len == 4);
    assert(p.coeffs != NULL);

    qr_poly_free(&p);
    assert(p.coeffs == NULL);
    assert(p.len == 0);
}

void test_qr_poly_add_basic() {
    qr_poly_t a, b, r;

    qr_poly_init(&a, 2);
    qr_poly_init(&b, 2);

    /* a = [1,2,3], b = [3,2,1] */
    a.coeffs[0] = 1;
    a.coeffs[1] = 2;
    a.coeffs[2] = 3;
    b.coeffs[0] = 3;
    b.coeffs[1] = 2;
    b.coeffs[2] = 1;

    assert(qr_poly_add(&r, &a, &b));

    /* XOR in GF(2^8) */
    uint8_t expected[] = {1 ^ 3, 0x2 ^ 2, 3 ^ 1};
    assert_poly_eq(&r, expected, 3);

    qr_poly_free(&a);
    qr_poly_free(&b);
    qr_poly_free(&r);
}

void test_qr_poly_mul_simple() {
    qr_poly_t a, b, r;

    qr_poly_init(&a, 1);  // degree 1
    qr_poly_init(&b, 1);

    a.coeffs[0] = 1;
    a.coeffs[1] = 2;
    b.coeffs[0] = 3;
    b.coeffs[1] = 4;

    assert(qr_poly_mul(&r, &a, &b));

    /*
        Expected:
        (1x + 2) * (3x + 4)
        = 3x^2 + (4+6)x + 8
        = 3x^2 + 2x + 8   (in GF)
    */
    uint8_t expected[] = {
        qr_gf_mul(1, 3),
        qr_gf_add(qr_gf_mul(1, 4), qr_gf_mul(2, 3)),
        qr_gf_mul(2, 4)};

    assert_poly_eq(&r, expected, 3);

    qr_poly_free(&a);
    qr_poly_free(&b);
    qr_poly_free(&r);
}

void test_qr_poly_mod_basic() {
    qr_poly_t a, b, r;

    /* a(x) = x^2 + 2x + 3 */
    qr_poly_init(&a, 2);
    a.coeffs[0] = 1;
    a.coeffs[1] = 2;
    a.coeffs[2] = 3;

    /* b(x) = x + 1 */
    qr_poly_init(&b, 1);
    b.coeffs[0] = 1;
    b.coeffs[1] = 1;

    assert(qr_poly_mod(&r, &a, &b));

    /*
      In GF(2^8), remainder should be degree < 1
      so just a single coefficient
    */
    assert(r.len == 1);

    qr_poly_free(&a);
    qr_poly_free(&b);
    qr_poly_free(&r);
}

void test_qr_poly_mod_div_zero() {
    qr_poly_t a, b, r;

    qr_poly_init(&a, 2);
    qr_poly_init(&b, 0);

    b.coeffs[0] = 0;

    assert(!qr_poly_mod(&r, &a, &b));

    qr_poly_free(&a);
    qr_poly_free(&b);
}

void test_poly() {
    TEST("poly init/free");
    test_qr_poly_init_free();

    TEST("poly add");
    test_qr_poly_add_basic();

    TEST("poly mul");
    test_qr_poly_mul_simple();

    TEST("poly mod");
    test_qr_poly_mod_basic();

    TEST("poly mod division-by-zero");
    test_qr_poly_mod_div_zero();

    return;
}