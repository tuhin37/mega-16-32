/****************************** DESCRIPTION ******************************
This library makes operation of hardware USART present in Mega16/32
easy and intuative. This library can handel auto data filtering when 
using with RF modules. Programmer will have to specify the expecting
packet format (rcvPacketFormat[]) in advance in user configurable area 
and number of expected data bytes (RCV_PKT_LEN). 

If "PKT_RX_FINISH_CALLBACK" is made as "YES" then there will be a
callback "packetReceved_callpack()" after every successful packet 
receive (!! Not after every byte receive).

The received packet will be available in "rcvPacket[]" array.
The array will hold just the data bytes and not the mask bytes
After every successfull packet receive the array will be updated 
with new values.


USER FUNCTIONS:
	USARTInit(BaudRate); => Initializes Usart with specified Baud rate
							also turns on the receive interrupt, and
							global interrupt as well

	UWriteData(byteToSend); => Sends an Atomic byte over TX of USART
	
	avrRXoff() 	These macros are useful in power management. 
	avrRXon()	programmers can make use of these and turn on
	avrTXoff()	and off internal RX and TX modules of AVR when  
	avrTXon()	they are not in use.

NOTE: 
	If "PKT_RX_FINISH_CALLBACK" is set to "YES" then the programmer
	has to explicitly define  "void packetReceved_callpack(void)" 
	or else the library WILL NOT COMPILE.

	When using with RF module its found to be best performing with
	baud rate 2400 and 4 of these "_delay_loop_2(0);" in every 
	iteration of the send packet loop.

	If you are using ATmega32 instead of ATmega16 then do set it up
	in AVR studio project config option, else RF modules do not work
------------------------------------------------------------------------*/





/****************************** DEPENDENCY *******************************/
#ifndef GPIO
	#include "../../io/gpio.h"
#endif
#undef NO
#undef YES
#define YES 1
#define NO 2
/*-----------------------------------------------------------------------*/





/****************************USER CONFIGURABLE ***************************/
#define RCV_PKT_LEN 4  // number of data bytes to be received

// specify the packet format. Note that you can specify only 3 initial 
// mask bytes and one terminating mask byte. number of Data bytes 
// can be varied and here denoted by '0'(s) 
volatile uint8_t rcvPacketFormat[] = { 'J', 'A', 'A', 0, 0, 0, 0, 'Z' };
#define PKT_RX_FINISH_CALLBACK NO
/*-----------------------------------------------------------------------*/




/******************************** GLOBAL *********************************/
volatile uint8_t rcvPacket[RCV_PKT_LEN];  // holds the received packet(mask
// bytes are not included).  
/*-----------------------------------------------------------------------*/






/***************************** USER FUNCTIONS *****************************/

// initialize this function with required baud rate
void USARTInit(uint32_t baudrate) {
	set(UCSRB, RXEN);	// power up RX Module
	set(UCSRB, TXEN);	// power up TX Module

   	// set data frame as 8bit long
	set(UCSRC, UCSZ1);
	set(UCSRC, UCSZ0);
	set(UCSRC, URSEL);

	// Calculate UBRR value acording to the baud rate
	uint16_t ubrrVal = (((F_CPU / (baudrate * 16UL))) - 1);
	
	// Set UBRR value
	UBRRH = hByte(ubrrVal); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRRL = lByte(ubrrVal); // Load lower 8-bits of the baud rate value into the low byte of the UBRR register

	set(UCSRB, RXCIE);  // Enable the USART Recieve Complete interrupt  ISR(USART_RXC_vect) {}
 	sei();
}


/* This function sends data  through TX */
void UWriteData(char data) {
	//Wait For Transmitter to become ready
	while(!(UCSRA & (1<<UDRE)));
	//Now write
	UDR=data;
}


/* power on and power off the Internal USART RX and TX modules to save power */
#define avrRXoff() (UCSRB &= ~(1<<RXEN))
#define avrRXon() (UCSRB |= (1<<RXEN))
#define avrTXoff() (UCSRB &= ~(1<<TXEN))
#define avrTXon() (UCSRB |= (1<<TXEN))
/*----------------------------------------------------------------------------------*/







/*************** Global Variables Intermidiate(temporary) ************************/
volatile uint8_t rcvDockTmp[RCV_PKT_LEN];// temporary docking place
volatile uint8_t rcvPacketIndex = 0;
volatile uint8_t prevRcvByte = 0;
volatile uint8_t newRcvByte = 0;
#define Z_CASE_NUM RCV_PKT_LEN + 2
/*------------------------------------------------------------------------------*/




/*********************************** PROTOTYPE ***********************************/
#if PKT_RX_FINISH_CALLBACK == YES
	void packetReceved_callpack(void);
#endif
/*-------------------------------------------------------------------------------*/




/****************************** RX Interrupt Vector ******************************/
ISR(USART_RXC_vect) {

	newRcvByte = UDR;
	//UDR = newByte; //  echos the received byte
	
	// store just the data bytes (not mask bytes)
	if(rcvPacketIndex>=2 && rcvPacketIndex < RCV_PKT_LEN+2 ){
		rcvDockTmp[rcvPacketIndex-2] = newRcvByte;
		rcvPacketIndex++;
		return;
	}


	switch (rcvPacketIndex) {
		// check the first 'J' => 'A' transition
		case 0:
			if (prevRcvByte == rcvPacketFormat[0] && newRcvByte == rcvPacketFormat[1]) {
				rcvPacketIndex++;
			}
			prevRcvByte = newRcvByte;
			break;

		case 1:
			if(newRcvByte == rcvPacketFormat[2]) {
				rcvPacketIndex++;
			}
			else {
				rcvPacketIndex = 0;
			}
			break;

		case Z_CASE_NUM:
			if(newRcvByte == rcvPacketFormat[RCV_PKT_LEN+3] ) { // successfull 
				for(rcvPacketIndex = 0; rcvPacketIndex < RCV_PKT_LEN; rcvPacketIndex++) {
					rcvPacket[rcvPacketIndex] = rcvDockTmp[rcvPacketIndex]; // copy data from temporary dock to the final packet
				}
				/*<<<<<<<<<<<<< This application specific code starts  <<<<<<<<<<<<<*/
				#if PKT_RX_FINISH_CALLBACK == YES
					packetReceved_callpack();
				#endif
				/*>>>>>>>>>>>>>>>>> Application specific code ends >>>>>>>>>>>>>>>>>*/
			}
			rcvPacketIndex = 0;
			break;
		
		default:
			break;
	}
}
/*-------------------------------------------------------------------------------*/








/******************************** EXAMPLE CODE ************************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/int/usart/usart.h"
#include "lib/lcd.h"

int main() {
    LCDInit(LS_NONE);
    _delay_ms(100);
    LCDClear();

    USARTInit(2400);
    
    while(1) { 
        LCDWriteIntXY(0, 0, rcvPacket[0], 2);
        UWriteData('A'); // send byte
		    LCDWriteIntXY(0, 1, rcvPacket[3], 3);
        UWriteData(23);  // send byte
        _delay_ms(100);
    }
}
---------------------------------------------------------------------------------*/