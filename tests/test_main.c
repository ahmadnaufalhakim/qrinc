#include <assert.h>
#include <stdio.h>

/* declarations from other test modules */
void test_encoding(void);
void test_ec(void);

int main(void) {
    printf("==== RUNNING TEST SUITE ====\n\n");

    test_encoding();
    test_ec();

    printf("\n==== ALL TESTS PASSED ====\n");
    return 0;
}