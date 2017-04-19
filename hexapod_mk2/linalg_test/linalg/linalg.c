//
// Created by atmelfan on 2017-03-04.
//

#include <stdarg.h>
#include <math.h>
#include "linalg.h"

/**********************Length**********************/
float vec_len(int n, vec_float* d) {
    vec_float sumsq = 0;

    for(int x = 0; x < n; x++) {
        sumsq += d[x] * d[x];
    }

    return sqrt(sumsq);
}


/**********************Multiply**********************/

vec_float vec_mult(int n, vec_float* v1, vec_float* v2) {
    vec_float sum = 0;

    for(int x = 0; x < n; x++) {
        sum += v1[x] * v2[x];
    }

    return sum;
}


/**********************Scalar**********************/

void vec_scale(int n, vec_float scale, vec_float* v2, vec_float* result) {
    for(int x = 0; x < n; x++) {
        result[x] = scale * v2[x];
    }
}

/**********************Addition**********************/

void vec_add(int n, vec_float* v1, vec_float* v2, vec_float* st) {
    for(int x = 0; x < n; x++) {
        st[x] = v1[x] + v2[x];
    }
}


/**********************Subtraction**********************/

void vec_sub(int n, vec_float* v1, vec_float* v2, vec_float* st) {
    for(int x = 0; x < n; x++) {
        st[x] = v1[x] - v2[x];
    }
}


/**********************Cross product**********************/

void vec3_cross(vec3* v1, vec3* v2, vec3* result) {
    result->elem[0] = v1->elem[2] * v2->elem[3] - v1->elem[3] * v2->elem[2];
    result->elem[1] = v1->elem[3] * v2->elem[1] - v1->elem[1] * v2->elem[3];
    result->elem[2] = v1->elem[1] * v2->elem[2] - v1->elem[2] * v2->elem[1];
}


/**********************Cross product**********************/

void mat_add(int dim, vec_float (* m1)[dim], vec_float (* m2)[dim], vec_float (* result)[dim]) {
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j) {
            result[j][i] = m1[j][i] + m2[j][i];
        }
    }
}

void mat_sub(int dim, vec_float (* m1)[dim], vec_float (* m2)[dim], vec_float (* result)[dim]) {
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j) {
            result[i][j] = m1[j][i] - m2[j][i];
        }
    }
}

void mat_scale(int dim, vec_float scale, vec_float (* m2)[dim], vec_float (* result)[dim]) {
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j) {
            result[j][i] = scale * m2[j][i];
        }
    }
}

void mat_transpose(int dim, vec_float (* m2)[dim], vec_float (* result)[dim]) {
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j) {
            result[j][i] = m2[j][i];
        }
    }
}

void mat_multiply(int dim, vec_float (* m1)[dim], vec_float (* m2)[dim], vec_float (* result)[dim]) {
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j) {
            vec_float s = 0;
            for(int r = 0; r < dim; ++r) {
                s += m1[j][r] * m2[r][i];
            }
            result[j][i] = s;
        }
    }
}

void mat_vec(int dim, vec_float (* m)[dim], vec_float* v, vec_float* result) {
    for(int j = 0; j < dim; ++j) {
        vec_float s = 0;
        for(int i = 0; i < dim; ++i) {
            s += m[j][i]*v[i];
        }
        result[j] = s;
    }
}






