#include <msp430.h> 
#include <stdint.h>
#include "i2c.h"

void i2cConfigB1 (uint8_t isMaster,uint8_t ownAddr,uint32_t baudrate_kHz) {
    UCB1CTL1 = UCSWRST;

    P5SEL0 |= BIT0;             // CONFIGURA UCB1SDA
    P5SEL1 &= ~BIT0;
    P5SEL0 |= BIT1;             // CONFIGURA UCB1SCL
    P5SEL1 &= ~BIT1;
    P5DIR &= ~(BIT0|BIT1);
    P5REN |= (BIT0|BIT1);       // RESISTORES DE PULL-UP
    P5OUT |= (BIT0|BIT1);

    UCB1CTLW0 |= UCMODE_3|UCSYNC|UCSSEL__SMCLK;
    if(isMaster){
        UCB1CTLW0 |= UCMST;
    }
    UCB1BRW = 1000/baudrate_kHz;
    UCB1I2COA0 = ownAddr | UCOAEN;
    UCB1CTL1 &= ~UCSWRST;

}
void i2cConfigB2 (uint8_t isMaster,uint8_t ownAddr,uint32_t baudrate_kHz) {
    UCB2CTL1 = UCSWRST;

    P7SEL0 |= BIT0;             // CONFIGURA UCB2SDA
    P7SEL1 &= ~BIT0;
    P7SEL0 |= BIT1;             // CONFIGURA UCB2SCL
    P7SEL1 &= ~BIT1;
    P7DIR &= ~(BIT0|BIT1);
    P7REN |= (BIT0|BIT1);       // RESISTORES DE PULL-UP
    P7OUT |= (BIT0|BIT1);

    UCB2CTLW0 |= UCMODE_3|UCSYNC|UCSSEL__SMCLK;
    if(isMaster){
        UCB2CTLW0 |= UCMST;
    }
    UCB2BRW = 1000/baudrate_kHz;
    UCB2I2COA0 = ownAddr | UCOAEN;
    UCB2CTL1 &= ~UCSWRST;
}

uint8_t i2cWriteByte (uint8_t slaveAddr,uint8_t data) {
    UCB2CTLW0 |= UCTR;

    UCB2I2CSA = slaveAddr;
    UCB2CTLW0 |= UCTXSTT;

    while(!(UCB2IFG & UCTXIFG));
    UCB2TXBUF = data;

    while(!(UCB2IFG & UCTXIFG) && !(UCB2IFG & UCNACKIFG));
    if(UCB2IFG & UCNACKIFG){
        UCB2CTLW0 |= UCTXSTP;
        while(UCB2CTLW0 & UCTXSTP);
        return 1;
    }

    while (!(UCB2IFG & UCTXIFG));
    UCB2CTLW0 |= UCTXSTP;
    UCB2IFG &= ~UCTXIFG;
    while(UCB2CTLW0 & UCTXSTP);

    return 0;

}

uint8_t i2cReadByte(uint8_t slaveAddr,uint8_t dataRead){
    UCB2IFG &= ~UCNACKIFG;
    UCB2CTLW0 &= ~UCTR;
    UCB2I2CSA = slaveAddr;

    UCB2CTLW0 |= UCTXSTT;

    //while(!(UCB2IFG & UCRXIFG) && !(UCB2IFG & UCNACKIFG));
    while(UCB2CTLW0 & UCTXSTT);
    if(UCB2IFG & UCNACKIFG){
            UCB2CTLW0 |= UCTXSTP;
            while(UCB2CTLW0 & UCTXSTP);
            return 1;
    }

    UCB2CTLW0 |= UCTXSTP;
    while (!(UCB2IFG & UCRXIFG));
    dataRead = UCB2RXBUF;
    while(UCB2CTLW0 & UCTXSTP);
    return dataRead;
}

