#ifndef MAESTRO
#define MAESTRO

#define SERVO_RADIAN(radians) 1500*4+(radians/3.14f*500)*4//-PI = -PI/3.14*500 = 1500µs-500µs = 1ms

extern void maestro_rst(void);
extern char maestro_rxd(void);
extern void maestro_txd(char);

void maestro_init();

void maestro_position(uint8_t ch, uint16_t value);

void maestro_velocity(uint8_t ch, uint16_t value);

void maestro_acceleration(uint8_t ch, uint16_t value);

void maestro_position_time(uint8_t ch, uint16_t value, uint16_t ms);

#endif





