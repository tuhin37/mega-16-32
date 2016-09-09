/******************** DESCRIPTION ********************
This library is used to generate Phase correct PWM with Timer0.
The PWM Output is visible on OC0 pin of AVR (PB3 PIN-4)
----------------------------------------------------*/



/*********************** DEPENDENCY ***********************/
#ifndef T0_OPMODE_NORMAL
	#include "timer0.h"
#endif
/*--------------------------------------------------------*/




/********************* USER FUNCTIONS *********************/ 
// initialize and start the PWM
void T0PhCrPWMStart() {
	DDRB |= (1<<PB3);	//make OC0 as output 
	T0operationMode(T0_OPMODE_PHASE_PWM);
	T0ocMode(T0_OC0_CLR_ON_MATCH);
	T0ClockSelect(T0_PRESCALER_1); // timer starts running 16Mhz
	OCR0 = 0; // start off with 0 volts
}

// Set duty cycle
void T0PhCrPWMduty(uint8_t duty) {
	OCR0 = duty;
}
/*--------------------------------------------------------*/




/*********************** SAMPLE CODE ***********************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "T0phaseCorrectPWM.h"


int main() {
   T0PhCrPWMStart();
   T0PhCrPWMduty(200);
   while(1) {   
   }
	// stop timer
	T0disable();
}
----------------------------------------------------------*/
