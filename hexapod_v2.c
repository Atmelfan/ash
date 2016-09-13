// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>
// #include <string.h>
// #include <stdlib.h> 
// #include <math.h>
// #include <stdbool.h>




// /*System clock interrupt*/
// uint32_t milliseconds_10 = 0;
// ISR(TCC0_OVF_vect){
// 	milliseconds++;
// }

// uint32_t clock_ms(){
// 	return milliseconds_10*10;
// }

// uint32_t clock_s(){
// 	return milliseconds_10/100;
// }

// uint32_t clock(){
// 	return clock_ms();
// }


// void set_clock_to_32MHz() {
//     CCP = CCP_IOREG_gc;              // disable register security for oscillator update
//     OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
//     while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
//     CCP = CCP_IOREG_gc;              // disable register security for clock update
//     CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
// }

// void setup(){
// 	set_clock_to_32MHz();

// }


// int main(void){
// 	setup();
// 	while(1){
// 		update();
// 		PORTD.OUTTGL = LED_PROCESSING;
// 	}
// 	return 1;
// }









