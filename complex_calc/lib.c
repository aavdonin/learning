#include <stdio.h>
#include "lib.h"

struct complex get_complex() {
    struct complex a;
    printf("Enter real and imaginary part of complex number:\n");
    printf("Real:      "); scanf("%f",&a.real);
    printf("Imaginary: "); scanf("%f",&a.imgry);
    return a;
}

void print_result(struct complex c) {
    printf("Result is: %.4f + %.4fi\n", c.real, c.imgry);
}

void get_numbers(struct complex *a, struct complex *b) {
    printf("1st complex number:\n");
    *a = get_complex();
    printf("2nd complex number:\n");
    *b = get_complex();
}
