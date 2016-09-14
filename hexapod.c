#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h> 
#include <math.h>
#include <stdbool.h>
#include "ff.h"		/* Declarations of FatFs API */
#include "maestro.h"
#include "lcd.h"
#include "ps2controller.h"
#include "walkpatterns.h"


#define LED_PROCESSING (1 << 0)
#define LED_UPDATE (1 << 1)
#define LED_READING (1 << 2)
#define LED_ERROR (1 << 3)
#define MAESTRO_SERPORT USARTC0
#define MAESTRO_BAUDSCL 0
#define MAESTRO_BAUDSEL 103

#define PYTHAGORAS(a,b) sqrt((a)*(a) + (b)*(b))


#define TCC0_FRQ (F_CPU/1024)//1Hz
#define ENABLE_INTERRUP(mask) PMIC.CTRL |= (mask); sei()

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))

uint8_t do_tick = 0;
uint32_t ticks = 0;

uint16_t limit(int16_t val, int16_t min, int16_t max){
	return min(max(val, min), max);
}

uint16_t positive(int16_t l){
	return l > 0 ? l : 0;
}

uint16_t negative(int16_t l){
	return l < 0 ? l : 0;
}

uint8_t walk = 4;
uint8_t turn = 4;


int8_t* walkpattern = NULL;

/*
* Oh god why!?
*/
void stepr(int8_t h, int8_t a){
	maestro_position(5*3 + 0, limit(1500 - a*200, 1250, 1750)*4);
	maestro_position(5*3 + 1, limit(1650 - h*100, 1300, 2000)*4);
	maestro_position(5*3 + 2, limit(1100 + negative(h*100), 1000, 1900)*4);

	maestro_position(2*3 + 0, limit(1500 + a*200, 1250, 1750)*4);
	maestro_position(2*3 + 1, limit(1350 + h*100, 1000, 1700)*4);
	maestro_position(2*3 + 2, limit(1900 + positive(h*100), 1100, 2000)*4);

	maestro_position(1*3 + 0, limit(1500 - a*200, 1250, 1750)*4);
	maestro_position(1*3 + 1, limit(1650 - h*100, 1300, 2000)*4);
	maestro_position(1*3 + 2, limit(1100 + negative(h*100), 1000, 1900)*4);
}

void stepl(int8_t h, int8_t a){
	maestro_position(4*3 + 0, limit(1500 + a*200, 1250, 1750)*4);
	maestro_position(4*3 + 1, limit(1350 + h*100, 1000, 1700)*4);
	maestro_position(4*3 + 2, limit(1900 - negative(h*100), 1100, 2000)*4);

	maestro_position(3*3 + 0, limit(1500 - a*200, 1250, 1750)*4);
	maestro_position(3*3 + 1, limit(1650 - h*100, 1300, 2000)*4);
	maestro_position(3*3 + 2, limit(1100 - positive(h*100), 1000, 1900)*4);

	maestro_position(0*3 + 0, limit(1500 + a*200, 1250, 1750)*4);
	maestro_position(0*3 + 1, limit(1350 + h*100, 1000, 1700)*4);
	maestro_position(0*3 + 2, limit(1900 - negative(h*100), 1100, 2000)*4);

}

void steppair(uint8_t i, int8_t h, int8_t a){
	maestro_position(i*6 + 0, limit(1500 + a*200, 1250, 1750)*4);
	maestro_position(i*6 + 1, limit(1350 + ((h!=0) ? h*100 : -100), 1000, 1700)*4);
	maestro_position(i*6 + 2, limit(1900, 1100, 1900)*4);

	maestro_position(i*6 + 3, limit(1500 - a*200, 1250, 1750)*4);
	maestro_position(i*6 + 4, limit(1650 + ((h!=0) ? h*100 : 100), 1300, 2000)*4);
	maestro_position(i*6 + 5, limit(1100, 1100, 1900)*4);
}



/*
* How the above should be done
*/
void calc_ik(const uint8_t leg, const float x, const float y, const float z){
	const float L1 = 50;
	const float L2 = 70;
	const float L3 = 110;
	float d_sq = x*x + y*y;
	float d = sqrt(d_sq) - L1;
	float s1 = (d*d + L2*L2 - L3*L3)/(2*d*L2);
	float s2 = (L2*L2 + L3*L3 - d*d)/(2*L2*L3);

	float S0 = (180.0f/M_PI)*(atan2(y, x));
	float S1 = (180.0f/M_PI)*(acos(s1) + atan2(z, d));
	float S2 = (180.0f/M_PI)*(acos(s2));

	#define S_FACTOR (500.0f/60.0f)

	if (leg%2)
	{
		maestro_position(leg*3 + 0, limit(1500 +  0*S_FACTOR - S0*S_FACTOR, 1250, 1750)*4);
		maestro_position(leg*3 + 1, limit(1500 + 90*S_FACTOR - S1*S_FACTOR, 1250, 2150)*4);
		maestro_position(leg*3 + 2, limit(1500 - 90*S_FACTOR + S2*S_FACTOR, 850, 2150)*4);
	}else{
		maestro_position(leg*3 + 0, limit(1500 +  0*S_FACTOR + S0*S_FACTOR, 1250, 1750)*4);
		maestro_position(leg*3 + 1, limit(1500 - 90*S_FACTOR + S1*S_FACTOR, 800, 1750)*4);
		maestro_position(leg*3 + 2, limit(1500 + 90*S_FACTOR - S2*S_FACTOR, 850, 2150)*4);
	}
}

#define STEP_HEIGHT 90

struct
{
	int16_t x0;
	int16_t y0;
}legs[] = {
	{-70, -160},
	{ 70, -160},
	{-70,    0},
	{ 70,    0},
	{-70,  160},
	{ 70,  160}
};

int16_t offset_y = -90;
int16_t offset_x = 130;

void step_partial(uint8_t i, float t, int16_t x, int16_t y, bool half_step){
	if (t < -M_PI || t > M_PI)
	{
		return;
	}

	int16_t nz = (int16_t)(STEP_HEIGHT*fmax(sin(t), 0) + offset_y);

	if (half_step && t >= 0)
	{
		//nz = STEP_HEIGHT*|sin(2x)| + offset_y
		nz = (int16_t)(STEP_HEIGHT*fabs(sin(2*t))/2 + offset_y);
	}

	float c = (2*fabs(t)/M_PI - 1);
	int16_t nx = (int16_t)(x*c) + offset_x;
	int16_t ny = (int16_t)(y*c);

	calc_ik(i, nx, ny, nz);
}

void turn_partial(uint8_t i, float t, float angle, bool half_step){
	if (t < -M_PI || t > M_PI)
	{
		return;
	}

	int16_t nz = (int16_t)(STEP_HEIGHT*fmax(sin(t), 0) + offset_y);

	if (half_step && t >= 0)
	{
		//nz = STEP_HEIGHT*|sin(2x)| + offset_y
		nz = (int16_t)(STEP_HEIGHT*fabs(sin(2*t))/2 + offset_y);
	}

	//calc_ik(5, 272*cos(((36+d)/180)*M_PI)-70, 272*sin(((36+d)/180)*M_PI)-160, 188.68*sin(((-32+a)/180)*M_PI));
	// calc_ik(5, 272*cos(((36+d)/180)*M_PI)-70, 272*sin(((36+d)/180)*M_PI)-160, 188.68*sin(((-32+a)/180)*M_PI));
	// calc_ik(3, 220*cos((d/180)*M_PI)-70, 220*sin((d/180)*M_PI), -100);
	// calc_ik(1, 272*cos(((-36+d)/180)*M_PI)-70, 272*sin(((-36+d)/180)*M_PI)+160, 188.68*sin(((-32-a)/180)*M_PI));
	// d=-d;
	// calc_ik(4, 272*cos(((36+d)/180)*M_PI)-70, 272*sin(((36+d)/180)*M_PI)-160, 188.68*sin(((-32+a)/180)*M_PI));
	// calc_ik(2, 220*cos((d/180)*M_PI)-70, 220*sin((d/180)*M_PI), -100);
	// calc_ik(0, 272*cos(((-36+d)/180)*M_PI)-70, 272*sin(((-36+d)/180)*M_PI)+160, 188.68*sin(((-32-a)/180)*M_PI));

	float d = 0;
	float h = 0;
	int16_t y00 = 0;

	switch(i){
		case 5:
			h = 256;
			d = 39.65f;
			y00 = -160;
		break;
		case 3:
			h = 200;
			d = 0;
			y00 = 0;
		break;
		case 1:
			h = 256;
			d = -39.65f;
			y00 = 160;
		break;
		case 4:
			h = 256;
			d = 39.65f;
			y00 = -160;
			angle = -angle;
		break;
		case 2:
			h = 200;
			d = 0;
			y00 = 0;
			angle = -angle;
		break;
		case 0:
			h = 256;
			d = -39.65f;
			y00 = 160;
			angle = -angle;
		break;
		default:
		break;
	}

	d=d*M_PI/180;

	float c = (2*fabs(t)/M_PI - 1);
	
	int16_t nx = (int16_t)(h*cos(d + angle*c) - 70);
	int16_t ny = (int16_t)(h*sin(d + angle*c) + y00);

	calc_ik(i, nx, ny, nz);
}

/*
* TODO this should be sent back to the hell it came from.
*/

volatile char usart_buffer[128];
volatile uint8_t index = 0;
volatile uint8_t buffer_complete = 0;

ISR(USARTE1_RXC_vect){
	char rx = USARTE1.DATA;
	if (index <= 127)
	{
		PORTD.OUTTGL = LED_READING;
		if (rx == '\r')
		{
			usart_buffer[index] = '\0';
			buffer_complete = 1;
		}else{
			usart_buffer[index] = rx;
		}
		index++;
	}
}

void usart_send(char* s){
	while(*s){
		while( !(USARTE1.STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
    	USARTE1.DATA = *s;  
    	s++;  
	}
}

uint8_t controller = 0;
int8_t progress = 0;
uint8_t home = 0;
uint8_t floodlights = 0;
ps2_controller controller_struct;

enum step_dir
{
	STEP_NONE,
	STEP_FORW,
	STEP_REVS,
	STEP_LEFT,
	STEP_RGHT,
	STEP_CCW,
	STEP_CW
};

/*
* TODO replace with IK and direction vector instead of step_dir
*/
void new_step(enum step_dir dir){
	if(walk >= 4 && turn >= 4){
		switch(dir){
			case STEP_FORW:
				if(walkpattern == walk_forw){
					walkpattern = walk_forw2;
				}else{
					walkpattern = walk_forw;
				}
				walk=0;
			break;
			case STEP_REVS:
				if(walkpattern == walk_revs){
					walkpattern = walk_revs2;
				}else{
					walkpattern = walk_revs;
				}
				walk=0;
			break;
			case STEP_CCW:
				walkpattern = walk_ccw;
				turn=0;
			break;
			case STEP_CW:
				walkpattern = walk_cw;
				turn=0;
			break;
			case STEP_NONE:
			default:
				/*Nothing*/
			break;
		}
	}
}

void disable_leg(uint8_t leg){
	for (int i = 0; i < 3; ++i)
	{
		maestro_position(leg*3+i, 0);
	}
}


/*
* TODO home position should be IK's default state instead of hard coded
*/
bool at_home_position = true;
void home_position(bool home){
	if (home)
	{
		stepr(2,0);
		stepl(2,0);	
		_delay_ms(500);
		for (int i = 0; i < 6; ++i)
		{
			disable_leg(i);
		}
	}else{
		stepr(-1,0);
		stepl(-1,0);	
	}
	at_home_position = home;
}


void look(float yaw, float pitch){
	maestro_position(18, limit(1500 + (pitch/M_PI)*1000, 1250, 1750)*4);
	maestro_position(20, limit(1500 -   (yaw/M_PI)*1000, 1250, 1750)*4);
	maestro_position(19, limit(1500 - (pitch/M_PI)*1000, 1250, 1750)*4);
	maestro_position(21, limit(1500 -   (yaw/M_PI)*1000, 1250, 1750)*4);
}


void setup(void)
{
	/*---------------Setup I/O---------------*/
	PORTD.DIR = 0xFF;
	PORTC.DIRSET = (1 << 5)|(1 << 6)|(1 << 7);

	PORTH.DIR = 0x00;
	PORTH.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTH.PIN1CTRL = PORT_OPC_PULLUP_gc;
	PORTH.PIN2CTRL = PORT_OPC_PULLUP_gc;
	PORTH.PIN3CTRL = PORT_OPC_PULLUP_gc;
	PORTH.PIN4CTRL = PORT_OPC_PULLUP_gc;
	PORTH.PIN5CTRL = PORT_OPC_PULLUP_gc;
	PORTH.PIN6CTRL = PORT_OPC_PULLUP_gc;
	PORTH.PIN7CTRL = PORT_OPC_PULLUP_gc;
	/*---------------Setup timer---------------*/
	// 4Hz
	TCC0.PER = TCC0_FRQ/(15);
	TCC0.CNT = 0;
	TCC0.INTCTRLA = TC_OVFINTLVL_MED_gc;
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;
	// 100Hz
	/*---------------Setup raspi usart---------------*/
	PORTE.DIRSET = PIN7_bm;
	USARTE1.BAUDCTRLB = 0xD0;
	USARTE1.BAUDCTRLA = 131;
	USARTE1.CTRLA = USART_RXCINTLVL_HI_gc;
	USARTE1.CTRLC = USART_CHSIZE_8BIT_gc;
	USARTE1.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
	/*---------------Setup display---------------*/
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_puts("GPA-ROBOTICS.COM\n");
	/*---------------Setup maestro---------------*/
	maestro_init();
	maestro_position(22, 1500);
	for (int i = 0; i < 18; ++i)
	{
		maestro_position(i, 0);
	}
	/*---------------Setup ps2 controller---------------*/
	ps2_init_analog_mode();

	home_position(true);
	ENABLE_INTERRUP(PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm);
	_delay_ms(10);
}

uint8_t remote = 0;
bool fast = false;
bool offset = false;

bool new_walk = false;
bool g_half_step=false;
int16_t translate_x = 0;
int16_t translate_y = 0;
int16_t translate_angle = 0;

int16_t t1 = 0;
int16_t t2 = 0;

void aitick(){
	if(!(PORTE.IN & PIN5_bm)){
		if(controller==0){
			
			lcd_gotoxy(0,1);
			lcd_puts("INITIALIZING... ");
			uint8_t controllerstat = ps2_init_analog_mode();
			lcd_gotoxy(0,1);
			if(controllerstat==PS2_CONTROLLER_MODE_ANALOG){
				controller = 1;
			}else{
				lcd_puts("ERR:PSMODE");
			}
		}
	}else{
		controller = 0;
	}

	PORTD.OUTTGL = LED_UPDATE;

	

	if (new_walk || g_half_step)
	{
		t1+=5;
		t2+=5;
		if (t1 > 100)
		{
			t1 = -100;
		}
		if (t2 > 100)
		{
			t2 = -100;
		}
		if (translate_angle != 0)
		{
			turn_partial(0, M_PI*t1/100, translate_angle*M_PI/180, g_half_step);
			turn_partial(1, M_PI*t2/100, translate_angle*M_PI/180, g_half_step);

			turn_partial(2, M_PI*t2/100, translate_angle*M_PI/180, g_half_step);
			turn_partial(3, M_PI*t1/100, translate_angle*M_PI/180, g_half_step);

			turn_partial(4, M_PI*t1/100, translate_angle*M_PI/180, g_half_step);
			turn_partial(5, M_PI*t2/100, translate_angle*M_PI/180, g_half_step);
		}else{
			step_partial(0, M_PI*t1/100,  translate_x, translate_y, g_half_step);
			step_partial(1, M_PI*t2/100, -translate_x, translate_y, g_half_step);

			step_partial(2, M_PI*t2/100,  translate_x, translate_y, g_half_step);
			step_partial(3, M_PI*t1/100, -translate_x, translate_y, g_half_step);

			step_partial(4, M_PI*t1/100,  translate_x, translate_y, g_half_step);
			step_partial(5, M_PI*t2/100, -translate_x, translate_y, g_half_step);
		}
		
		if (t2 == 0 || t1 == 0)
		{
			if((controller_struct.buttons & (1 << 4))
				&& (controller_struct.buttons & (1 << 5))
				&& (controller_struct.buttons & (1 << 6))
				&& (controller_struct.buttons & (1 << 7)))
			{
				g_half_step = true;
				new_walk = false;
			}else{
				g_half_step = false;
			}
		}else if (t2 == 50 || t1 == 50)
		{
			g_half_step = false;
		}
	}


}


void reset_walk(){
	g_half_step = true;
	new_walk = true;
	t1 = 50;
	t2 = -50;
}


void update(){
	if (do_tick)
	{
		do_tick = 0;
		aitick();
	}


	if (controller)
	{
		cli();
		ps2_read_controller(&controller_struct);
		sei();
		if(controller_struct.buttons_change  & (1 << 0) && controller_struct.buttons & (1 << 0)){
			remote = !remote;
			if (remote)
			{
				lcd_gotoxy(0,1);
				lcd_puts("   --REMOTE--   ");
			}else{
				lcd_gotoxy(0,1);
				lcd_puts("   ---AUTO---   ");
				home_position(true);
			}
		}
		//maestro_position(4*3 + 0, 1500*4+(controller_struct.lx-127)*2);
		//calc_ik(4, 130+(controller_struct.ly-127)/1.5f, 0+(controller_struct.lx-127)/1.5f, offset_y);

		if (remote)
		{
			if(controller_struct.buttons_change  & (1 << 3) && !(controller_struct.buttons & (1 << 3))){
				home_position(!at_home_position);
			}
			if(controller_struct.buttons & (1 << 11) && !(controller_struct.buttons & (1 << 11))){
				if (!new_walk && !g_half_step)
				{
					reset_walk();
					translate_y = 1;
					translate_x = 0;
					translate_angle = 0;
				}
			}
			if(controller_struct.buttons_change  & (1 << 4) && !(controller_struct.buttons & (1 << 4))){
				if (!new_walk && !g_half_step)
				{
					reset_walk();
					translate_y = 75;
					translate_x = 0;
					translate_angle = 0;
				}
			}
			if(controller_struct.buttons_change  & (1 << 6) && !(controller_struct.buttons & (1 << 6))){
				if (!new_walk && !g_half_step)
				{
					reset_walk();
					translate_y = -75;
					translate_x = 0;
					translate_angle = 0;
				}
			}
			if(controller_struct.buttons_change  & (1 << 5) && !(controller_struct.buttons & (1 << 5))){
				if (!new_walk && !g_half_step)
				{
					reset_walk();
					translate_y = 0;
					translate_x = -35;
					if (controller_struct.buttons & (1 << 10))
						translate_angle = -15;
					else
						translate_angle = 0;
				}
			}
			if(controller_struct.buttons_change  & (1 << 7) && !(controller_struct.buttons & (1 << 7))){
				if (!new_walk && !g_half_step)
				{
					reset_walk();
					translate_y = 0;
					translate_x = 35;
					if (controller_struct.buttons  & (1 << 10))
						translate_angle = 15;
					else
						translate_angle = 0;
				}
			}
			maestro_position(18, 1500*4 + (controller_struct.ry-127)*10);
			maestro_position(20, 1500*4 - (controller_struct.ry-127)*10);
			maestro_position(19, 1425*4 - (controller_struct.rx-127)*10);
			maestro_position(21, 1600*4 - (controller_struct.rx-127)*10);
			if(controller_struct.buttons_change  & (1 << 8)){
				TCC0.CTRLA = TC_CLKSEL_OFF_gc;
				//This is bad and you should feel bad...
				if(!(controller_struct.buttons & (1 << 8))){
					TCC0.PER = TCC0_FRQ/(5*1024);
				}else{
					TCC0.PER = TCC0_FRQ/(3*1024);
				}
				TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;
			}
			if(controller_struct.buttons_change  & (1 << 15) && controller_struct.buttons & (1 << 15)){
				floodlights = !floodlights;
			}
			if(controller_struct.buttons_change  & (1 << 1) && controller_struct.buttons & (1 << 1)){
				offset = !offset;
			}
			if(controller_struct.buttons & (1 << 13) && floodlights){//RED
				PORTC.OUTCLR = PIN5_bm;
			}else{
				PORTC.OUTSET = PIN5_bm;
			}
			if(controller_struct.buttons & (1 << 12) && floodlights){//GREEN
				PORTC.OUTCLR = PIN6_bm;
			}else{
				PORTC.OUTSET = PIN6_bm;
			}
			if(controller_struct.buttons & (1 << 14) && floodlights){//BLUE
				PORTC.OUTCLR = PIN7_bm;
			}else{
				PORTC.OUTSET = PIN7_bm;
			}
			if(!new_walk && !g_half_step && !at_home_position){
				if (offset)
				{
					for (int i = 0; i < 6; ++i)
					{
						if (i%2)
						{
							calc_ik(i, 150-(controller_struct.lx-127)/4, -(controller_struct.ly-127)/2, -100);
						}else{
							calc_ik(i, 150+(controller_struct.lx-127)/4, -(controller_struct.ly-127)/2, -100);
						}
					}
				}else{
					float d=(controller_struct.lx-127)/8;
					float a=(controller_struct.ly-127)/8;
					calc_ik(5, 272*cos(((36+d)/180)*M_PI)-70, 272*sin(((36+d)/180)*M_PI)-160, 188.68*sin(((-32+a)/180)*M_PI));
					calc_ik(3, 220*cos((d/180)*M_PI)-70, 220*sin((d/180)*M_PI), -100);
					calc_ik(1, 272*cos(((-36+d)/180)*M_PI)-70, 272*sin(((-36+d)/180)*M_PI)+160, 188.68*sin(((-32-a)/180)*M_PI));
					d=-d;
					calc_ik(4, 272*cos(((36+d)/180)*M_PI)-70, 272*sin(((36+d)/180)*M_PI)-160, 188.68*sin(((-32+a)/180)*M_PI));
					calc_ik(2, 220*cos((d/180)*M_PI)-70, 220*sin((d/180)*M_PI), -100);
					calc_ik(0, 272*cos(((-36+d)/180)*M_PI)-70, 272*sin(((-36+d)/180)*M_PI)+160, 188.68*sin(((-32-a)/180)*M_PI));
				}
				//calc_ik(5, 150 + (controller_struct.lx-127)/5, (controller_struct.ly-127)/5, 0);
				
				
			}
		}else{
			for (int i = 0; i < 18; ++i)
			{
				maestro_position(i, 0);
			}
		}
		
	}


	if(buffer_complete)
	{
		char returnbuffer[16];
		char* arg = strtok (&usart_buffer," ");
		if(strcmp(&usart_buffer,"lookat") == 0){
			if (!remote)
			{
				for(int argc = 0; argc < 3 && arg != NULL; argc++)
				{
					if(argc == 2){
						float yaw = atof(arg);
						maestro_position(18, limit(1500 + (yaw/M_PI)*1000, 1150, 1850)*4);
						maestro_position(20, limit(1500 - (yaw/M_PI)*1000, 1150, 1850)*4);
						maestro_position(22, 1500);
					}else if(argc == 1){
						float pitch = atof(arg);
						maestro_position(19, limit(1425 - (pitch/M_PI)*1000, 1250, 1750)*4);
						maestro_position(21, limit(1600 - (pitch/M_PI)*1000, 1250, 1750)*4);
					}
					arg = strtok (NULL, " ");
				}
			}else{
				usart_send("lookat not possible!\n\r");
			}
			
		}else if(strcmp(&usart_buffer,"light") == 0){
			if (!remote)
			{
				for(int argc = 0; argc < 4 && arg != NULL; argc++)
				{
					if(strcmp(arg,"on") == 0){
						PORTC.OUTSET = PIN5_bm | PIN6_bm | PIN7_bm;
					}else if(strcmp(arg,"off") == 0){
						PORTC.OUTCLR = PIN5_bm | PIN6_bm | PIN7_bm;
					}else if(argc == 1){
						float r = atof(arg);
						if(r > 0){
							PORTC.OUTSET = PIN5_bm;
						}else{
							PORTC.OUTCLR = PIN5_bm;
						}
					}else if(argc == 2){
						float g = atof(arg);
						if(g > 0){
							PORTC.OUTSET = PIN6_bm;
						}else{
							PORTC.OUTCLR = PIN6_bm;
						}
					}else if(argc == 3){
						float b = atof(arg);
						if(b > 0){
							PORTC.OUTSET = PIN7_bm;
						}else{
							PORTC.OUTCLR = PIN7_bm;
						}
					}
					arg = strtok (NULL, " ");
				}
			}else{
				usart_send("light not possible!\n\r");
			}
			
		}else if(strcmp(&usart_buffer,"version") == 0){
			usart_send("ASH version 0.1.1 (c) GPA-ROBOTICS 2016!\n\r");
		}else if(strcmp(&usart_buffer,"feet?") == 0){
			itoa(PORTH.IN, &returnbuffer, 16);
			usart_send(&returnbuffer);
			usart_send("\n\r");
		}else if(strcmp(&usart_buffer,"remote?") == 0){
			if (remote)
			{
				usart_send("true\n\r");
			}else{
				usart_send("false\n\r");
			}
		}else if(strcmp(&usart_buffer,"home?") == 0){
			if (at_home_position)
			{
				usart_send("true\n\r");
			}else{
				usart_send("false\n\r");
			}
			
		}else if(strcmp(&usart_buffer,"step") == 0){
			if (!remote)
			{
				arg = strtok (NULL, " ");
				if(strcmp(arg,"forw") == 0){

				}else if(strcmp(arg,"back") == 0){

				}else if(strcmp(arg,"left") == 0){

				}else if(strcmp(arg,"right") == 0){

				}else{
					usart_send("Invalid direction!\n\r");
				}
			}else{
				usart_send("Step not possible!\n\r");
			}
			
		}else{
			usart_send("Unknown command!\n\r");
		}
		
		index = 0;
		buffer_complete = 0;
	}
}


/*
* Maestro usart implementation
*/
void maestro_rst(void){
	//Configure USART
	MAESTRO_SERPORT.BAUDCTRLB = MAESTRO_BAUDSCL;
	MAESTRO_SERPORT.BAUDCTRLA = MAESTRO_BAUDSEL;
	MAESTRO_SERPORT.CTRLA = 0x00;
	MAESTRO_SERPORT.CTRLB = USART_TXEN_bm;
	MAESTRO_SERPORT.CTRLC = USART_CHSIZE_8BIT_gc;
	PORTC.OUTSET = PIN3_bm;
	PORTC.DIRSET = PIN3_bm;

	PORTC.DIRSET = PIN4_bm; //RST pin as output
	//Reset
	PORTC.OUTCLR = PIN4_bm;
	_delay_ms(10);
	PORTC.OUTSET = PIN4_bm;
	_delay_ms(10);

}

char maestro_rxd(void){
	return 0x00;
}

void maestro_txd(char c){
	while( !(MAESTRO_SERPORT.STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
    MAESTRO_SERPORT.DATA = c;    
}

/*
* Main, clock & ISRs.
*/
void setClockTo32MHz() {
    CCP = CCP_IOREG_gc;              // disable register security for oscillator update
    OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
    while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
    CCP = CCP_IOREG_gc;              // disable register security for clock update
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
}

ISR(TCC0_OVF_vect){
	do_tick=1;
}

int main(void){
	setClockTo32MHz();
	setup();
	while(1){
		update();
		PORTD.OUTTGL = LED_PROCESSING;
	}
	return 1;
}