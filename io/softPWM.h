/*************************** DESCRIPTION  ********************************
This library is used to generate PWM signals from any of the GPIO pins
PWM signal frequency is fixed at 630 hz and can not be changed.

NOTE:
	Sice CPU actively generates PWM, at a time only one pin can be used
-------------------------------------------------------------------------*/


/************************** USER CONFIGURABLES **************************/

/*----------------------------------------------------------------------*/


/****************************** DEPENDENCY ******************************/
#ifndef GPIO
	#include "gpio.h"
#endif
/*----------------------------------------------------------------------*/


/***************************** USER FUNCTION ****************************/
// duty value is in % 
void genSoftPWM(uint8_t pos, uint8_t duty, uint16_t msCounter) {
	initGPIO();
	// make the soft pwm pin as output
	outLow(pos);
	duty = (uint16_t)(duty<<8)/100;
	while(msCounter) { // change the breaking condition later using auto decrementing ms counter
		// make the pin high
		outHigh(pos);
		soft_delay_us(5*duty);
		outLow(pos);
		soft_delay_us((256-duty)*5);
	}
	// restore prest prestine condition
	inLow(pos);
}
/*------------------------------------------------------------------------*/



/****************************** EXAMPLE CODE *******************************
#include <avr/io.h>
#include <util/delay.h>
#include "mega16/io/softPWM.h"

int main() {
  
	while(1) { 
    	genSoftPWM(A7 ,30, 1);
  	}
}
--------------------------------------------------------------------------*/
