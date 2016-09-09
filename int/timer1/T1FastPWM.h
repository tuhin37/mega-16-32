/******************** DESCRIPTION ********************
This library is used to generate FAST PWM with Timer1.
Since this timer has two output comrare units, namely 
OC1A and OC1B, Its possible to generate two independent 
PWM with this timmer. 
One available at OC1A pin (PD5)
another at OC1B pin (PD4)

User functions are self explainatory
----------------------------------------------------*/

/*********************** DEPENDENCY ***********************/
#ifndef T1_PRESCALER_NONE
	#include "timer1.h"
#endif
/*--------------------------------------------------------*/


/********************* USER FUNCTIONS *********************/ 
// initialize and start the PWM
void T1AfastPWMStart() { 
	T1ClockSelect(T1_PRESCALER_1); // running at 16Mhz
  	T1operationMode(5); // 8 bit fast PWM mode; datasheet page 109
	OCR1A = 0; // start off with 0 volts
}

// Set duty cycle
void T1AfastPWMduty(uint8_t duty) {
	DDRD |= (1<<PD5);	//make OC1A as output
	T1ocAMode(T1_OC_CLR_ON_MATCH);
	OCR1A = (duty*256)/100;
}

// Set duty cycle
void T1BfastPWMduty(uint8_t duty) {
	DDRD |= (1<<PD4);	//make OC1B as output
	T1ocBMode(T1_OC_CLR_ON_MATCH);
	OCR1B = (duty*256)/100;
}
/*--------------------------------------------------------*/


/********************** EXAMPLE CODE ***********************
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "mega16/int/timer1/T1FastPWM.h"



int main() {
	T1AfastPWMStart();
	T1AfastPWMduty(30);
	T1BfastPWMduty(60);

	while(1) { 
	}
}

----------------------------------------------------------*/
