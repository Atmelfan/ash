//
// Created by atmelfan on 2017-03-05.
//

#include <math.h>
#include "leg.h"
#include "linalg.h"

void leg_calc_ik(const leg_t* ik, const vec4* pos) {
    float L1 = ik->lengths[0];
    float L2 = ik->lengths[1];
    float L3 = ik->lengths[2];

    float d_sq = pos->x*pos->x + pos->y*pos->y;
    float d = sqrt(d_sq) - L1;
    float s1 = (d*d + L2*L2 - L3*L3)/(2*d*L2);
    float s2 = (L2*L2 + L3*L3 - d*d)/(2*L2*L3);

    ik->angles[0] = atan2(pos->y, pos->x);
    ik->angles[1] = acos(s1) + atan2(pos->z, d);
    ik->angles[2] = acos(s2);
}

void leg_calc_ik_absolute(const leg_t* ik, const vec4* abs) {
    vec4 temp;
    mat4_vec(&ik->transform, abs, &temp);//Removes offset and fixes mirroring/rotation etc...
    leg_calc_ik(ik, &temp);
}

void leg_home(leg_t* ik) {
    leg_calc_ik(ik, &ik->home);
}
