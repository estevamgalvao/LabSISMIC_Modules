#include "lcd.h"
#include <msp430.h> 
#include <stdint.h>
#include "i2c.h"

void delay(volatile uint16_t tempo){
    while(tempo--);
}


void lcdWriteNibble(uint8_t nibble,uint8_t rs){
    i2cWriteByte(LCD_ADDR,nibble<<4|BT| 0|rs);
    delay(5000);
    i2cWriteByte(LCD_ADDR,nibble<<4|BT|EN|rs);
    delay(5000);
    i2cWriteByte(LCD_ADDR,nibble<<4|BT| 0|rs);
}

void lcdWriteByte(uint8_t byte, uint8_t rs){

    lcdWriteNibble(byte >> 4,rs);
    lcdWriteNibble(byte,rs);

    //while( lcdReadByte(RS) & BIT0 );
}

void lcdInit(void){
    lcdWriteNibble(0x03,INSTR); //inicializacao do LCD
    delay(5000);
    lcdWriteNibble(0x03,INSTR);
    delay(5000);
    lcdWriteNibble(0x03,INSTR);
    delay(5000);
    lcdWriteNibble(0x02,INSTR);
    delay(5000);
    lcdWriteByte(0x28,INSTR);
    delay(5000);
    lcdWriteByte(0x08,INSTR);
    delay(5000);
    lcdWriteByte(0x06,INSTR);
    delay(5000);
    lcdWriteByte(0x0F,INSTR);
    delay(5000);
    lcdWriteByte(0x01,INSTR);
    delay(5000);
}


