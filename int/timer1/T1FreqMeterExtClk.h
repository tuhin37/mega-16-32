/********************* DESCRIPTION *********************
This library uses Timer 1's external clock pin(T0 at PB1) 
to measure frequency with accuracy of 1 Hz

NOTE:
	The function blocks program execution for 1 Second

--------------------------------------------------------*/

volatile uint8_t T1overflow;



/********************* USER FUNCTION *********************/
uint32_t getFreqCountT1Pin() {
	TIMSK |= (1<<TOIE1); // overflow interrupt enable
	T1overflow++;;
	sei();
	TCNT1 = 0;
	TCCR1B |=(7); // set timer 1 clock source: rising edge on T0 pin
	_delay_ms(1000);	// give 1 ms delay
	TCCR1B &= ~(7);// stop timer1
	return (TCNT1 + T1overflow*65536);
}
/*---------------------------------------------------------*/




ISR(TIMER1_OVF_vect) {
	T1overflow++;
}



/*********************** EXAMPLE CODE **********************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/lcd.h"

#include "mega16/int/timer0/timer0.h"
#include "mega16/int/timer1/T1FreqMeterExtClk.h"






int main() {
  
   LCDInit(LS_NONE);
   LCDClear();
   
   _delay_ms(200);
    DDRB |= (1<<PB3);
    OCR0 = 10;
    uint32_t freq;

    // generate frequency at PB3
    // connect PB3 to PB1 with a 
    // jumper wire
   T0ClockSelect(T0_PRESCALER_64);
   T0operationMode(T0_OPMODE_CTC);
   T0ocMode(T0_OC0_TOGGLE);

   
   while(1) { 
      // frequency is measured at PA0
      freq = getFreqCountT1Pin();
      LCDWriteIntXY(0,0,(uint16_t)freq,5);
      _delay_ms(300);
   }
}

-----------------------------------------------------------*/
