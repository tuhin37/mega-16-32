/*********************** DESCRIPTION ************************
This library contains all the macros and functions for 
low evel control of timer2
-------------------------------------------------------------*/

/************************ DEFINITIONS ************************/
#define T2_PRESCALER_NONE 	0
#define T2_PRESCALER_1		1
#define T2_PRESCALER_8		8
#define T2_PRESCALER_32		32
#define T2_PRESCALER_64		64
#define T2_PRESCALER_128	128
#define T2_PRESCALER_256	256
#define T2_PRESCALER_1024	1024


#define T2_OPMODE_NORMAL 	0
#define T2_OPMODE_PHASE_PWM	1
#define T2_OPMODE_CTC 		2
#define T2_OPMODE_FAST_PWM	3


#define T2_OC2_NORNAL	0
#define T2_OC2_TOGGLE	1
#define T2_OC2_CLR_ON_MATCH	2
#define	T2_OC2_SET_ON_MATCH	3
/*-----------------------------------------------------------*/



/*************************** GLOBAL ***************************/
uint16_t T2overflow;
/*------------------------------------------------------------*/






/*********************** USER FUNCTIONS ***********************/
void T2ClockSelect(uint16_t clk) {
	switch(clk) {
		case T2_PRESCALER_NONE:
			TCCR2 &= ~(7);
			break;
		
		case T2_PRESCALER_1:
			TCCR2 &= ~(7);
			TCCR2 |= 1;
			break;
		
		case T2_PRESCALER_8:
			TCCR2 &= ~(7);
			TCCR2 |= 2;
			break;

		case T2_PRESCALER_32:
			TCCR2 &= ~(7);
			TCCR2 |= 3;
			break;

		case T2_PRESCALER_64:
			TCCR2 &= ~(7);
			TCCR2 |= 4;
			break;
		
		case T2_PRESCALER_128:
			TCCR2 &= ~(7);
			TCCR2 |= 5;
			break;
		
		case T2_PRESCALER_256:
			TCCR2 &= ~(7);
			TCCR2 |= 6;
			break;

		case T2_PRESCALER_1024:
			TCCR2 |= 7;
			break;

		default:
			break;
	}
}

// 0 => 'normal';	1 => 'PWM Phase Correct'
// 2 => 'CTC';		3 => 'fast PWM'
void T2operationMode(uint8_t modeNumber) {
	switch(modeNumber) {
		case T2_OPMODE_NORMAL: 	// normal mode
			TCCR2 &= ~(72);
			break;

		case T2_OPMODE_PHASE_PWM:	// phase correct PWM
			TCCR2 &= ~(72);	
			TCCR2 |= 64;
			break;

		case T2_OPMODE_CTC:	// CTC
			TCCR2 &= ~(72);	
			TCCR2 |= 8;
			break;

		case T2_OPMODE_FAST_PWM:	// fast PWM
			TCCR2 &= ~(72);	
			TCCR2 |= 72;
			break;

		default:
			break;
	}
}



void T2ocMode(uint8_t ocMode) {
	switch(ocMode) {
		case T2_OC2_NORNAL:
			TCCR2 &= ~(48);
			break;

		case T2_OC2_TOGGLE:
			TCCR2 &= ~(48);
			TCCR2 |= 16;
			break;

		case T2_OC2_CLR_ON_MATCH:
			TCCR2 &= ~(48);
			TCCR2 |= 32;
			break;

		case T2_OC2_SET_ON_MATCH:
			TCCR2 &= ~(48);
			TCCR2 |= 48;
			break;

		default:
			break;
	}
}



#define enableOC2Interrupt() (TIMSK |= (1<<OCIE2))  	// ISR (TIMER2_COMPA_vect)

#define disableOC2Interrupt() (TIMSK &= ~(1<<OCIE2))

#define enableT2OVFInterrupt() (TIMSK |= (1<<TOIE2))	//ISR(TIMER2_OVF_vect)

#define disableT2OVFInterrupt() (TIMSK &= ~(1<<TOIE2))

#define clearOC2InterruptFlag() (TIFR |= (1<<OCF2))

#define clearT2OVFInterruptFlag() (TIFR |= (1<<TOV2))

#define T2disable() (T2ClockSelect(T2_PRESCALER_NONE)) // stops the timmer from runnign
/*--------------------------------------------------------------*/






/****************************** ISR ******************************/
ISR(TIMER2_OVF_vect) {
	T2overflow++;
}
/*---------------------------------------------------------------*/








/****************************** EXAMPLE CODE *****************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/int/timer2/timer2.h"
#include "lib/lcd.h"




int main() {
  DDRD |= (1<<PD7);
  T2operationMode(T2_OPMODE_CTC);
  T2ocMode(T2_OC2_TOGGLE);
  T2ClockSelect(T2_PRESCALER_256);
  OCR2 = 100;   
  while(1) { 
       
  }
}

**************************************************************************/