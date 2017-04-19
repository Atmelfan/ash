#include <stdio.h>
#include "linalg/linalg.h"
#include "testext.h"

void vec_print(int n, vec_float* v){
    printf("[");
    for (int i = 0; i < n; ++i) {
        if(i < n-1)
            printf("%f, ", v[i]);
        else
            printf("%f", v[i]);

    }
    printf("]\n");
}

void mat_print(int n, vec_float (*v)[n]){
    printf("[");
    for (int j = 0; j < n; ++j) {
        if(j > 0)
            printf(" ");
        for (int i = 0; i < n; ++i) {
            if(i < n-1)
                printf("%f, ", v[j][i]);
            else
                printf("%f", v[j][i]);
        }
        if(j < n-1)
            printf("\n");
    }

    printf("]\n");
}

int main() {
    vec4 a = {{1, 2, 3, 4}};
    vec4 b = {{5, 6, 7, 8}};
    mat4 m = {{ 1,  2,  3,  4,
                5,  6,  7,  8,
                9, 10, 11, 12,
               13, 14, 15, 16}};
    mat4 r;
    vec4 c;
    //vec_add(2, a.elem, b.elem, c.elem);
    printf("a = \n");
     vec_print(4, a.elem);
    printf("b = \n");
     vec_print(4, b.elem);
    printf("M = \n");
     mat_print(4, m.elem);
    printf("M x a = \n");
     mat4_vec(&m, &a, &c);
     vec_print(4, c.elem);
    printf("a x b = %f\n", vec4_mult(&a, &b));
    printf("M x M = \n");
     mat4_multiply(&m, &m, &r);
     mat_print(4, r.elem);

    return 0;
}