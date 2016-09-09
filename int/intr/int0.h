/******************** DESCRIPTION ********************
	This library is for External interrupt on INT0 pin wich 
	is PIN-16 (PD2). Its can be configured so that it can
	generate interrupt request when either of the following 
	event happens on INT0 pin

	EVENTS: 
		1. FALLING EDGE 	=> Transistor SW or SW between GND and INT0
		2. RISING EDGE 		=> INT0 pulled down with 47k, SW between VCC and INT0
		3. LOW LEVEL 		=> Transistor SW or SW between GND and INT0
		4. TOGGLE LEVEL 	=> Transistor SW or SW between GND and INT0
	
	USER FUNCTIONS:
		1. int0Init() 		=> Initializes the registers needed to setup INT0 for external interrupt.
		2. int0Disable		=> Resets the registers to pristine condition
		3. int0_Callback()	=> Callback function when even happens.

	NOTE: 
		The function "int0_Callback()" has to be defined
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
#define LOW_LEVEL 3
#define TOGGLE_LEVEL 4
/*--------------------------------------------------*/


/***************** USER CONFIGURABLE *****************/
// Options are FALLING_EDGE, RISING_EDGE, LOW_LEVEL, TOGGLE_LEVEL
#define TRIG_YTPE RISING_EDGE	
/*---------------------------------------------------*/





/******************* USER FUNCTIONS *******************/
void int0Init() {
	cli(); // clear global interrupt
	DDRD &= ~(1<<PD2);  //make it input
	
	#if TRIG_YTPE == FALLING_EDGE
		PORTD |=(1<<PD2); // connect internal pull up
   		MCUCR |= (2<<0); // falling edge trigger on INT0 (PD2); ISC01:0=2
	#elif TRIG_YTPE == RISING_EDGE
   		PORTD &= ~(1<<PD2); // remove internal pullups, Use external Pull down
   		MCUCR |= (3<<0); // rising edge trigger on INT0 (PD2); ISC01:0=3
	#elif TRIG_YTPE == LOW_LEVEL
   		PORTD |=(1<<PD2); // connect internal pull up
   		// do nothing. This is default;	ISC01:0=0
	#elif TRIG_YTPE == TOGGLE_LEVEL
   		PORTD |=(1<<PD2); // connect internal pull up
   		MCUCR |= (1<<0);	//Toggle level trigger on INt0 (PD2); ISC01:0=1 
	#endif

   	GICR |= (1<<INT0); // enable INT0 interrupt
   	sei();   // set global interrupt
}

void int0Disable() {
	cli();
	PORTD &= ~(1<<PD2); // disconnect internal pullups
	MCUCR &= ~(1<<ISC01); // clear MCUCSR
	MCUCR &= ~(1<<ISC00); // clear MCUCSR
	GICR &= ~(1<<INT0); // clear interrupt mask
	sei();
}

// macro


/*------------------------------------------------------*/



/****************** FUNCTION PROTOTYPES ******************/
void int0_Callback(void);
/*-------------------------------------------------------*/


ISR (INT0_vect) {
	int0_Callback();
	GIFR |= (1<<INTF0); // clear the flag by writing 1 to it. This clear any other 
}						// interrupt request might had come during ISR execution



/************************ UNDEFINE ************************/
// #undef FALLING_EDGE
// #undef RISING_EDGE
// #undef LOW_LEVEL
// #undef TOGGLE_LEVEL
/*--------------------------------------------------------*/



/********************* SAMPLE PROGRAM *********************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"
#include <inttypes.h>
#include "mega16/int/intr/int0.h"

volatile uint8_t data;

int main() {

   int0Init();
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

void int0_Callback() {
   _delay_ms(100);
   data++;
}

----------------------------------------------------------*/
