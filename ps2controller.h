#include <avr/io.h>
#include <util/delay.h>

#define PS2_CONTROLLER_MODE_DIGITAL 0x41
#define PS2_CONTROLLER_MODE_ANALOG 0x73


#define PS2PORT PORTE
#define PS2DATA 2
#define PS2COMMAND 1
#define PS2CLOCK 0
#define PS2ATTENTION 3

#define PS2_PIN_DIRSET(i) PS2PORT.DIRSET = (1 << i)
#define PS2_PIN_OUTSET(i) PS2PORT.OUTSET = (1 << i)
#define PS2_PIN_OUTCLR(i) PS2PORT.OUTCLR = (1 << i)
#define PS2_PIN_GET(i) (PS2PORT.IN & (1 << i))

typedef struct 
{
	uint8_t mode;
	uint16_t buttons_change;
	uint8_t buttons_pressure;
	uint16_t buttons;
	uint8_t rx;
	uint8_t ry;
	uint8_t lx;
	uint8_t ly;


}ps2_controller;

int ps2_txrx(short int command);

uint8_t  ps2_init_analog_mode();

uint8_t ps2_read_controller(ps2_controller* controller);