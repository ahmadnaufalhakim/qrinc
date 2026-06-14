#include <assert.h>
#include <stdio.h>

/* declarations from other test modules */
void test_encoding(void);
void test_ec(void);
void test_gf(void);

int main(void) {
    printf("==== RUNNING TEST SUITE ====\n\n");

    test_encoding();
    test_ec();
    test_gf();

    printf("\n==== ALL TESTS PASSED ====\n");
    return 0;
}