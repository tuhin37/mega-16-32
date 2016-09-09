/************************ DESCRIPTION ************************
This library contains all the macros and functions for 
low evel control of timer0
-------------------------------------------------------------*/




/************************ DEFINITIONS ************************/
#define T0_PRESCALER_NONE 	0
#define T0_PRESCALER_1		1
#define T0_PRESCALER_8		8
#define T0_PRESCALER_64		64
#define T0_PRESCALER_256	256
#define T0_PRESCALER_1024	1024
#define T0_PRESCALER_EXT_FALL	2
#define	T0_PRESCALER_EXT_RISE	3

#define T0_OPMODE_NORMAL 	0
#define T0_OPMODE_PHASE_PWM	1
#define T0_OPMODE_CTC 		2
#define T0_OPMODE_FAST_PWM	3

#define T0_OC0_NORNAL	0
#define T0_OC0_TOGGLE	1
#define T0_OC0_CLR_ON_MATCH	2
#define	T0_OC0_SET_ON_MATCH	3
/*-----------------------------------------------------------*/


/*************************** GLOBAL ***************************/
uint16_t T0overflow;
/*------------------------------------------------------------*/

/****************** LOW LEVEL USER FUNCTIONS ******************/

// options are 0, 1, 8, 64, 256, 1024, 'f', 'r' ("falling edge" & "rising edge")  
void T0ClockSelect(uint16_t clk) {
	switch(clk) {
		case T0_PRESCALER_NONE:
			TCCR0 &= ~(7);
			break;
		
		case T0_PRESCALER_1:
			TCCR0 &= ~(7);
			TCCR0 |= 1;
			break;
		
		case T0_PRESCALER_8:
			TCCR0 &= ~(7);
			TCCR0 |= 2;
			break;

		case T0_PRESCALER_64:
			TCCR0 &= ~(7);
			TCCR0 |= 3;
			break;

		case T0_PRESCALER_256:
			TCCR0 &= ~(7);
			TCCR0 |= 4;
			break;
		
		case T0_PRESCALER_1024:
			TCCR0 &= ~(7);
			TCCR0 |= 5;
			break;
		
		case T0_PRESCALER_EXT_FALL:
			TCCR0 &= ~(7);
			TCCR0 |= 6;
			break;

		case T0_PRESCALER_EXT_RISE:
			TCCR0 &= ~(7);
			TCCR0 |= 7;
			break;

		default:
			break;
	}
}

// 0 => 'normal';	1 => 'PWM Phase Correct'
// 2 => 'CTC';		3 => 'fast PWM'
void T0operationMode(uint8_t modeNumber) {
	switch(modeNumber) {
		case T0_OPMODE_NORMAL: 	// normal mode
			TCCR0 &= ~(72);
			break;

		case T0_OPMODE_PHASE_PWM:	// phase correct PWM
			TCCR0 &= ~(72);	
			TCCR0 |= 64;
			break;

		case T0_OPMODE_CTC:	// CTC
			TCCR0 &= ~(72);	
			TCCR0 |= 8;
			break;

		case T0_OPMODE_FAST_PWM:	// fast PWM
			TCCR0 &= ~(72);	
			TCCR0 |= 72;
			break;

		default:
			break;
	}
}


void T0ocMode(uint8_t oc0Mode) {
	switch(oc0Mode) {
		case 0:
			TCCR0 &= ~(48);
			break;

		case 1:
			TCCR0 &= ~(48);
			TCCR0 |= 16;
			break;

		case 2:
			TCCR0 &= ~(48);
			TCCR0 |= 32;
			break;

		case 3:
			TCCR0 &= ~(48);
			TCCR0 |= 48;
			break;

		default:
			break;
	}
}


#define enableOC0Interrupt() (TIMSK |= (1<<OCIE0))  	// ISR (TIMER0_COMPA_vect)

#define disableOC0Interrupt() (TIMSK &= ~(1<<OCIE0))

#define enableT0OVFInterrupt() (TIMSK |= (1<<TOIE0))	//ISR(TIMER0_OVF_vect)

#define disableT0OVFInterrupt() (TIMSK &= ~(1<<TOIE0))

#define clearOC0InterruptFlag() (TIFR |= (1<<OCF0))

#define clearT0OVFInterruptFlag() (TIFR |= (1<<TOV0))

#define T0disable() (T0ClockSelect(T0_PRESCALER_NONE)) // stops the timmer from runnign
/*--------------------------------------------------------------*/





/****************************** ISR ******************************/
ISR(TIMER0_OVF_vect) {
	T0overflow++;
}
/*---------------------------------------------------------------*/
