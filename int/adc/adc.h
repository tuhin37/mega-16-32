/**************************** DESCRIPTION *****************************************
This Function deals with ADC for MCU. 
Mainly thers Two modes of operation. Single channel polling type, Internal interrupt
driven free running type.

If you wish to operate in free running mode, then in user configurable section 
make "IS_FREE_RUNNING" as "YES".

In free running mode you also have to specify which all mux channels are to be
included in the digitization process. You can specify it in  "muxIndex[]"
array. if that array has {0, 2, 7} it means the program will digitize ADC0, 
ADC2, ADC7 and the results will be available at "adcResults[0]", "adcResults[2]" 
and  "adcResults[7]" respectively. Rest all value in adcResults[] will be zero
or garbage.


USER FUNCTIONS:
	initADC(); => Initializes ADC. Needs ADC voltage refference as input.  
	selAutoTriggerADC(); => If you wish to set up auto trigger then use it
	ReadADC	=> Pool single channel, Takes channel number as argument
	adcInterruptEnable() => Enables ADC interrupt

---------------------------------------------------------------------------------*/



/****************************** RDEFINE ******************************/
#undef YES
#undef NO
#define YES 1
#define NO 2

// Fefference Voltage for ADC
#define ADC_AVCC	4
#define ADC_AREF	5
#define ADC_2_56 	6

// auto trigger mode
#define ADC_TRIG_FREE_RUNNING 0
#define ADC_TRIG_ANA_COMP 1
#define ADC_TRIG_INTR0 2
#define ADC_TRIG_T0_COMP 3
#define ADC_TRIG_T0_OVF 4
#define ADC_TRIG_T1_COMPB 5
#define ADC_TRIG_T1_OVF 6
#define ADC_TRIG_T1_INPUT_CAP 7 
/*-------------------------------------------------------------------*/




/************************* USER CONFIGURABLE *************************/
#define IS_FREE_RUNNING YES


#if IS_FREE_RUNNING == YES
	// this channels are digitized in endless cycle. Reffer page 215, Table - 84 
	volatile uint8_t muxIndex[] = { 4, 6, 7 }; // which mux combinations are used
#endif
/*-------------------------------------------------------------------*/



/****************************** GLOBAL *******************************/
#if IS_FREE_RUNNING == YES
	struct adc{
		volatile uint8_t muxIndexLen;
		volatile uint8_t currMuxIndexIndex;
	} adc;

	// all the results are stored here in free running mode
	volatile uint16_t adcResults[28]; // reffer page 215 of data sheet. 28 different mux combination
#endif
/*-------------------------------------------------------------------*/






/*************************** USER FUNCTIONS ***************************/
void initADC(uint8_t refVoltage) {
	#if IS_FREE_RUNNING == YES
		// set mux
		adc.muxIndexLen = sizeof(muxIndex)/sizeof(uint8_t);
		adc.currMuxIndexIndex = 255; // not yet set condition
 	#endif

	// turn the ADC om
	ADCSRA |= (1<<ADEN);
	// adc clock prescaller to 128
	ADCSRA |= (3);  // Page 217, Table 85
	

	// set refference voltage
	// Page 214, Table 83 
	if(refVoltage == ADC_AVCC) {
		ADMUX &= ~(192);
		ADMUX |= (64);
	}
	else if(refVoltage == ADC_AREF) {
		ADMUX &= ~(192);
	}
	else if(refVoltage == ADC_2_56) {
		ADMUX |= (192);	
	}

	// start  first conversion
	#if IS_FREE_RUNNING == YES
		ADCSRA |= (1<<ADIE); // enable interrupt
		sei();
		// start the first conversion
		ADMUX &= ~(31); // clear the previously selected channel in ADMUX
		// set the first mux in the que
		ADMUX |= muxIndex[0];
		adc.currMuxIndexIndex = 0;
		_delay_us(700); // give time for the mux to change
		ADCSRA |= (1<<ADSC);
	#endif
}


// reffer page 218 Table 86 for auto trigger
void selAutoTriggerADC(uint8_t trigSrc) {
	ADCSRA |= (1<<ADATE); // auto trigger enable

	if (trigSrc == ADC_TRIG_FREE_RUNNING) {
		SFIOR &= ~(224);
		ADCSRA |= (1<<ADSC);	
	}
	else {
		SFIOR &= ~(224);
		trigSrc &= 7;
		SFIOR |= (trigSrc << 5);	
	}
}


// single channel read on demand.
// channels can be 0-28 reffer page 215 Table-84
uint16_t ReadADC(uint8_t ch){
	ADMUX &= ~(31); // clear the previously selected channel in ADMUX
	ADMUX |= ch;
	_delay_us(700); // give time for the mux to change

	//start conversion
	ADCSRA |= (1<<ADSC);

	while(!(ADCSRA & (1<<ADIF)));
	ADCSRA |= (1<<ADIF); // clear interrupt flag
	return(ADC); 
}


// enable adc interrupt
#define adcInterruptEnable() (ADCSRA |= (1<<ADIE))  // ISR(ADC_vect)	
/* ------------------------------------------------------------------*/




/******************************** ISR ********************************/
#if IS_FREE_RUNNING == YES
	ISR(ADC_vect) {
		
		// read and store recent conversion result
		adcResults[muxIndex[adc.currMuxIndexIndex]] = ADC;
		
		// clear admux 
		ADMUX &= ~(31); 
		

		if(adc.currMuxIndexIndex < (adc.muxIndexLen-1)) {
			adc.currMuxIndexIndex++;
		}
		else {
			adc.currMuxIndexIndex = 0;
		}
		ADMUX |= muxIndex[adc.currMuxIndexIndex];

		_delay_us(15);
		ADCSRA |= (1<<ADSC);
	}
#endif
/*-------------------------------------------------------------------*/





/****************************** EXAMPLE CODE SINGLE CONVERSION ******************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/int/adc/adc.h"
#include "mega16/ext/lcd16x2/lcd.h"

int main() {
    LCDInit(LS_NONE);
    _delay_ms(100);
    LCDClear();
    
    initADC(ADC_AVCC);
    
    while(1) { 
        LCDWriteIntXY(0, 0, ReadADC(0), 4);
		LCDWriteIntXY(0, 1, ReadADC(1), 4);
        LCDWriteIntXY(5, 0, ReadADC(2), 4);
        LCDWriteIntXY(5, 1, ReadADC(4), 4);
        _delay_ms(100);
    }
}
-------------------------------------------------------------------------------------------*/




/****************************** EXAMPLE CODE FREE RUNNING ***********************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/int/adc/adc.h"
#include "mega16/ext/lcd16x2/lcd.h"

int main() {
    LCDInit(LS_NONE);
    _delay_ms(100);
    LCDClear();
    initADC(ADC_AVCC);
    while(1) { 
        
        LCDWriteIntXY(0, 0, adcResults[4], 4);
		    LCDWriteIntXY(0, 1, adcResults[6], 4);
        _delay_ms(100);
    }
}
// NOTE: in adc.h do the following changes
// #define IS_FREE_RUNNING YES (Line Number 53)
// volatile uint8_t muxIndex[] = { 4, 6 }; (Line number 58)
-------------------------------------------------------------------------------------------*/
