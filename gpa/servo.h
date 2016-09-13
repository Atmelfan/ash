#ifndef _GPA_SERVO_H_
#define _GPA_SERVO_H_

#define SERVO_SETTING_MIRROR 0x01


typedef struct {
	uint16_t position;
	uint16_t pin;
	uint8_t settings;
}servo_t;

/*
* List of servos and their pin number
*/
extern servo_t servos[];
/*
* Length of servo array above
*/
extern uint8_t servos_length;
/*
* Called during servo_init to init the timer
*/
extern void servo_timer_init();
/*
* Called to schedule the next update
* us = 0: reset timer and schedule next update in 20000us
* us = [1,10000]: schedule next update in $us
*/
extern void servo_timer_set(uint16_t us);
/*
* Called to set a pin high
*/
extern void servo_set_pin(uint8_t pin);
/*
* Called to set a pin low
*/
extern void servo_clr_pin(uint8_t pin);
/*
* Called to make the pin an output
*/
extern void servo_out_pin(uint8_t pin);

void servo_update();

void servo_init();

void servo_start();

void servo_stop();

void servo_set(uint8_t servo, uint16_t us);

void servo_set_lim(uint8_t servo, uint16_t us, uint16_t min, uint16_t max);





#endif






