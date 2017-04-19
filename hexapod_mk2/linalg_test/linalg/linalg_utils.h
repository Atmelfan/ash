//
// Created by atmelfan on 2017-03-05.
//

#ifndef LINALG_TEST_LINALG_UTILS_H
#define LINALG_TEST_LINALG_UTILS_H

#define MAT4_ROTX(theta)  {{1, 0, 0, 0,\
                            0, cos(theta), -sin(theta), 0,\
                            0, sin(theta), cos(theta), 0,\
                            0, 0, 0, 1}}

#define MAT4_ROTY(theta)  {{cos(theta), 0, sin(theta), 0,\
                            0, 1, 0, 0,\
                            -sin(theta), 0, cos(theta), 0,\
                            0, 0, 0, 1}}

#define MAT4_ROTZ(theta)  {{cos(theta), -sin(theta), 0, 0,\
                            sin(theta), cos(theta), 0, 0,\
                            0, 0, 1, 0,\
                            0, 0, 0, 1}}

#define MAT4_TRANS(x, y, z) {{1, 0, 0, x,\
                              0, 1, 0, y,\
                              0, 0, 1, z,\
                              0, 0, 0, 1}}

#define MAT4_SCALE(x, y, z) {{x, 0, 0, 0,\
                              0, y, 0, 0,\
                              0, 0, z, 0,\
                              0, 0, 0, 1}}

#endif //LINALG_TEST_LINALG_UTILS_H
