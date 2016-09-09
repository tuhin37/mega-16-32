/************************ DESCRIPTION **********************
This library uses 8 Bit Timer2 to measure frequency of an 
input signal. This library starts counting at a rising edge and 
counts upto falling edge with a known speed, and thats how it
calculated frequency from half (positive width) peroid.
 -----------------------------------------------------------*/


/************************* DEFINITION *************************/
#ifndef T2_PRESCALER_NONE
	#include "timer2.h"
#endif

#ifndef GPIO
  #include "../../io/gpio.h"
#endif

/*------------------------------------------------------------*/



/************************ USER FUNCTIONS ***********************/
uint32_t getT2Freq(volatile uint8_t pos ) {
	enableT2OVFInterrupt(); // enable over flow interrupt
	sei();
	T2overflow = 0;
	T2ClockSelect(T2_PRESCALER_NONE); // clear clock
	//wait as long as its high
	while(getInput(pos));
	// wait as long as its low
	while(!getInput(pos));
	// rising edge detected
	TCNT2 = 0;
	T2ClockSelect(T2_PRESCALER_1); // start counter at 16Mhz 
	while(*getInput(pos)); // wait and count as long as its high
	// falling edge detected
	T2ClockSelect(T2_PRESCALER_NONE); // stop timer
	return 8000000UL/(TCNT2 + T2overflow*256);
}
/*-------------------------------------------------------------*/






/************************* EXAMPLE CODE *************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"

#include "mega16/int/timer0/timer0.h"
#include "mega16/int/timer2/T2FreqMeter.h"


int main() {
    initGPIO();  
    LCDInit(LS_NONE);
    LCDClear();
   
    _delay_ms(200);
    DDRB |= (1<<PB3);
    OCR0 = 10;
    uint32_t freq;

    // generate frequency at PB3
    // connect PB3 to PA0 with a 
    // jumper wire
    T0ClockSelect(T0_PRESCALER_64);
    T0operationMode(T0_OPMODE_CTC);
    T0ocMode(T0_OC0_TOGGLE);

   
    while(1) { 
      // frequency is measured at PA0
      freq = getT2Freq(&PINA, PA0);
      LCDWriteIntXY(0,0,(uint16_t)freq,5);
      _delay_ms(300);
    }
}



---------------------------------------------------------------*/