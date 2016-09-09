/****************************** DESCRIPTION ******************************
Thsi library generates software frequency.

NOTE:
	softFreqGen() => function takes frequency as an input argument
	softFreqGenPreDef() => 	this function needs to have frequency 
							value assigned(hard coded) in "freq"
							variable within the function definition
							at compile time
------------------------------------------------------------------------*/


#ifndef GPIO
	#include "gpio.h"
#endif



// This function tries to compensate for the fact that 
// _delay_us() needs an argument value known at compile 
// time. 
void softFreq_delay_us(uint16_t n) {
   n = ((uint32_t)n * 205) >> 8;
   while (n--) {
      _delay_us(1);
   }
}


// This function generates frequency on demand, Not very accurate
// but daviations are withing acceptable range. User can specify 
// the desired frequency value to be generated during runtime, passed
// as a function argument during function call.
void softFreqGen(uint8_t pos, uint32_t freq, uint16_t msCounter) {
	initGPIO();
	outLow(pos);
	volatile uint8_t* port;
	uint16_t us; 
	port = getOriginalGPIO(pos, PORTx); // translate from A7  to PORTA
	pos = getOriginalPos(pos); // translate from A7 to PA7
	us = 500000UL/freq;
	
	while(msCounter) {
		*port ^= (1<<pos);
		softFreq_delay_us(us);	
	}
}

// This function generates a frequency predefined at compile time.
// This is more accurate and dosent take frequency as a function
// argument. The frequency has to be assigned to "freq" variabl
// in Hertz withing the function definition 
void softFreqGenPreDef(uint8_t pos, uint16_t msCounter) {
	initGPIO();
	outLow(pos);
	volatile uint8_t* port;
	uint32_t freq = 1000;
	uint16_t us; 
	port = getOriginalGPIO(pos, PORTx); // translate from A7  to PORTA
	pos = getOriginalPos(pos); // translate from A7 to PA7
	us = 500000UL/freq;
	
	while(msCounter) {
		*port ^= (1<<pos);
		_delay_us(us);	
	}
}


/****************************** EXAMPLE CODE ******************************
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "mega16/io/softFreqGen.h"


int main() {
    while(1) { 
      softFreqGen(A7, 420, 1);
   	}
}


// the third argument ideally should be a decrementing time counter
// as long as the third argument is true the function will continue to 
//generate frequency and block the next statements.0
-------------------------------------------------------------------------*/
