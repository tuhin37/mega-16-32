/***************************************** DESCRIPTION *****************************************
This Library help to interface colorsensor TSC3200 with avr MCU. 
Each color sensor module will have structure "colorSensor".
example "struct colorSensor cs0;" each of these objects will 
inherit following properties:
	red 	- > color value
	green 	- > color value
	blue	- > color value
	clear	- > color value
	s1 : s0	- > frequency prescaller pins
	s3 : s2 - > color select pins 
	vOut	- > frequency out pin
	power 	- > active low power pin (transistor driven) 

Associated Methods are:
	initColorSensor(&cs0); 		-> initiates the color sensor "cs0" 
	CSsetFreqPercent(&cs0,20);	-> set frequency prescaller of "cs0" to 20 %
	CSgetColor(&cs0);			-> When called will update the color values once

NOTE: 
	To use this library programmer will must have to call "initGPIO();" before cleating cs object	


REFERENCE:
	http://extremeelectronics.co.in/avr-tutorials/interfacing-tcs3200-colour-sensor-with-avr-atmega32/
	http://www.dfrobot.com/wiki/index.php/TCS3200_Color_Sensor_(SKU:SEN0101)
----------------------------------------------------------------------------------------------*/



/***************************************** DEPENDENCY *****************************************/
#ifndef GPIO
	#include "../../io/gpio.h"
#endif

#ifndef T0_FRQ_METER
	#include "../../int/timer0/T0FreqMeter.h"
#endif
/*---------------------------------------------------------------------------------------------*/




/******************************************* DATATYPE *******************************************/
struct colorSensor{
	// color 
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t clear;
	// freq scaler (optional)
	uint8_t s0;		// s1:s0 = 0 --> powerwoen; s1:s0 = 2 --> 20% freq
	uint8_t s1;		// s1:s0 = 1 --> 2 % freq;  s1:S0 = 3 --> 100% freq
	//color select
	uint8_t s2;		//  s2:se3  0 --> red;	1 --> blue;
	uint8_t s3;		//  2 --> clear; 	3 --> green
	// freq out
	uint8_t vOut;
	// power up pin (optional)
	uint8_t power; // transistoe control
};
/*---------------------------------------------------------------------------------------------*/




/**************************************** USER FUNCTIONS ****************************************/
void initColorSensor(struct colorSensor *cs) {
	// power test sensor color sensor
	if(cs->power) { // zero means invalid. so if not zero
		outLow(cs->power); // inverting transistor switch
		_delay_ms(200);
		outHigh(cs->power);
	}

	if(cs->s0 && cs->s1) { // if frequency scalling pins are defined
		// keep them at 100%
		outHigh(cs->s0);
		outLow(cs->s1);
	}

	//set color to clear
	outHigh(cs->s2);
	outLow(cs->s3); 
}


void CSsetFreqPercent(struct colorSensor *cs, uint8_t percent) {
	if(!percent) { // if at zero
		outLow(cs->s0);
		outLow(cs->s1);
	}

	else if(percent > 0 && percent <= 2) {
		outLow(cs->s0);
		outHigh(cs->s1);
	}

	else if (percent > 2 && percent <= 20) {
		outHigh(cs->s0);
		outLow(cs->s1);
	}

	else {
		outHigh(cs->s0);
		outHigh(cs->s1);
	}	

}


void CSgetColor(struct colorSensor *cs) {
	uint32_t freq;

	// power up the sensor
	if(cs->power) { // zero means invalid. so if not zero
		outLow(cs->power); // assumes inverter transistoe switch
	}


	// select green and read
	outHigh(cs->s2);
	outHigh(cs->s3);
	freq = getT0Freq(cs->vOut);
	cs->green = freq/200;
	if(freq > 51000) { // saturation condition
		cs->green=255;
	}

	// select blue and read
	outLow(cs->s2);
	outHigh(cs->s3);
	freq = getT0Freq(cs->vOut);
	cs->blue = freq/200;
	if(freq > 51000) { // saturation condition
		cs->blue=255;
	}

	// select clear and read
	outHigh(cs->s2);
	outLow(cs->s3);
	freq = getT0Freq(cs->vOut);
	cs->clear = freq/200;
	if(freq > 51000) { // saturation condition
		cs->clear=255;
	}

	// select red and read
	outLow(cs->s2);
	outLow(cs->s3);
	freq = getT0Freq(cs->vOut);
	cs->red = freq/200;
	if(freq > 51000) { // saturation condition
		cs->red=255;
	}

	// power off
	if(cs->power) { // zero means invalid. so if not zero
		outHigh(cs->power); // assumes inverter transistoe switch
	}	
}
/*----------------------------------------------------------------------------------------------*/






/***************************************** EXAMPLE CODE *****************************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/io/gpio.h"

#include "mega16/ext/coloSensor/TSC3200.h"
#include "lib/lcd.h"


// declear color sensor object
struct colorSensor cs0;


int main() {
    initGPIO();
    

    // configure color sensors pinout
    cs0.power = A2;
    cs0.s3 = D7;
    cs0.s2 = D6;
    cs0.vOut = A0; 

    // initiate color sensor 
    initColorSensor(&cs0);  

    
    LCDInit(LS_NONE);
    LCDClear();
    initGPIO();

    while(1) { 
        
        //update color
        CSgetColor(&cs0);

        //show colors 
        LCDWriteIntXY(0,0,cs0.red,3);  
        LCDWriteIntXY(5,0,cs0.green,3);
        LCDWriteIntXY(9,0,cs0.blue,3);
        LCDWriteIntXY(0,1,cs0.clear,3);
        
        _delay_ms(100);
    }
}
-----------------------------------------------------------------------------------------------*/
