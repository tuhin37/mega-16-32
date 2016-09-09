/************************ DESCRIPTION ************************
This library contains all the macros and functions for 
low evel control of timer0
-------------------------------------------------------------*/




/************************ DEFINITIONS ************************/
#define T1_PRESCALER_NONE 	0
#define T1_PRESCALER_1		1
#define T1_PRESCALER_8		8
#define T1_PRESCALER_64		64
#define T1_PRESCALER_256	256
#define T1_PRESCALER_1024	1024
#define T1_PRESCALER_EXT_FALL	2
#define	T1_PRESCALER_EXT_RISE	3

#define T1_OC_NORNAL	0
#define T1_OC_TOGGLE	1
#define T1_OC_CLR_ON_MATCH	2
#define	T1_OC_SET_ON_MATCH	3
/*-----------------------------------------------------------*/




/****************** LOW LEVEL USER FUNCTIONS ******************/

// options are 0, 1, 8, 64, 256, 1024, 'f', 'r' ("falling edge" & "rising edge")  
void T1ClockSelect(uint16_t clk) {
	switch(clk) {
		case T1_PRESCALER_NONE:
			TCCR1B &= ~(7);
			break;
		
		case T1_PRESCALER_1:
			TCCR1B &= ~(7);
			TCCR1B |= 1;
			break;
		
		case T1_PRESCALER_8:
			TCCR1B &= ~(7);
			TCCR1B |= 2;
			break;

		case T1_PRESCALER_64:
			TCCR1B &= ~(7);
			TCCR1B |= 3;
			break;

		case T1_PRESCALER_256:
			TCCR1B &= ~(7);
			TCCR1B |= 4;
			break;
		
		case T1_PRESCALER_1024:
			TCCR1B &= ~(7);
			TCCR1B |= 5;
			break;
		
		case T1_PRESCALER_EXT_FALL:
			TCCR1B &= ~(7);
			TCCR1B |= 6;
			break;

		case T1_PRESCALER_EXT_RISE:
			TCCR1B &= ~(7);
			TCCR1B |= 7;
			break;

		default:
			break;
	}
}

// reffer data sheet for mode number Page 109
void T1operationMode(uint8_t modeNumber) {
	TCCR1A |= (modeNumber & 3);
	TCCR1B |= ((modeNumber & 12) << 1);
}




void T1ocAMode(uint8_t ocMode) {
	switch(ocMode) {
		case 0:
			TCCR1A &= ~(192);
			break;

		case 1:
			TCCR1A &= ~(192);
			TCCR1A |= 64;
			break;

		case 2:
			TCCR1A &= ~(192);
			TCCR1A |= 128;
			break;

		case 3:
			TCCR1A |= 192;
			break;

		default:
			break;
	}
}



void T1ocBMode(uint8_t ocMode) {
	switch(ocMode) {
		case 0:
			TCCR1A &= ~(48);
			break;

		case 1:
			TCCR1A &= ~(48);
			TCCR1A |= 16;
			break;

		case 2:
			TCCR1A &= ~(48);
			TCCR1A |= 32;
			break;

		case 3:
			TCCR1A |= 48;
			break;

		default:
			break;
	}
}


// Compare math interrupt OCR1A
#define enableOC1AInterrupt() (TIMSK |= (1<<OCIE1A))  	// ISR (TIMER1_COMPA_vect)
#define disableOC1AInterrupt() (TIMSK &= ~(1<<OCIE1A))

// Compare math interrupt OCR1B
#define enableOC1BInterrupt() (TIMSK |= (1<<OCIE1B))  	// ISR (TIMER1_COMPB_vect)
#define disableOC1BInterrupt() (TIMSK &= ~(1<<OCIE1B))

// Overflow interrupt
#define enableT1OVFInterrupt() (TIMSK |= (1<<TOIE1))	//ISR(TIMER1_OVF_vect)
#define disableT1OVFInterrupt() (TIMSK &= ~(1<<TOIE1))

// input capture interrupt
#define enableInputCapInterrupt() (TIMSK |= (1<<TICIE1))
#define disableInputCapInterrupt() (TIMSK &= ~(1<<TICIE1))

// compare match interrupt flags
#define clearOC1AInterruptFlag() (TIFR |= (1<<OCF1A))
#define clearOC1BInterruptFlag() (TIFR |= (1<<OCF1B))

// overflow interrupt flag
#define clearT1OVFInterruptFlag() (TIFR |= (1<<TOV1))

// input capture interrupt flag
#define clearInputCaptureInterruptFlag() (TIFR |= (1<<ICF1))


// diable timer1
#define T1disable() (T1ClockSelect(T1_PRESCALER_NONE)) // stops the timmer from runnign
/*--------------------------------------------------------------*/




/************************* EXAMPLE CODE *************************
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "mega16/int/timer1/timer1.h"

// this code generates fast PWM at 
// OC1A (PD5) with 200/255 = 78.43% duty ratio

int main() {
	T1ClockSelect(T1_PRESCALER_1); // running at 16Mhz
	T1operationMode(5);
	T1ocAMode(T1_OC_CLR_ON_MATCH);
	OCR1A=200;
	DDRD |= (1<<PD5);
	while(1) { 
	}
}

---------------------------------------------------------------*/