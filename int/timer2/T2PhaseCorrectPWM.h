/********************* DESCRIPTION ***********************
This library uses Timer2 to generage Phase Correct PWM
at OC2 pin a.k.a. PD7

NOTE:
	DDRD have to mbe set to make PD7 as output
--------------------------------------------------------*/





/*********************** DEPENDENCY ***********************/
#ifndef T2_OPMODE_NORMAL
	#include "timer2.h"
#endif
/*--------------------------------------------------------*/







/********************* USER FUNCTIONS *********************/ 
// initialize and start the PWM
void T2PhCrPWMStart() {
	DDRD |= (1<<PD7);	//make OC0 as output 
	T2operationMode(T2_OPMODE_PHASE_PWM);
	T2ocMode(T2_OC2_CLR_ON_MATCH);
	T2ClockSelect(T2_PRESCALER_1); // timer starts running 16Mhz
	OCR2 = 0; // start off with 0 volts
}

// Set duty cycle
void T2PhCrPWMduty(uint8_t duty) {
	OCR2 = duty;
}
/*--------------------------------------------------------*/








/********************** EXAMPLE CODE ***********************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"
#include "mega16/int/timer2/T2PhaseCorrectPWM.h"


int main() {  
    DDRB |= (1<<PB3);
	T2PhCrPWMStart();
  	T2PhCrPWMduty(200);
   	while(1) { 
   	}
}
----------------------------------------------------------