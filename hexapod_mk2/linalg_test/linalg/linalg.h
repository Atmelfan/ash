//
// Created by atmelfan on 2017-03-04.
//

#ifndef HEXAPOD_LINALG_H
#define HEXAPOD_LINALG_H

#define vec_float float


/**********************************************************************************
 *                                 vector/matrix structs                          *
 **********************************************************************************/

typedef union{
    struct {vec_float x, y; };
    vec_float elem[2];
} vec2;

typedef struct{
    union{
        struct {vec_float x, y, z; };
        vec_float elem[3];
    };
} vec3;

typedef struct{
    union{
        struct {vec_float x, y, z, w; };
        vec_float x1, x2, x3, x4;
        vec_float elem[4];
    };
} vec4;

typedef union {
    struct {
        vec_float   m11, m12,
                    m21, m22;
    };
    vec_float elem[2][2];
} mat2;

typedef union {
    struct {
        vec_float   m11, m12, m13,
                    m21, m22, m23,
                    m31, m32, m33;
    };
    vec_float elem[3][3];
} mat3;

typedef union {
    struct {
        vec_float   m11, m12, m13, m14,
                m21, m22, m23, m24,
                m31, m32, m33, m34,
                m41, m42, m43, m44;
    };
    vec_float elem[4][4];
} mat4;

/**********************************************************************************
 *                                       functions                                *
 **********************************************************************************/

/**vector math**/
#define MAKE_VEC2(dim, name) static inline void vec##dim##_##name (vec##dim *v1, vec##dim *v2, vec##dim *result) { vec_##name ( dim, v1->elem, v2->elem, result->elem ); }
#define MAKE_VECF(dim, name) static inline void vec##dim##_##name (vec_float scale, vec##dim *v2, vec##dim *result) { vec_##name ( dim, scale, v2->elem, result->elem ); }
#define MAKE_VEC1(dim, name) static inline vec_float vec##dim##_##name (vec##dim *v1, vec##dim *v2) { return vec_##name ( dim, v1->elem, v2->elem ); }
#define MAKE_VECL(dim, name) static inline vec_float vec##dim##_##name (vec##dim *v1) { return vec_##name ( dim, v1->elem); }


vec_float vec_len(int n, vec_float* d);
MAKE_VECL(2, len)
MAKE_VECL(3, len)
MAKE_VECL(4, len)

vec_float vec_mult(int n, vec_float* v1, vec_float* v2);
MAKE_VEC1(2, mult)
MAKE_VEC1(3, mult)
MAKE_VEC1(4, mult)


void vec_scale(int n, vec_float scale, vec_float* v2, vec_float* st);
MAKE_VECF(2, scale)
MAKE_VECF(3, scale)
MAKE_VECF(4, scale)

void vec_add(int n, vec_float* v1, vec_float* v2, vec_float* st);
MAKE_VEC2(2, add)
MAKE_VEC2(3, add)
MAKE_VEC2(4, add)

void vec_sub(int n, vec_float* v1, vec_float* v2, vec_float* st);
MAKE_VEC2(2, sub)
MAKE_VEC2(3, sub)
MAKE_VEC2(4, sub)

void vec3_cross(vec3* v1, vec3* v2, vec3* result);

/**matrix math**/
#define MAKE_MAT2(name, dim) static inline void mat##dim##_##name (mat##dim *m1, mat##dim *m2, mat##dim *res) { mat_##name ( dim, m1->elem, m2->elem, res->elem); }
#define MAKE_MAT1(name, dim) static inline void mat##dim##_##name (mat##dim *m2, mat##dim *res) { mat_##name ( dim, m2->elem, res->elem); }
#define MAKE_MATF(name, dim) static inline void mat##dim##_##name (vec_float scale, mat##dim *m2, mat##dim *res) { mat_##name ( dim, scale, m2->elem, res->elem); }
#define MAKE_MATV(name, dim) static inline void mat##dim##_##name (mat##dim *m1, vec##dim *v, vec##dim *res) { mat_##name ( dim, m1->elem, v->elem, res->elem); }


void mat_add(int dim, vec_float (* m1)[], vec_float (* m2)[], vec_float (* result)[]);
MAKE_MAT2(add, 2)
MAKE_MAT2(add, 3)
MAKE_MAT2(add, 4)

void mat_sub(int dim, vec_float (* m1)[], vec_float (* m2)[], vec_float (* result)[]);
MAKE_MAT2(sub, 2)
MAKE_MAT2(sub, 3)
MAKE_MAT2(sub, 4)

void mat_multiply(int dim, vec_float (* m1)[], vec_float (* m2)[], vec_float (* result)[]);
MAKE_MAT2(multiply, 2)
MAKE_MAT2(multiply, 3)
MAKE_MAT2(multiply, 4)

void mat_scale(int dim, vec_float scale, vec_float (* m2)[], vec_float (* result)[]);
MAKE_MATF(scale, 2)
MAKE_MATF(scale, 3)
MAKE_MATF(scale, 4)

void mat_transpose(int dim, vec_float (* m2)[], vec_float (* result)[]);
MAKE_MAT1(transpose, 2)
MAKE_MAT1(transpose, 3)
MAKE_MAT1(transpose, 4)

void mat_vec(int dim, vec_float (* m)[], vec_float* v, vec_float* result);
MAKE_MATV(vec, 2)
MAKE_MATV(vec, 3)
MAKE_MATV(vec, 4)


#endif //HEXAPOD_LINALG_H
