/******************** DESCRIPTION ********************
	This library is for External interrupt on INT1 pin wich 
	is PIN-17 (PD3). Its can be configured so that it can
	generate interrupt request when either of the following 
	event happens on INT1 pin

	EVENTS: 
		1. FALLING EDGE 	=> Transistor SW or SW between GND and INT1
		2. RISING EDGE 		=> INT1 pulled down with 47k, SW between VCC and INT1
		3. LOW LEVEL 		=> Transistor SW or SW between GND and INT1
		4. TOGGLE LEVEL 	=> Transistor SW or SW between GND and INT1
	
	USER FUNCTIONS:
		1. int1Init() 		=> Initializes the registers needed to setup INT1 for external interrupt.
		2. int1Disable		=> Resets the registers to pristine condition
		3. int1_Callback()	=> Callback function when even happens.

	NOTE: 
		The function "int1_Callback()" has to be defined
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
#define TRIG_YTPE FALLING_EDGE	
/*---------------------------------------------------*/





/******************* USER FUNCTIONS *******************/
void int1Init() {
	cli(); // clear global interrupt
	DDRD &= ~(1<<PD3);  //make it input
	
	#if TRIG_YTPE == FALLING_EDGE
		PORTD |=(1<<PD3); // connect internal pull up
   		MCUCR |= (1<<ISC11); // falling edge trigger on INT1 (PD3); ISC11:0=2
	#elif TRIG_YTPE == RISING_EDGE
   		PORTD &= ~(1<<PD3); // remove internal pullups, Use external Pull down
   		MCUCR |= (1<<ISC11); // rising edge trigger on INT1 (PD3); ISC11:0=3
   		MCUCR |= (1<<ISC10);
	#elif TRIG_YTPE == LOW_LEVEL
   		PORTD |=(1<<PD3); // connect internal pull up
   		// do nothing. This is default;	ISC11:0=0
	#elif TRIG_YTPE == TOGGLE_LEVEL
   		PORTD |=(1<<PD3); // connect internal pull up
   		MCUCR |= (1<<ISC10);	//Toggle level trigger on INt1 (PD3); ISC11:0=1 
	#endif

   	GICR |= (1<<INT1); // enable INT1 interrupt
   	sei();   // set global interrupt
}

void int1Disable() {
	cli();
	PORTD &= ~(1<<PD3); // disconnect internal pullups
	MCUCR &= ~(1<<ISC11); // clear MCUCSR
	MCUCR &= ~(1<<ISC10); // clear MCUCSR
	GICR &= ~(1<<INT1); // clear interrupt mask
	sei();
}
/*------------------------------------------------------*/



/****************** FUNCTION PROTOTYPES ******************/
void int1_Callback(void);
/*-------------------------------------------------------*/


ISR (INT1_vect) {
	int1_Callback();
	GIFR |= (1<<INTF1); // clear the flag by writing 1 to it. This clear any other 
}						// interrupt request might had come during ISR execution



/************************ UNDEFINE ************************/
#undef FALLING_EDGE
#undef RISING_EDGE
#undef LOW_LEVEL
#undef TOGGLE_LEVEL
/*--------------------------------------------------------*/



/********************* SAMPLE PROGRAM *********************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"
#include <inttypes.h>
#include "mega16/int/intr/int1.h"

volatile uint8_t data;

int main() {

   int1Init();
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

void int1_Callback() {
   _delay_ms(100);
   data++;
}

----------------------------------------------------------*/