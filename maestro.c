#include <stdlib.h>
#include "avr/io.h"
#include "maestro.h"

uint16_t servo_positions[24];

void maestro_init(){
	maestro_rst();
	maestro_txd(0xAA);
}

void maestro_position(uint8_t ch, uint16_t value){
	maestro_txd(0x84); 
	maestro_txd((char)ch);
	maestro_txd((char)(value >> 0) & 0x7F);
	maestro_txd((char)(value >> 7) & 0x7F);
	servo_positions[ch] = value;
}

void maestro_velocity(uint8_t ch, uint16_t value){
	maestro_txd(0x87); 
	maestro_txd((char)ch);
	maestro_txd((char)(value >> 0) & 0x7F);
	maestro_txd((char)(value >> 7) & 0x7F);
}

void maestro_acceleration(uint8_t ch, uint16_t value){
	maestro_txd(0x89); 
	maestro_txd((char)ch);
	maestro_txd((char)(value >> 0) & 0x7F);
	maestro_txd((char)(value >> 7) & 0x7F);
}

void maestro_position_time(uint8_t ch, uint16_t value, uint16_t ms){
	uint16_t dv = abs((int16_t)(value - servo_positions[ch]));
	maestro_acceleration(ch, dv/(ms/10));
	maestro_position(ch, value);
}