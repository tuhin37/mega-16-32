/*********************** DSCRIPTION ***********************
This "SOFTWARE USART "library helps in the case where the HARDWARE USART of
an MCU is already used up by "USB to USART" of "RF MODULES"
and then thers no other USART port left.
This library can turn any two GPIO pins into RX and TX pins

NOTE: 
	"softUSARTRead()" function unlike interrupt driven functions 
	do block the execution sequence until the byte is receieved successfully.
	The Rx interrupt driven soft USART library is also available "softUSARTIntr.h"

GLOBAL STRICTURE:
	With this library programmers can create objects of class softUsart.
	lets assume the programmer decides to create an object called "usart0"
	To do that execute: "struct softUsart usart0;" Programmer can create 
	more than one objects usaer1, usart2 and so on. Essentially each of 
	thes objects represents one virtual usart hardware connected to the MCU.
		Each of these usart objets will the the following properties:
			usart0.rx => RXD pin for that particular usart module 
			usart0.tx => TXD pin for that particular usart module
			usart0.receivedByte => holds the last received data byte




USER FUNCTIONS:
	1. initSoftUSART(&usart0); => initializes the software USART pins of usart0
	2. softUSARTWrite(&usart0, 'a'); => writes a byte (e.g. 'a') onto TX pin of usart0
	3. softUSARTRead(&usart0); => receieves a byte from RX pin of usart0 and will update
								  usart0.receivedByte.	

NOTE: 
	"gpio.h" is a must. Before using any functions of this
	library the programmer will have to call "initGPIO()".
	Right now all usart objects will work with same baud
	specified in user configurable area. For some reason 
	baud rate upto 4800 is working fine and not beyond that.

	When sending two bytes back to back from computer its
	not working.


FUTURE SCOPE:
	I tried to specify individual baud rates for individual objects,
	usart0.baud=2400;
	This should calculate baud delay autometically 
	"usart->baudDelay = 1000000UL/usart->baud;"
	and then call soft delay instead of _delay_us().
	Didnt work. I'm guessing either 
	"usart->baudDelay = 1000000UL/usart->baud;" this division
	is faulty of the soft delay function overhead messing up
	delay timing. Have to check with oscilloscope.
----------------------------------------------------------*/




/******************* USER CONFIGURABLE *******************/
#define BAUD 2400
/*-------------------------------------------------------*/



/*********************** DEPENDENCY ***********************/
#ifndef GPIO
    #include "../io/gpio.h"
#endif
/*--------------------------------------------------------*/




/************************* GLOBAL *************************/
 struct softUsart{
 	volatile uint8_t rx;
 	volatile uint8_t tx;
 	volatile uint8_t receivedByte;
 	volatile uint16_t baud;
 	volatile uint16_t baudDelay;
 };
/*--------------------------------------------------------*/


/*********************** INTERNAL ***********************/
#define BAUD_DELAY 1000000UL/BAUD
/*-----------------------------------------------------*/





/******************* USER FUNCTIONS *******************/
void initSoftUSART(struct softUsart *usart) {
	usart->receivedByte = 0;
	inHigh(usart->rx);  // active low bus
	outHigh(usart->tx); // idle state is high
	usart->baudDelay = 1000000UL/usart->baud;
}


// This function send a byte of data through TX_POS defined above
void softUSARTWrite(struct softUsart *usart, uint8_t byte) {
	//send start bit
	outLow(usart->tx);
	_delay_us(BAUD_DELAY);
	//soft_delay_us(BAUD_DELAY);
	// send data byte LSB to MSB
	uint8_t i;
	for(i=0; i<8; i++) {
		if(byte & 0b00000001) {
			outHigh(usart->tx);
		}
		else {
			outLow(usart->tx);
		}
		_delay_us(BAUD_DELAY);
		//soft_delay_us(BAUD_DELAY);
		// shift data 1 bit to the right
		byte = (byte>>1);
	}
	//send stop condition
	outHigh(usart->tx);
}


// This function receieves a byte of data through RX_POS defined above
void softUSARTRead(struct softUsart *usart) {
	uint8_t byte=0x00;
	uint8_t i;
	if(!getInput(usart->rx)) { // !!!! Caution This functions blocks the execution untill a byte is receieved !!!!
		return; // Error condition, data receieve in progress
	}

	while(getInput(usart->rx)); // wait unless idle
	//if a start bit is receieved 
	_delay_us(BAUD_DELAY>>2); //.5 delay; go to the middle of start bit.
	// sample the following 8 data bits
	for(i=0; i<8; i++) {
		_delay_us(BAUD_DELAY);
		// sample the first bit
		if(getInput(usart->rx)) {
			byte |= (1<<i);
		}	
	}
	usart->receivedByte = byte;
}
/*--------------------------------------------------------*/







/********************** EXAMPLE CODE **********************
#include <avr/io.h>
#include <util/delay.h>
#include "mega16/softUSART/softUSART.h"

struct softUsart usart0;

int main() {
    initGPIO();
    usart0.rx = A0;
    usart0.tx = A1;

    initSoftUSART(&usart0);
    uint8_t data,i;
    while(1) {
        softUSARTRead(&usart0);
        softUSARTWrite(&usart0, usart0.receivedByte+1);
    }
}
----------------------------------------------------------*/
