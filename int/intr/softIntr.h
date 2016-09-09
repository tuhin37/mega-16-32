/**************** DESCRIPTION ****************
This library can generate software interrupts.
It uses INT0, INT1, INT2 (Either of them). It
Currently supports 8 different software interrupt vectors


USER FUNCTIONS:
	1.softIntrInit();
	2.softIntrTrig(n); // nth vector call
	3.softIntVecX(); replace 'X' with 0..7

NOTE: 
	If SOFT_INTR_VECT_X is defined to be 'YES' then
	the programmer has to explicitly define corresponding 
	software interrupt vecotr i.e. softIntVecX(). Otherwise
	the library WILL NOT COMPILE
--------------------------------------------*/



/***************** INTERNALS *****************/
#undef YES
#undef NO
#define YES 1
#define NO 2
/*---------------------------------------------------*/


/***************** USER CONFIGURABLES *****************/
#define INTX_PIN INT0	// options are INT0, INT1, INT2

#define SOFT_INTR_VECT_0 YES // options are: YES, NO
#define SOFT_INTR_VECT_1 NO
#define SOFT_INTR_VECT_2 NO
#define SOFT_INTR_VECT_3 NO 
#define SOFT_INTR_VECT_4 NO
#define SOFT_INTR_VECT_5 NO
#define SOFT_INTR_VECT_6 NO
#define SOFT_INTR_VECT_7 NO
/*---------------------------------------------------*/


/********************* DEPENDENCY *********************/
#if INTX_PIN == INT0
	#include "int0.h"
#elif INTX_PIN == INT1
	#include "int1.h"
#elif INTX_PIN == INT2
	#include "int2.h"
#endif
/*----------------------------------------------------*/


/*********************** GLOBAL ***********************/
struct  softIntr {
	volatile uint8_t vectorNumber;	
}softIntrGlobal;

/*----------------------------------------------------*/



/******************* USER FUNCTIONS *******************/
void softIntrInit() {
	softIntrGlobal.vectorNumber = 255;
	#if INTX_PIN == INT0
		// set INT0 pin for falling edge interrupt trigger.
		cli(); 
		DDRD |= (1<<PD2);	// make it output 
		PORTD |=(1<<PD2); 	// connect internal pull up
   		MCUCR |= (2<<0); 	// falling edge trigger on INT0 (PD2); ISC01:0=2
   		GICR |= (1<<INT0); // enable INT0 interrupt
   		sei();   // set global interrupt	
	#endif

   	#if INTX_PIN == INT1
		// set INT0 pin for falling edge interrupt trigger.
		cli(); 
		DDRD |= (1<<PD3);	// make it output 
		PORTD |=(1<<PD3); 	// connect internal pull up
   		MCUCR |= (1<<ISC11); // falling edge trigger on INT1 (PD3); ISC11:0=2
   		MCUCR &= ~(1<<ISC10);
   		GICR |= (1<<INT1); // enable INT0 interrupt
   		sei();   // set global interrupt	
	#endif

   	#if INTX_PIN == INT2
		// set INT0 pin for falling edge interrupt trigger.
		cli(); 
		DDRB |= (1<<PB2);	// make it output 
		PORTB |=(1<<PB2); 	// connect internal pull up
   		MCUCSR &= ~(1<<ISC2); // falling edge trigger on INT2 (PB2); ISC2=0
   		GICR |= (1<<INT2); // enable INT0 interrupt
   		sei();   // set global interrupt	
	#endif
}


void softIntrTrig(uint8_t intrNumber) {
	#if INTX_PIN == INT0
		softIntrGlobal.vectorNumber = intrNumber;
		PORTD &= ~(1<<PD2);		
	#endif

	#if INTX_PIN == INT1
		softIntrGlobal.vectorNumber = intrNumber;
		PORTD &= ~(1<<PD3);		
	#endif

	#if INTX_PIN == INT2
		softIntrGlobal.vectorNumber = intrNumber;
		PORTB &= ~(1<<PB2);		
	#endif
}
/*-----------------------------------------------------*/




/********************* PROTOTYPES *********************/
#if SOFT_INTR_VECT_0 == YES
	void softIntVec0(void);
#endif

#if SOFT_INTR_VECT_1 == YES
	void softIntVec1(void);
#endif

#if SOFT_INTR_VECT_2 == YES
	void softIntVec2(void);
#endif

#if SOFT_INTR_VECT_3 == YES
	void softIntVec3(void);
#endif

#if SOFT_INTR_VECT_4 == YES
	void softIntVec4(void);
#endif

#if SOFT_INTR_VECT_5 == YES
	void softIntVec5(void);
#endif

#if SOFT_INTR_VECT_6 == YES
	void softIntVec6(void);
#endif

#if SOFT_INTR_VECT_7 == YES
	void softIntVec7(void);
#endif
/*----------------------------------------------------*/


void softIntr() {
	switch(softIntrGlobal.vectorNumber) {
		case 0:
			#if SOFT_INTR_VECT_0 == YES
			softIntVec0();
			#endif
			break;

		case 1:
			#if SOFT_INTR_VECT_1 == YES
			softIntVec1();
			#endif
			break;

		case 2:
			#if SOFT_INTR_VECT_2 == YES
			softIntVec2();
			#endif
			break;

		case 3:
			#if SOFT_INTR_VECT_3 == YES
			softIntVec3();
			#endif
			break;

		case 4:
			#if SOFT_INTR_VECT_4 == YES
			softIntVec4();
			#endif
			break;

		case 5:
			#if SOFT_INTR_VECT_5 == YES
			softIntVec5();
			#endif
			break;
		
		case 6:
			#if SOFT_INTR_VECT_6 == YES
			softIntVec6();
			#endif
			break;

		case 7:
			#if SOFT_INTR_VECT_7 == YES
			softIntVec7();
			#endif
			break;

		default:
			break;
	}
}



#if INTX_PIN == INT0
	void int0_Callback() {
		PORTD |= (1<<PD2);
   		softIntr();
		softIntrGlobal.vectorNumber = 255; // invalid vector
	}
#endif

#if INTX_PIN == INT1
	void int1_Callback() {
		PORTD |= (1<<PD3);
   		softIntr();
		softIntrGlobal.vectorNumber = 255; // invalid vector
	}
#endif

#if INTX_PIN == INT2
	void int2_Callback() {
		PORTB |= (1<<PB2);
   		softIntr();
		softIntrGlobal.vectorNumber = 255; // invalid vector
	}
#endif


/******************* EXAMPLE CODE *******************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/int/intr/softIntr.h"


int main() {
   softIntrInit();
   DDRA |= (1<<PA7);
   DDRA |= (1<<PA6);
   while(1) { 
     
      _delay_ms(500);
      softIntrTrig(0);
      _delay_ms(1000);
      softIntrTrig(5);
     }
}

void softIntVec0() {
   PORTA ^= (1 << PA7);
}


void softIntVec5() {
   PORTA ^= (1 << PA6);
}

-----------------------------------------------------*/
