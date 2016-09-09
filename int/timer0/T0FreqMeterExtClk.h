/********************* DESCRIPTION *********************
This library uses Timer 0's external clock pin(T0 at PB0) 
to measure frequency with accuracy of 1 Hz. The way it works
is simple. By definition frequency is number of cycles
in a second. So this library literally counts number of
cycles coming in on T0 pin in exactly one second time.

NOTE:
	The function blocks program execution for 1 Second

--------------------------------------------------------*/

volatile uint16_t T0overflow;

/*********************** DEPENDENCY **********************/
#ifndef T0_OPMODE_NORMAL
	#include "timer0.h"
#endif
/*-------------------------------------------------------*/


/********************* USER FUNCTION *********************/
uint32_t getFreqCountT0Pin() {
	T0overflow = 0;
	enableT0OVFInterrupt();
	sei();
	TCNT0 = 0;
	T0ClockSelect(T0_PRESCALER_EXT_RISE); // set timer 1 clock source: rising edge on T0 pin
	_delay_ms(1000);	// give 1 ms delay
	T0ClockSelect(T0_PRESCALER_NONE);// stop timer0
	return (TCNT0 + T0overflow*256);
}
/*---------------------------------------------------------*/




ISR(TIMER0_OVF_vect) {
	T0overflow++;
}



/*********************** EXAMPLE CODE ***********************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"
#include "mega16/int/timer2/T2FreqGen.h"
#include "mega16/int/timer0/T0FreqMeterExtClk.h"




int main() {
  LCDInit(LS_NONE);
  LCDClear();
   
  _delay_ms(200);
  DDRB |= (1<<PB3);
  uint32_t freq=0;   
 
  // generate frequency at PD7
  // connect external jumper from 
  // PD7 to PB0 (T0) pin
  T2freqInit();
  T2freqStart(T2_PRESCALER_32, 66);
   
  while(1) { 
    // frequency is measured at PB0
    freq = getFreqCountT0Pin();  
    LCDWriteIntXY(0,0,(uint16_t)freq,5);
   }
}

// NOTE: since Dev Brd V3 has Defalut LCD Pin RS(pin4 of LCD)
// connected to PD7. In this example I had to change LCD pin 
// RS from PD7 to PD7 and connect those two with external 
// jumper and had to turn the switch off.

-----------------------------------------------------------*/