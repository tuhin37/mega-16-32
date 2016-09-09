/*
	This library is created by Tuhin Sengupta.
	Email: tuhin3737@gmail.com
	Creation date: 15th Feb, 2015
	last modified: 15th Feb, 2015, 11:18 pm

	This librery contains functions and macros for 
	implementing RTC( Real time clock )  in
	Atmega8, Atmega16 & Atmega32 with the internal counters



Resources:
	
	FUNCTIONS
	---------
		1. InitTimeKeeper() 
		2. ms_callback()
		3. s_callback()
		4. m_callback()
		5. h_callback()
	

	VARIABLES
	---------
		1. clock_us
		2. clock_ms
		3. clock_s
		4. clock_m
		5. clock_h
		 

*/

#define HUNDRED_US 1
#define ONE_MS 2 
#define OFF 1
#define A 1
#define B 2
#define YES 1
#define NO 2

#ifndef GPIO
	#include "gpio.h"
#endif

struct timekeeper {
	volatile uint16_t us;
	volatile uint16_t ms;
	volatile uint8_t s;
	volatile uint8_t m;
	volatile uint8_t h;
}timeKeeper;







/*------------------------------- User configurable area ------------------------------------- */
  
	// set which timer to use. Options available: 0, 1, 2
	#define TIMER 2
	
	// maximum count time in Hour. Can range upto 255
	#define MX_HOUR 24

	// define whats the clock frequency (MHz) before prescaller. 
	// Options : 1, 2, 4, 8, 12, 16
	#define CLOCK 16

	// set the time resolution. Minimum time stem to be counted
	// Options: HUNDRED_US, ONE_MS
	#define TIME_BASE ONE_MS
	
	
	//options FOR TIMER-2: OFF, 1, 8, 32, 64, 128, 256, 1024
	//options FOR TIMER-1: OFF, 1, 8, 64, 256, 1024
  
	#define OCR_REG A // A or B.  timer1 has two  Output Compare Register OCR1A, OCR1B


	// call backs: assign 'NO' to the ones which are not going to be used.
	// options: YES, NO
	#define CLOCK_MS_CALLBACK NO
	#define CLOCK_S_CALLBACK NO
	#define CLOCK_M_CALLBACK NO
	#define CLOCK_H_CALLBACK NO

	/*
	 ++++++++++++++ NOTE **********************
	 Always do this calculation. 
	 system clock = 16 MHz. counter clock = 16Mhz/64 = 250 Khz.
	 the counter counts 250 in 1 ms. thats why the OCR = 250, and prescaller = 64 and 
	 system clock = 16 Mhz
	*/



// these are the call backs.. 
#if CLOCK_MS_CALLBACK == YES
	 void ms_callback(){
	 	// code here
	 		freqCount =  TCNT1;
	 		TCNT1 = 0;
	 		ovf = 0;
	 }
#endif

#if CLOCK_S_CALLBACK == YES
	 void s_callback(){
	 	// code here
	 }
#endif
	 #if CLOCK_M_CALLBACK == YES
	 void m_callback(){
	 	// code here
	 }
#endif
	 #if CLOCK_H_CALLBACK == YES
	 void h_callback(){
	 	// code here
	 }
#endif



/* ----------------------------- user configurable setion end here ---------------------------- */




#if TIME_BASE == HUNDRED_US
 	#if CLOCK == 16
 		#define PRESCALLER 64
 		#define OCR_VAL 25
 	#elif CLOCK == 12
 		#define PRESCALLER 8
 		#define OCR_VAL 150
 	#elif CLOCK == 8
 		#define PRESCALLER 8
 		#define OCR_VAL 100
 	#elif CLOCK == 4
 		#define PRESCALLER 8
 		#define OCR_VAL 50
 	#elif CLOCK == 2
 		#define PRESCALLER 8
 		#define OCR_VAL 25
 	#elif CLOCK == 1
 		#define PRESCALLER 1
 		#define OCR_VAL 100
 	#endif
#endif

#if TIME_BASE == ONE_MS
 	#if CLOCK == 16
 		#define PRESCALLER 64
 		#define OCR_VAL 250
 	#elif CLOCK == 8
 		#define PRESCALLER 64
 		#define OCR_VAL 125
 	#elif CLOCK == 4
		#if TIMER == 2
	 		#define PRESCALLER 32
	 		#define OCR_VAL 125
		#endif
 	#elif CLOCK == 2
 		#define PRESCALLER 8
 		#define OCR_VAL 250
 	#elif CLOCK == 1
 		#define PRESCALLER 8
 		#define OCR_VAL 125
 	#endif
#endif

#ifndef OCR_VAL
	 #error SUITABLE VALUE FOR PRESCALER AND OCR COULD NOT BE FOUND
#endif





void InitTimeKeeper() {
	// reset the clock
	timeKeeper.us=0;
	timeKeeper.ms=0;
	timeKeeper.m=0;
	timeKeeper.h=0;


	#ifdef OCR_VAL
		#if TIMER == 2
			#undef OCR_REG
			set(TCCR2, WGM21); // CTC mode: clear counter value on compare match
			OCR2 = OCR_VAL; // conter reaches this value after 1ms @ 250 khz.
			set(TIMSK, OCIE2);//Enable the Output Compare A interrupt
			#if PRESCALLER == 1
				set(TCCR2, CS20); 
			#elif PRESCALLER == 8
				set(TCCR2, CS21);
			#elif PRESCALLER == 32
				set(TCCR2, CS20);
				set(TCCR2, CS21);
			#elif PRESCALLER == 64
				set(TCCR2, CS22);
			#elif PRESCALLER == 128
				set(TCCR2, CS22);
				set(TCCR2, CS20);
			#elif PRESCALLER == 256
				set(TCCR2, CS22);
				set(TCCR2, CS21);
			#elif PRESCALLER == 1024
				set(TCCR2, CS20);
				set(TCCR2, CS21);
				set(TCCR2, CS22);
			#elif PRESCALLER == OFF
				clear(TCCR2, CS20);
				clear(TCCR2, CS21);
				clear(TCCR2, CS22);
			#endif
		#endif

		#if TIMER == 1
			#if OCR_REG == A
				OCR1A = OCR_VAL; // conter reaches 25 after 100us @ 250 Khz.
			#elif OCR_REG == B
				OCR1B = OCR_VAL;
			#endif
			set(TCCR1B, WGM12); // CTC mode: clear counter value on compare match
			set(TIMSK, OCIE1A);//Enable the Output Compare A interrupt    
			#if PRESCALLER == 1	
				set(TCCR1B, CS10);  // Set prescaler division factor 64 
			#elif PRESCALLER == 8
				set(TCCR1B, CS11);
			#elif PRESCALLER == 64
				set(TCCR1B, CS10);
				set(TCCR1B, CS11);
			#elif PRESCALLER == 256
				set(TCCR1B, CS12);
			#elif PRESCALLER == 1024
				set(TCCR1B, CS10);
				set(TCCR1B, CS12);
			#elif PRESCALLER == OFF
				clear(TCCR1B, CS10);
				clear(TCCR1B, CS11);
				clear(TCCR1B, CS12);
			#endif
		#endif

		#if TIMER == 0
			#undef OCR_REG
			OCR0 = OCR_VAL;
			set(TCCR0, WGM01); // CTC mode for timer0
			set(TIMSK, OCIE0); // interrupt enable on compare match
			#if PRESCALLER == 1	
				set(TCCR0, CS00);  // Set prescaler division factor 64 
			#elif PRESCALLER == 8
				set(TCCR0, CS01);
			#elif PRESCALLER == 64
				set(TCCR0, CS00);
				set(TCCR0, CS01);
			#elif PRESCALLER == 256
				set(TCCR0, CS02);
			#elif PRESCALLER == 1024
				set(TCCR0, CS00);
				set(TCCR0, CS02);
			#elif PRESCALLER == OFF
				clear(TCCR0, CS00);
				clear(TCCR0, CS01);
				clear(TCCR0, CS02);
			#endif
		#endif
	#endif
	sei();
}


	
#if TIMER == 2
	#define TIMER_COMP_VECT_NAME TIMER2_COMP_vect
#elif TIMER == 1
	#if OCR_REG == A
		#define TIMER_COMP_VECT_NAME TIMER1_COMPA_vect
	#elif OCR_REG == B
		#define TIMER_COMP_VECT_NAME TIMER1_COMPB_vect
	#endif
#elif TIMER == 0
	#define TIMER_COMP_VECT_NAME TIMER0_COMP_vect
#endif   
ISR(TIMER_COMP_VECT_NAME){
    #if TIME_BASE == HUNDRED_US
	    timeKeeper.us += 100;
	    if(timeKeeper.us == 1000) {
		    timeKeeper.ms++;
		    timeKeeper.us = 0;
		    #if CLOCK_MS_CALLBACK == YES
		    	ms_callback();
		    #endif
			if(timeKeeper.ms==1000) {
		        timeKeeper.s++;
		        timeKeeper.ms=0;
		        #if CLOCK_S_CALLBACK == YES
		    		s_callback();
		    	#endif
		        if(timeKeeper.s==60) {
		            timeKeeper.m++;
		            timeKeeper.s=0;
		            #if CLOCK_M_CALLBACK == YES
		    			m_callback();
		    		#endif
		            if(timeKeeper.m==60) {
		                timeKeeper.h++;
		                timeKeeper.m=0;
		                #if CLOCK_H_CALLBACK == YES
		    				h_callback();
		    			#endif
		                if (timeKeeper.h == MX_HOUR) {
		                    timeKeeper.us = 0;
		                    timeKeeper.ms = 0;
		                    timeKeeper.s = 0;
		                    timeKeeper.m = 0;
		                    timeKeeper.h = 0;
		                }
		            }
		        }
		    }
		}
	#elif TIME_BASE == ONE_MS
	    timeKeeper.ms++;
	    #if CLOCK_MS_CALLBACK == YES
		    	ms_callback();
		 #endif
	    if(timeKeeper.ms==1000) {
	        timeKeeper.s++;
	        timeKeeper.ms=0;
	        #if CLOCK_S_CALLBACK == YES
		    	s_callback();
		    #endif
	        if(timeKeeper.s==60) {
	            timeKeeper.m++;
	            timeKeeper.s=0;
	            #if CLOCK_M_CALLBACK == YES
		    		m_callback();
		    	#endif
	            if(timeKeeper.m==60) {
	                timeKeeper.h++;
	                timeKeeper.m=0;
	                #if CLOCK_H_CALLBACK == YES
		    			h_callback();
		    		#endif
	                if (timeKeeper.h == MX_HOUR) {
	                    timeKeeper.us = 0;
	                    timeKeeper.ms = 0;
	                    timeKeeper.s = 0;
	                    timeKeeper.m = 0;
	                    timeKeeper.h = 0;
	                }
	            }
	        }
	    }
	#endif

}
