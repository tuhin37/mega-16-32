/************************ DESCRIPTION **********************
This library uses 8 Bit Timer0 to measure frequency of an 
input signal. This library starts counting at a rising edge and 
counts upto falling edge with a known speed, and thats how it
calculated frequency from half (positive width) peroid.

NOTE: 
	Needs gpio.h to work. The programmer has to call
	initGPIO() before calling getT0Freq()
 -----------------------------------------------------------*/


/************************* DEFINITION *************************/
#ifndef T0_PRESCALER_NONE
	#include "timer0.h"
#endif

#ifndef GPIO
	#include "../../io/gpio.h"
#endif

#define T0_FRQ_METER 1 // identifies  that this library is already included 
/*------------------------------------------------------------*/





/************************ USER FUNCTIONS ***********************/
uint32_t getT0Freq(uint8_t pos) {
	
	enableT0OVFInterrupt(); // enable over flow interrupt
	sei();
	T0overflow = 0;
	T0ClockSelect(T0_PRESCALER_NONE); // clear clock
	T0operationMode(T0_OPMODE_NORMAL);
	T0ocMode(T0_OC0_NORNAL);
	//wait as long as its high
	while(getInput(pos));
	// wait as long as its low
	while(!getInput(pos));
	// rising edge detected
	TCNT0 = 0;
	T0ClockSelect(T0_PRESCALER_1); // start counter at 16Mhz 
	while(getInput(pos)); // wait and count as long as its high
	// falling edge detected
	T0ClockSelect(T0_PRESCALER_NONE); // stop timer
	return 8000000UL/(TCNT0 + T0overflow*256);
}

/*-------------------------------------------------------------*/


/************************* EXAMPLE CODE *************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"
#include "mega16/io/gpio.h"
#include "mega16/int/timer1/T1FreqGen.h"
#include "mega16/int/timer0/T0FreqMeter.h"




int main() {
	initGPIO();
  	LCDInit(LS_NONE);
  	LCDClear();
   
  	_delay_ms(200);
  	DDRB |= (1<<PB3);
  	uint32_t freq=0;   
 
  	// generate frequency at PD7
  	T2freqInit();
  	T2freqStart(T2_PRESCALER_32, 66);
   
  	while(1) { 
    	// frequency is measured at PA0
    	freq = getT0Freq(D7);  
    	_delay_ms(100);
    	LCDWriteIntXY(0,0,(uint16_t)freq,5);
   	}
}


// NOTE: since Dev Brd V3 has Defalut LCD Pin RS(pin4 of LCD)
// connected to PD7. In this example I had to change LCD pin 
// RS from PD7 to PD7 and connect those two with external 
// jumper and had to turn the switch off.
----------------------------------------------------------------*/