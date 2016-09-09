/******************** DESCRIPTION ********************
	This library is for External interrupt on INT2 pin wich 
	is PIN-3 (PB2). Its can be configured so that it can
	generate interrupt request when either of the following 
	event happens on INT2 pin

	EVENTS: 
		1. FALLING EDGE 	=> Transistor SW or SW between GND and INT2
		2. RISING EDGE 		=> INT2 pulled down with 47k, SW between VCC and INT2
	
	USER FUNCTIONS:
		1. int2Init() 		=> Initializes the registers needed to setup INT2 for external interrupt.
		2. int2Disable		=> Resets the registers to pristine condition
		3. int2_Callback()	=> Callback function when even happens.

	NOTE: 
		The function "int2_Callback()" has to be defined
		by the programmer,  Otherwise the library WILL NOT COMPILE

	
----------------------------------------------------*/


/*************** INCLUDE DEPENDENCIES ***************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
/*--------------------------------------------------*/

		

/********************* INTERNAL *********************/
#define FALLING_EDGE 1
#define RISING_EDGE 2
/*--------------------------------------------------*/


/***************** USER CONFIGURABLE *****************/
// Options are FALLING_EDGE, RISING_EDGE
#define TRIG_YTPE FALLING_EDGE	
/*---------------------------------------------------*/





/******************* USER FUNCTIONS *******************/
void int2Init() {
	cli(); // clear global interrupt
	DDRB &= ~(1<<PB2);  //make it input
	
	#if TRIG_YTPE == FALLING_EDGE
		PORTB |=(1<<PB2); // connect internal pull up
   		MCUCSR &= ~(1<<ISC2); // falling edge trigger on INT2 (PB2); ISC2=0
	#elif TRIG_YTPE == RISING_EDGE
   		PORTB &= ~(1<<PB2); // remove internal pullups, Use external Pull down
   		MCUCSR |= (1<<ISC2); // rising edge trigger on INT2 (PB2); ISC2=1
	#endif

   	GICR |= (1<<INT2); // enable INT2 interrupt
   	sei();   // set global interrupt
}


void int2Disable() {
	cli();
	PORTB &= ~(1<<PB2); // disconnect internal pullups
	MCUCSR &= ~(1<<ISC2); // clear MCUCSR
	GICR &= ~(1<<INT2); // clear interrupt mask
	sei();
}
/*------------------------------------------------------*/



/****************** FUNCTION PROTOTYPES ******************/
void int2_Callback(void);
/*-------------------------------------------------------*/


ISR (INT2_vect) {
	int2_Callback();
	GIFR |= (1<<INTF2); // clear the flag by writing 1 to it. This clear any other 
}						// interrupt request might had come during ISR execution



/************************ UNDEFINE ************************/
#undef FALLING_EDGE
#undef RISING_EDGE
/*--------------------------------------------------------*/





/********************* SAMPLE PROGRAM *********************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"
#include <inttypes.h>
#include "int2.h"

volatile uint8_t data;

int main() {

   int2Init();
   DDRA |= (1<<PA7);
   LCDInit(LS_NONE);
   LCDClear();
   LCDWriteString("READY");
   _delay_ms(1000);
   LCDClear();
   
   while(1) {
      LCDWriteIntXY(0,0,data,3);
      PORTA ^= (1 << PA7); // keep toggling an led 
      _delay_ms(200);
   }
}

void int2_Callback() {
   _delay_ms(100);
   data++;
}

----------------------------------------------------------*/