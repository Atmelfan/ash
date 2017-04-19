//
// Created by atmelfan on 2017-03-05.
//


#include <stdint.h>
#include "gpa_servos.h"

uint8_t gpa_servo_configure(uint8_t id, gpa_servo_conf conf) {


    return gpa_servo_getparam(id, GPA_SERVO_WHOAMI);
}

uint16_t gpa_servo_getparam(uint8_t id, uint16_t param) {
    return 0;
}

void gpa_servo_setparam(uint8_t id, uint16_t param, uint16_t value) {

}

void gpa_servo_enable(uint8_t id, bool enable) {
    gpa_servo_setparam(id, GPA_SERVO_ENABLE, enable ? 0 : 1);
}

void gpa_servo_set(uint8_t id, uint16_t position) {
    gpa_servo_setparam(id, GPA_SERVO_POSITION, position);
}

uint16_t gpa_servo_get(uint8_t id) {
    return gpa_servo_getparam(id, GPA_SERVO_POSITION);
}
