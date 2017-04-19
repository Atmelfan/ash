//
// Created by atmelfan on 2017-03-05.
//

#ifndef HEXAPOD_LEG_H
#define HEXAPOD_LEG_H

#include "linalg.h"

typedef struct{
    vec4 home;
    mat4 transform;//Removes offset and potentially mirrors/rotates
    float lengths[3];
    float angles[3];
} leg_t;

void leg_calc_ik(const leg_t* ik, const vec4* pos);
void leg_calc_ik_absolute(const leg_t* ik, const vec4* abs);
void leg_home(leg_t* ik);

#endif //HEXAPOD_LEG_H
