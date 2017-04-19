#include <stdio.h>
#include "movement/linalg.h"

int main() {
    vec2 a = {{1.0f, 2.0f}};
    vec2 b = {{3, 4}};
    vec2 c;
    vec2_add(&a, &b, &c);

    printf("Hello, World!\n");
    return 0;
}