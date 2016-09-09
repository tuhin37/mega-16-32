/************************ DESCRIPTION **********************
This library uses 16 Bit Timer1 to measure frequency of an 
input signal. This library starts counting at a rising edge and 
counts upto falling edge with a known speed, and thats how it
calculated frequency from half peroid.

The user function is generic and can be used with any GPIO pin

NOTE: 
	Programmer has to specify the frequency range is to be 
	expected at input pin. The following ranges are available
		2 Hz - 62 Khz
		61 Hz - 4 Mhz
		122 Hz - 8 Mhz  
-----------------------------------------------------------*/



/***************************************** DEPENDENCY *****************************************/
#ifndef GPIO
	#include "../../io/gpio.h"
#endif


/************************* DEFINITION *************************/
#define HZ122_MHZ8 1 	// 122 Hz to 8 Mhz
#define HZ61_MHZ4 2		// 61 Hz to 4 Mhz
#define HZ2_KHZ62 3		// 2 Hz to 62 Khz
/*------------------------------------------------------------*/

/********************** USER CONFIGURABLE **********************/
#define FREQ_RANGE HZ122_MHZ8 // options are: HZ122_MHZ8; HZ61_MHZ4; HZ2_KHZ62

/*-------------------------------------------------------------*/



/************************ USER FUNCTIONS ***********************/

uint32_t getT1Freq(volatile uint8_t pos) {
	TCCR1B &= ~(7); // clear clock
	//wait as long as its high
	while(getInput(pos));
	// wait as longs as its low
	while(!getInput(pos));
	// falling edge detected
	TCNT1 = 0;
	#if FREQ_RANGE == HZ122_MHZ8
		TCCR1B |= (1<<CS10); // start counter at 16Mhz 
	#elif FREQ_RANGE == HZ61_MHZ4
		TCCR1B |= (1<<CS11); // start counter at 8Mhz
	#elif FREQ_RANGE == HZ2_KHZ62
		TCCR1B | =(3<<0); // start counter at 250 Khz
	#endif
	while(!getInput(pos)); // wait and count as long as its low
	// rising edge detected
	TCCR1B &= ~(1<<CS10); // stop timer
	//frqTmp = ;
	return 8000000UL/TCNT1;
}
/*-------------------------------------------------------------*/

/*************************** UNDEFINE ***************************/
#undef HZ122_MHZ8
#undef HZ61_MHZ4
#undef HZ2_KHZ62
#undef FREQ_RANGE
/*--------------------------------------------------------------*/

/************************* EXAMPLE CODE *************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"
#include "mega16/int/timer0/timer0.h"
#include "mega16/int/timer1/T1FreqMeter.h"



int main() {
   initGPIO();
   LCDInit(LS_NONE);
   LCDClear();
   
   _delay_ms(200);
    DDRB |= (1<<PB3);
    OCR0 = 1;
    uint32_t freq=3266;

    // generate frequency at PB3
    // connect PB3 to PA0 with a 
    // jumper wire
   T0ClockSelect(T0_PRESCALER_256);
   T0operationMode(T0_OPMODE_CTC);
   T0ocMode(T0_OC0_TOGGLE);

   
   while(1) { 
       // frequency is measured at PA0
       freq = getT1Freq(A0);
      _delay_ms(300);
      LCDWriteIntXY(0,0,(uint16_t)freq,5);
   }
}


---------------------------------------------------------------*/