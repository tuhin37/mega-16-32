/******************************************* DESCRIPTION ********************************************
This library lets you set up an AVR MCU as I2C Slave device with a perticular slave address.
It makes use of AVR's Internal I2C hardware TWI. 


USAGE: 
    initSlave(0x01); this function call will set 0x01 to be the devices slave address. And thats it.
    Slaves do not initiate or terminate any connection. So when master makes a query to this slaves 
    address, It will receive data from slave and store it in slave_recv[] buffer.
    and then it will send whatever's there in buffer slave_tran[]. 



NOTE: 
    if "I2C_EXC_DONE_CALLBACK" is set as "YES" then programmer will have to define i2cExchangeCompleteCallback()
    function or else the library will not compile.
---------------------------------------------------------------------------------------------------*/




/********************************************* DEPENDENCY *********************************************/
#ifndef I2C
    #include "i2c.h"
#endif

#undef YES
#undef NO
#define YES 1
#define NO 2
/*----------------------------------------------------------------------------------------------------*/





/***************************************** USER CONFIGURABLES *****************************************/
#define BUFLEN_RECV 12
#define BUFLEN_TRAN 3
#define I2C_EXC_DONE_CALLBACK NO
// callback prototype.  Programmer can define chanege IO arguments here. 
void i2cExchangeCompleteCallback(void);
/*----------------------------------------------------------------------------------------------------*/




/*********************************************** GLOBAL ***********************************************/
volatile uint8_t slave_r_index =0;
volatile uint8_t slave_recv[BUFLEN_RECV]; //buffer to store received bytes

volatile uint8_t slave_t_index=0;
volatile uint8_t slave_tran[BUFLEN_TRAN] = {0x12, 0x34, 0x56};  // This buffer is sent
/*---------------------------------------------------------------------------------------------------*/





/******************************************* USER FUNCTIONS *******************************************/
void initSlave(uint8_t address) {
    sei();
    TWAR = (address<<1); //we're using address 0x01
    TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
}   


void i2cSlaveHandle() {
    switch(TW_STATUS){
//--------------Slave receiver------------------------------------
        //SLA_W received and acked, prepare for data receiving
        case 0x60:  
            slave_r_index =0;
            TWI_TRANSFER_ACK();
            TWCR |=(1<<TWIE); // enable TWI
            break;
    
        case 0x80:  //a byte was received, store it and 
                    //setup the buffer to recieve another
            slave_recv[slave_r_index] = TWDR;
            slave_r_index++;
            //don't ack next data if buffer is full
            if(slave_r_index >= BUFLEN_RECV){
              TWI_TRANSFER(); // Do not send ACK
              TWCR |=(1<<TWIE);
            }
            else {
                TWI_TRANSFER_ACK();
                TWCR |=(1<<TWIE);
            }
            break;
        
        case 0x68://adressed as slave while in master mode.
              //should never happen, better reset;
            
            TWI_RESET();
            TWCR |=(1<<TWIE);
        
        case 0xA0: //Stop or rep start, reset state machine
            TWI_TRANSFER_ACK();
            TWCR |=(1<<TWIE);
            break;

//-------------- error recovery ----------------------------------
        case 0x88: //data received  but not acked
            //should not happen if the master is behaving as expected
            //switch to not adressed mode
            TWI_TRANSFER_ACK();
            TWCR |=(1<<TWIE);
            break;
//---------------Slave Transmitter--------------------------------
        case 0xA8:  //SLA R received, prep for transmission
                    //and load first data
            slave_t_index=1;
            TWDR = slave_tran[0];
            TWI_TRANSFER_ACK();
            TWCR |=(1<<TWIE);
            break;
        
        case 0xB8:  //data transmitted and acked by master, load next
            TWDR = slave_tran[slave_t_index];
            slave_t_index++;
            //designate last byte if we're at the end of the buffer
            if(slave_t_index >= BUFLEN_TRAN) {
                TWI_TRANSFER(); // no ACK will be sent
                TWCR |=(1<<TWIE);
                #if I2C_EXC_DONE_CALLBACK == YES
                    i2cExchangeCompleteCallback();
                #endif
            }
            else {
                TWI_TRANSFER_ACK();
                TWCR |=(1<<TWIE);
            }
            break;
        
        case 0xC8: //last byte send and acked by master
            //last bytes should not be acked, ignore till start/stop
            //reset=1;
            TWI_RESET();
            TWCR |=(1<<TWIE);
        
        case 0xC0: //last byte send and nacked by master 
            //(as should be)
            TWI_TRANSFER_ACK();
            TWCR |=(1<<TWIE);
            break;
//--------------------- bus error---------------------------------
        //illegal start or stop received, reset the I2C hardware
        case 0x00: 
            TWI_TRANSFER_ACK();
            TWCR |=(1<<TWIE);
            break;
    }
}




ISR(TWI_vect) {  // Interrupt service routine
    i2cSlaveHandle();
}
/*----------------------------------------------------------------------------------------------------*/










/***************************************** EXAMPLE CODE *****************************************
// This code will set this slaves address as 0x01 Keep on listen (interrupt based tho)
// for master query directed to its own address. Once master makes a query, It receives 
// data from master and sends data to master as well. Finally shows received data on LCD

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "mega16/ext/lcd16x2/lcd.h"
#include "mega16/int/i2c/slave.h"



int main() {
    LCDInit(LS_NONE);
    _delay_ms(100);
    LCDClear();
    LCDWriteStringXY(0,0, "Slave...");
        
    initSlave(0x01); // This is Slave Address

    while(1){
        LCDWriteIntXY(9,0, slave_recv[1],3);
        LCDWriteIntXY(0,1, slave_recv[2],3);
        LCDWriteIntXY(9,1, slave_recv[3],3);
    }
}
-----------------------------------------------------------------------------------------------*/
