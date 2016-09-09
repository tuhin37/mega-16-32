/********************** DESCRIPTION  **********************
This library makes use of timer2 to maintain accurate timing

USER FUNCTION:	
	initT2timeKeeper();
	pauseTimeKeeper();  // time values are retained
	resumeTimeKeeper(); // time values start incrementing from the paused state
	disableTimeKeeper(); // counter stops and all time values resets to zero.

GLOBAL: 
	time.us; microsecond value
	time.s; second
	time.m; minute
	time.h; hour
	time.d; day
	
	timeLoop.msCounter0
	timeLoop.msCounter1
	timeLoop.msCounter2
	timeLoop.msCounter3
	timeLoop.sCounter0
	timeLoop.sCounter1
	timeLoop.sCounter2
	timeLoop.sCounter3



NOTE:
	Minimum time base is 10us so whne that is selected 
	timekeeper_us_callback() function is called after every
	10us or 100us and not after every 1us

	If any callback is selected to be YES. The corrosponding 
	function has to be defined by the programmer or else 
	the program WILL NOT COMPILE

	time.isRunningFlag = 2 means its paused


FUTURE:
	Add asynchronous clock support for clocking
---------------------------------------------------------*/


/*********************** DEFINITION ***********************/
#undef TIME_BASE
#undef TEN_US
#undef HUNDRED_US
#undef ONE_MS
#undef YES
#undef NO
#define TEN_US 1
#define HUNDRED_US 2
#define ONE_MS 3
#define YES 1
#define NO 2
#ifndef T2_PRESCALER_NONE
	#include "timer2.h"
#endif
/*--------------------------------------------------------*/



/******************* USER CONFIGURABLES *******************/
#define TIME_BASE ONE_MS	// options are TEN_US, HUNDRED_US, ONE_MS
#define US_CALLBACK NO
#define MS_CALLBACK NO
#define S_CALLBACK NO
#define M_CALLBACK NO
#define H_CALLBACK NO
#define D_CALLBACK NO
/*--------------------------------------------------------*/

/******************** GLOBAL VARIABLES ********************/
struct timekeeper{
volatile uint16_t us;
volatile uint16_t ms;
volatile uint8_t s;
volatile uint8_t m;
volatile uint8_t h;
volatile uint8_t d;
volatile uint8_t isRunningFlag;
}time;

struct timeLoop{
	volatile uint16_t msCounter0;
	volatile uint16_t msCounter1;
	volatile uint16_t msCounter2;
	volatile uint16_t msCounter3;
	volatile uint16_t sCounter0;
	volatile uint16_t sCounter1;
	volatile uint16_t sCounter2;
	volatile uint16_t sCounter3;
}timeLoop;
/*---------------------------------------------------------*/




/********************** USER FUNCTIONS **********************/
void initT2timeKeeper() {
	// reset the clock
	time.us=0;
	time.ms=0;
	time.s=0;
	time.m=0;
	time.h=0;
	time.d=0;
	time.isRunningFlag = 1;

	timeLoop.msCounter0 = 0;
	timeLoop.msCounter1 = 0;
	timeLoop.msCounter2 = 0;
	timeLoop.msCounter3 = 0;
	timeLoop.sCounter0 = 0;
	timeLoop.sCounter1 = 0;
	timeLoop.sCounter2 = 0;
	timeLoop.sCounter3 = 0;

	T2operationMode(T2_OPMODE_CTC);
	T2ocMode(T2_OC2_NORNAL);
	#if TIME_BASE == TEN_US
		T2ClockSelect(T2_PRESCALER_1);
		OCR2=160;
	#elif TIME_BASE == HUNDRED_US
		T2ClockSelect(T2_PRESCALER_8);
		OCR2=200;
	#elif TIME_BASE == ONE_MS
		T2ClockSelect(T2_PRESCALER_64);
		OCR2=250;
	#endif
	sei();
	enableOC2Interrupt();
}

void pauseTimeKeeper(){
	T2ClockSelect(T2_PRESCALER_NONE);
	T2operationMode(T2_OPMODE_NORMAL);
	T2ocMode(T2_OC2_NORNAL);
	disableOC2Interrupt();
	time.isRunningFlag = 2; // means paused
}

void resumeTimeKeeper() {
	time.isRunningFlag = 1;
	T2operationMode(T2_OPMODE_CTC);
	T2ocMode(T2_OC2_NORNAL);
	#if TIME_BASE == TEN_US
		T2ClockSelect(T2_PRESCALER_1);
		OCR2=160;
	#elif TIME_BASE == HUNDRED_US
		T2ClockSelect(T2_PRESCALER_8);
		OCR2=200;
	#elif TIME_BASE == ONE_MS
		T2ClockSelect(T2_PRESCALER_64);
		OCR2=250;
	#endif
	sei();
	enableOC2Interrupt();	
}

void disableTimeKeeper(){
	pauseTimeKeeper();
	time.us=0;
	time.ms=0;
	time.s=0;
	time.m=0;
	time.h=0;
	time.d=0;
	timeLoop.msCounter0 = 0;
	timeLoop.msCounter1 = 0;
	timeLoop.msCounter2 = 0;
	timeLoop.msCounter3 = 0;
	timeLoop.sCounter0 = 0;
	timeLoop.sCounter1 = 0;
	timeLoop.sCounter2 = 0;
	timeLoop.sCounter3 = 0;
}

/************************* PROTOTYPES *************************/
#if US_CALLBACK == YES
	void timekeeper_us_callback(void);
#endif
#if MS_CALLBACK == YES
	void timekeeper_ms_callback(void);
#endif
#if S_CALLBACK == YES
	void timekeeper_s_callback(void);
#endif
#if M_CALLBACK == YES
	void timekeeper_m_callback(void);
#endif
#if H_CALLBACK == YES
	void timekeeper_h_callback(void);
#endif
#if D_CALLBACK == YES
	void timekeeper_d_callback(void);
#endif
/*------------------------------------------------------------*/





#if TIME_BASE == TEN_US
ISR (TIMER2_COMP_vect){
	time.us += 10;
	#if US_CALLBACK == YES
		timekeeper_us_callback();
	#endif
	if(time.us==1000) {
		time.us=0;
		time.ms++;
		if(timeLoop.msCounter0){
			timeLoop.msCounter0--;
		}
		if(timeLoop.msCounter1) {
			timeLoop.msCounter1--;
		}
		if(timeLoop.msCounter2) {
			timeLoop.msCounter2--;
		}
		if(timeLoop.msCounter3) {
			timeLoop.msCounter3--;	
		}
		#if MS_CALLBACK == YES
			timekeeper_ms_callback();
		#endif
		if(time.ms==1000) {
			time.ms=0;
			time.s++;
			if(timeLoop.sCounter0) {
				timeLoop.sCounter0--;
			}
			if(timeLoop.sCounter1) {
				timeLoop.sCounter1--;
			}
			if(timeLoop.sCounter2) {
				timeLoop.sCounter2--;
			}
			if(timeLoop.sCounter3) {
				timeLoop.sCounter3--;
			}
			#if S_CALLBACK == YES
				timekeeper_s_callback();
			#endif
			if(time.s==60) {
				time.s=0;
				time.m++;
				#if M_CALLBACK == YES
					timekeeper_m_callback();
				#endif
				if(time.m==60) {
					time.m=0;
					time.h++;
					#if H_CALLBACK == YES
						timekeeper_h_callback();
					#endif
					if(time.h==24){
						time.h=0;
						time.d++;
						#if D_CALLBACK == YES
							timekeeper_d_callback();
						#endif
						if(time.d==255){
							time.d=0;
						}
					}
				}
			}
		}
	}
}
#endif




#if TIME_BASE == HUNDRED_US
ISR (TIMER2_COMP_vect){
	time.us += 100;
	#if US_CALLBACK == YES
		timekeeper_us_callback();
	#endif
	if(time.us==1000) {
		time.us=0;
		time.ms++;
		if(timeLoop.msCounter0){
			timeLoop.msCounter0--;
		}
		if(timeLoop.msCounter1) {
			timeLoop.msCounter1--;
		}
		if(timeLoop.msCounter2) {
			timeLoop.msCounter2--;
		}
		if(timeLoop.msCounter3) {
			timeLoop.msCounter3--;	
		}
		#if MS_CALLBACK == YES
			timekeeper_ms_callback();
		#endif
		if(time.ms==1000) {
			time.ms=0;
			time.s++;
			if(timeLoop.sCounter0) {
				timeLoop.sCounter0--;
			}
			if(timeLoop.sCounter1) {
				timeLoop.sCounter1--;
			}
			if(timeLoop.sCounter2) {
				timeLoop.sCounter2--;
			}
			if(timeLoop.sCounter3) {
				timeLoop.sCounter3--;
			}
			#if S_CALLBACK == YES
				timekeeper_s_callback();
			#endif
			if(time.s==60) {
				time.s=0;
				time.m++;
				#if M_CALLBACK == YES
					timekeeper_m_callback();
				#endif
				if(time.m==60) {
					time.m=0;
					time.h++;
					#if H_CALLBACK == YES
						timekeeper_h_callback();
					#endif
					if(time.h==24){
						time.h=0;
						time.d++;
						#if D_CALLBACK == YES
							timekeeper_d_callback();
						#endif
						if(time.d==255){
							time.d=0;
						}
					}
				}
			}
		}
	}
}
#endif



#if TIME_BASE == ONE_MS
ISR (TIMER2_COMP_vect){
	time.ms++;
	if(timeLoop.msCounter0){
		timeLoop.msCounter0--;
	}
	if(timeLoop.msCounter1) {
		timeLoop.msCounter1--;
	}
	if(timeLoop.msCounter2) {
		timeLoop.msCounter2--;
	}
	if(timeLoop.msCounter3) {
		timeLoop.msCounter3--;	
	}
	#if MS_CALLBACK == YES
		timekeeper_ms_callback();
	#endif
	if(time.ms==1000) {
		time.ms=0;
		time.s++;
		if(timeLoop.sCounter0) {
				timeLoop.sCounter0--;
			}
			if(timeLoop.sCounter1) {
				timeLoop.sCounter1--;
			}
			if(timeLoop.sCounter2) {
				timeLoop.sCounter2--;
			}
			if(timeLoop.sCounter3) {
				timeLoop.sCounter3--;
			}
		#if S_CALLBACK == YES
			timekeeper_s_callback();
		#endif
		if(time.s==60) {
			time.s=0;
			time.m++;
			#if M_CALLBACK == YES
				timekeeper_m_callback();
			#endif
			if(time.m==60) {
				time.m=0;
				time.h++;
				#if H_CALLBACK == YES
					timekeeper_h_callback();
				#endif
				if(time.h==24){
					time.h=0;
					time.d++;
					#if D_CALLBACK == YES
						timekeeper_d_callback();
					#endif
					if(time.d==255){
						time.d=0;
					}
				}
			}
		}
	}
}
#endif



/***************************** EXAMPLE CODE *****************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/int/timer2/T2timekeeper.h"
#include "mega16/ext/lcd16x2/lcd.h"




int main() {
  
   LCDInit(LS_NONE);
   LCDClear();
   DDRA |=(1<<PA7);

   initT2timeKeeper();
   _delay_ms(200);

   
  timeLoop.msCounter0 = 5000;
   while(1) { 
       LCDWriteIntXY(0,0,time.s,2);
       _delay_ms(100);
       if(timeLoop.msCounter0) {
        PORTA ^= (1<<PA7); // this is executed only for initial 5 seconds
       }
   }
}

-----------------------------------------------------------------------*/
