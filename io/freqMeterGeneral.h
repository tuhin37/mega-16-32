/**************************** DESCRIPTION ****************************
This library essentially uses timings of timeKeeper and simply counts 
rising edges ant any GPIO pins and thus calculates the frequeny present 
at that perticular GPIO pin.

Do a initGPIO() first.
---------------------------------------------------------------------*/

/**************************** DEPENDENCY *****************************/
#ifndef TEN_US
	#include "../int/timer2/T2timeKeeper.h"
#endif

#ifndef GPIO
	#include "gpio.h"
#endif
/*-------------------------------------------------------------------*/


/*************************** USER FUNCTION ***************************/
uint32_t getFreq(volatile pos) {
	uint8_t ifPausedFlag = 0;
	uint8_t ifStoppedFlag = 0;
	uint32_t freq=0;

	if(time.isRunningFlag==2) { // if was paused
		resumeTimeKeeper();
		ifPausedFlag = 1;
	}
	if(time.isRunningFlag == 0) { // if it wsa disabled
		initT2timeKeeper();
		ifStoppedFlag = 1;
	}

	timeLoop.msCounter0 = 1000; // load 1000 ms time in counter
	while(timeLoop.msCounter0) { // this loop runs for one second
		while(!getInput(pos)); // wait as long as low
		// rising edge detected;
		freq++; 
		while(getInput(pos)); // wait as long as high
	}

	if(ifPausedFlag) {
		ifPausedFlag = 0;
		pauseTimeKeeper();
	}

	if(ifStoppedFlag) {
		ifStoppedFlag = 0;
		disableTimeKeeper();
	}
	return freq;
}
/*--------------------------------------------------------------------*/


/**************************** EXAMPLE CODE ****************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"
#include "mega16/int/timer0/timer0.h"
#include "mega16/io/freqMeterGeneral.h"



int main() {
	initGPIO();
	LCDInit(LS_NONE);
   	LCDClear();
   
   	_delay_ms(200);
    DDRB |= (1<<PB3);
    OCR0 = 20;
    uint32_t freq=3266;

    initT2timeKeeper();

    // generate frequency at PB3
    // connect PB3 to PA0 with a 
    // jumper wire
   	T0ClockSelect(T0_PRESCALER_256);
   	T0operationMode(T0_OPMODE_CTC);
   	T0ocMode(T0_OC0_TOGGLE);

   
   while(1) { 
       // frequency is measured at PA0
       freq = getFreq(A0);
      _delay_ms(100);
      LCDWriteIntXY(0,0,(uint16_t)freq,5);
   }
}
---------------------------------------------------------------------*/