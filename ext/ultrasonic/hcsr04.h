/***************************************** DESCRIPTION ****************************************
This library helps in interfacing HC-SR04 ultrasonic range finder module with AVR MCU.
The module essentially has four pins, VCC, GND, ECHO and TRIGGER

The way it works is one has to give 10us high  pulse at trigger pin, then the device send out 
sonic waves and the module calculates the time difference between those sonic waves being
sent and received. This duration info is then sent to the MCU through echo pin. So after the 
module receives sonic reflections, it pulls up the echo pin exactly for the same time duration 
which is the sonic waves took to bounce back.

After sending 10us high pulse MCU waits for echo pin to go high. One this happens MCU starts 
timer0 at a known rate (2 Mhz) in this case. The timer0 runs as long as the echo pin stays high
Timer0 stops immediately after echo pin comes to low.
This timeduration is essentially the 16bit value that is proportional to the distace.

To make it work programmer must crate object of class "ultrasound", let the object be "us0"
e.g.: struct ultrasound us0; (global scope)
	This object will inherently have the following properties
		us0.echo 	=> echo pin position
		us0.trig    => trigger pin position
		us0.power	=> if the module is powered through a transistor switch (active low)
		us0.range   => desired distace 

 USER FUNCTION:
 	USgetRange(&us0) => will take us0 object and finaly get the range update in us0.range

 NOTE: 
 	"gpio.h" is must. And befor USgetRange(&us0) is called initGPIO(); has to be called
  
 REFERENCE:
 	"https://www.google.co.in/url?sa=t&rct=j&q=&esrc=s&source=web&cd=2&ved=0CCMQF
 	jABahUKEwjU5arszczIAhWIj44KHX6OCLU&url=http%3A%2F%2Fwww.micropik.com%2FPDF%2FH
 	CSR04.pdf&usg=AFQjCNGQka2FGj3tvAzU2HEDfgZvvCQpNw&sig2=7DfYGthWIHsBO7Wyq55T3w"

 	"http://extremeelectronics.co.in/avr-tutorials/interfacing-ultrasonic-rangefinder
 	-with-avr-mcus-%E2%80%93-avr-tutorial/"
 ---------------------------------------------------------------------------------------------*/







/***************************************** DEPENDENCY *****************************************/
#ifndef GPIO
	#include "../../io/gpio.h"
#endif

#ifndef T0_FRQ_METER
	#include "../../int/timer0/T0FreqMeter.h"
#endif
/*---------------------------------------------------------------------------------------------*/







/******************************************** GLOBAL*********************************************/
struct  ultrasound{
	volatile uint8_t echo;
	volatile uint8_t trig;
	volatile uint8_t power;
	volatile uint16_t range;
};
/*---------------------------------------------------------------------------------------------*/






/***************************************** USER FUNCTION ****************************************/
void USgetRange(struct  ultrasound* us) {
	if(us->power) { // if defined
		outLow(us->power); // active low transistor switch
	}

	// set up the timer0
	T0ClockSelect(T0_PRESCALER_NONE); // T0 stopped
	T0operationMode(T0_OPMODE_NORMAL);
	T0ocMode(T0_OC0_NORNAL);
	enableT0OVFInterrupt();
	sei();

	// send 10us pulse over trig pin
	outHigh(us->trig);
	_delay_us(10);
	outLow(us->trig);

	// wait as long as echo is low
	while(!getInput(us->echo));
	// rising edge detected
	T0overflow=0;
	TCNT0 = 0;
	T0ClockSelect(T0_PRESCALER_8); // T0 started at 2 Mhz
	// wait and count as long as echo stays high
	while(getInput(us->echo));
	// echo dropped to low
	// stop counter
	T0ClockSelect(T0_PRESCALER_NONE);
	us->range = 256*T0overflow + TCNT0;
	
	if(us->power) {
		outHigh(us->power); // turn off (active low transistor switch)
	}
	_delay_ms(10); // inter poll delay
}
/*------------------------------------------------------------------------------------------------*/





/****************************************** EXAMPLE CODE *****************************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/io/gpio.h"
#include "lib/lcd.h"

#include "mega16/ext/ultrasonic/hcsr04.h"


// declare ultrasonic range finder object
struct ultrasound us0;


int main() {
    
    initGPIO();
    
    LCDInit(LS_NONE);
    LCDClear();
    
    // configure us0 module
    us0.echo = D6;
    us0.trig = A0;
    us0.power = 0; // not in use
    us0.range = 0; // pristine 

    while(1) { 
        
        USgetRange(&us0); // poll for range
        LCDWriteIntXY(0,0,us0.range,5);  
        _delay_ms(100);
    }
}
------------------------------------------------------------------------------------------------*/
