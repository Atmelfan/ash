/*Auto generated! Don't touch!*/
#ifndef GPA_IMAGE_TABLE
#define GPA_IMAGE_TABLE
#include <avr/eeprom.h>
/*Image name: test2*/
	#define TEST2_OFFSET 0
	#define TEST2_WIDTH 16
	#define TEST2_HEIGHT 16
	#define TEST2_SIZE 256
	#define TEST2_GET_PIXEL(x,y) eeprom_read_byte((uint8_t*)(TEST2_OFFSET+y*TEST2_WIDTH+x))

#endif
