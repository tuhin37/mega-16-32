/**************************************** DESCRIPTION ****************************************
This library is used to connect Two or more I2C AVR MCU(s) together on same I2C bus.
Currently the This library supports Single Master situation. This Perticular library 
is used to configure a Master. There can be multiple slaves on the bus.


Essentially there are two Buffers namely
  master_recv[]
  master_tran[]

Once "i2cMasterHandle(deviceAddress)" function is called  with a perticular device address
It communicates with the specified device. Sends all the bytes present in master_tran[] buffer
and receives whatever slave device sends and stores it in master_recv[] buffer.

The function returns '0' if the exchange is unsuccessful and '1' when exchange is successful


NOTE:
  First SCL frequency will have to be set like this: initTWI(0, 10); => TWBR = 10; TWPS = 0;
  The Slave one the other end must run the "slave.h".
--------------------------------------------------------------------------------------------*/


/***************************************** USER CONFIGURABLES *****************************************/
#define MASTER_BUFLEN_RECV 3
#define MASTER_BUFLEN_TRAN 12
/*----------------------------------------------------------------------------------------------------*/





/************************************* GLOBAL SECTION *************************************/
uint8_t master_r_index =0;
uint8_t master_recv[MASTER_BUFLEN_RECV]; //buffer to store received bytes

uint8_t master_t_index=0;
uint8_t master_tran[MASTER_BUFLEN_TRAN]= {1,2,3,4,5,6,7,8,9,10,11,12}; // this buffer is sent
/*-----------------------------------------------------------------------------------------*/




/**************************************** DEPENDENCY ****************************************/
#ifndef I2C
    #include "i2c.h"
#endif
/*------------------------------------------------------------------------------------------*/





/************************************** USER FUNCTIONS **************************************/
uint8_t i2cMasterHandle(uint8_t devAddress) {
    uint8_t status;


    // first send start condiiton-------------
    status = TWI_START();
    if(status == TW_START || status == TW_REP_START) { // if start was successful
        //reset buffer indices
        master_t_index =0;
        master_r_index =0;
        
        // now send SLA+W--------------------
        TWDR = (devAddress<<1) | TW_WRITE;
        TWI_TRANSFER_ACK();
        TWI_WAIT_TILL_DONE;
    }
    else return 0; // sending start failed




    if(TWI_STATUS == TW_MT_SLA_ACK) { // if slave ACKed to SLA+W
        // send first byte from master_trans[] buffer
        TWDR = master_tran[0];
        master_t_index=1;
        TWI_TRANSFER_ACK();// send first byte -------------
        TWI_WAIT_TILL_DONE;
    }
    else return 0; // SLA+W Did not ACKed 

    while(master_t_index < MASTER_BUFLEN_TRAN) {
        // if Slave ACKed byte sent
        if(TWI_STATUS == TW_MT_DATA_ACK) {
            TWDR =  master_tran[master_t_index];
            master_t_index++;
            TWI_TRANSFER_ACK();
            TWI_WAIT_TILL_DONE;
        }
        else return 0; // slave did not ACK data bytes sent by master
    }
    // last Byte sent



    // if Slave ACKed last byte sent
    if(TWI_STATUS == TW_MT_DATA_ACK) {
        // Send Restart Condition
        status=TWI_RESTART();
    }
    else return 0; // slave did not ACK last byte


    if(status == TW_REP_START) { // if REPEAT START was successful
        
        TWDR = (devAddress<<1) | TW_READ;
        TWI_TRANSFER_ACK(); // Send SLA+R  ----
        TWI_WAIT_TILL_DONE;
    }
    else return 0; // REPEAT START was not successful



    if(TWI_STATUS == TW_MR_SLA_ACK) { // if SLA+R was ACKed
        TWI_TRANSFER_ACK(); // do nothing and now data will come from slave
        TWI_WAIT_TILL_DONE;
    }
    else return 0; // SLA+R was not ACKed 

    while(master_r_index < MASTER_BUFLEN_RECV) {
        if(TWI_STATUS == TW_MR_DATA_ACK) { // if a data byte received and ACK sent to slave
            //store data
            master_recv[master_r_index] = TWDR;
            master_r_index++;

            if(master_r_index < MASTER_BUFLEN_RECV){
                TWI_TRANSFER_ACK();
            }
            //otherwise NACK the next byte
            else {
                TWI_TRANSFER();
                master_r_index =MASTER_BUFLEN_RECV;
            }
            TWI_WAIT_TILL_DONE;
        }
    }// Last data byte has been received



    if(TWI_STATUS == TW_MR_DATA_NACK) { // If after Last byte Received ACK was not sent (NACK sent) 
        TWI_STOP();
        TWCR &= ~(TWEN);
    }
  
    // all is successfull
    return 1;
} 
/*-------------------------------------------------------------------------------------------*/






/***************************** EXAMPLE CODDE *****************************
// This Example makes a query to slave device "0x01" and makes
// data exchange. Finally shows received data on LCD

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#include "mega16/ext/lcd16x2/lcd.h"
#include "mega16/int/i2c/master.h"


int main(){
    
    LCDInit(LS_NONE);
    _delay_ms(100);
    LCDClear();
    LCDWriteStringXY(0,0, "Master");
    
    _delay_ms(1000);

    // set SCL Frequency ( reffer SCL Frequency table in the same folder as i2c.h)
    initTWI(0, 10);

    //Completes one I2C exchange
    i2cMasterHandle(0x01);
    

    while(1){  
        LCDWriteIntXY(8,0,master_recv[0],3);
        LCDWriteIntXY(0,1,master_recv[1],3);
        LCDWriteIntXY(8,1,master_recv[2],3);
    }
}
-------------------------------------------------------------------------*/
