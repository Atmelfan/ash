//
// Created by atmelfan on 2017-03-05.
//

#ifndef HEXAPOD_GPA_SERVOS_H
#define HEXAPOD_GPA_SERVOS_H


#include <stdint.h>
#include <stdbool.h>

typedef struct {

} gpa_servo_conf;

typedef enum {
    GPA_SERVO_WHOAMI = 0,
    GPA_SERVO_ENABLE,
    GPA_SERVO_POSITION,
    GPA_SERVO_PID_P,
    GPA_SERVO_PID_I,
    GPA_SERVO_PID_D,

} gpa_servo_param;

uint8_t gpa_servo_configure(uint8_t id, gpa_servo_conf conf);

void gpa_servo_enable(uint8_t id, bool enable);

void gpa_servo_set(uint8_t id, uint16_t position);

uint16_t gpa_servo_get(uint8_t id);

void gpa_servo_setparam(uint8_t id, uint16_t  param, uint16_t value);

uint16_t gpa_servo_getparam(uint8_t id, uint16_t param);

#endif //HEXAPOD_GPA_SERVOS_H
