#include <msp430.h> 
#include <msp430.h>
#include <stdint.h>
#include "lib/clock.h"
#include "lib/i2c.h"
#include "lib/lcd.h"
#include "lib/adc12.h"

uint16_t vetor1[16];
uint16_t vetor2[16];
uint8_t posicao= 0;
uint8_t atualizaDisplay = 0;


void waitDelayACLK(unsigned int microDelay) {

    TA0CTL = TASSEL__ACLK | MC__UP | TACLR;
    TA0CCR1 = microDelay;

    while(!(TA0CCTL1 & CCIFG));
    TA0CCTL1 &= ~CCIFG;

    ADC12CTL0 |= ADC12ENC;
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;

}


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    clockInit();

    i2cConfigB2(MASTER,0x12,100);
    lcdInit();

    initAdc12();

    timer();

    __enable_interrupt();

    float mediaVetor1;
    float mediaVetor2;
    volatile uint16_t inteiro,decimal,centesimal,milesimal, hex1, hex2, aux;


    while(1) {
        while(!(atualizaDisplay));

        atualizaDisplay=0;
        lcdWriteByte (0x01,INSTR);
        waitDelayACLK(16384);

        mediaVetor1 = (vetor1[0]+vetor1[1]+vetor1[2]+vetor1[3]+vetor1[4]+vetor1[5]+vetor1[6]+vetor1[7]+vetor1[8]+vetor1[9]+vetor1[10]+vetor1[11]+vetor1[12]+vetor1[13]+vetor1[14]+vetor1[15])/16;
        mediaVetor2 = (vetor2[0]+vetor2[1]+vetor2[2]+vetor2[3]+vetor2[4]+vetor2[5]+vetor2[6]+vetor2[7]+vetor2[8]+vetor2[9]+vetor2[10]+vetor2[11]+vetor2[12]+vetor2[13]+vetor2[14]+vetor2[15])/16;

        inteiro = (int) mediaVetor1;
        decimal = (int) ((mediaVetor1 - inteiro)*10);
        centesimal = (int) (((mediaVetor1 - inteiro)*10-decimal)*10);
        milesimal = (int) ((((mediaVetor1 - inteiro)*10-decimal)*10-centesimal)*10);

        inteiro+=0x30;
        decimal+=0x30;
        centesimal+=0x30;
        milesimal+=0x30;

        lcdWriteByte(inteiro,CHAR);
        lcdWriteByte(0x2C,CHAR);
        lcdWriteByte(decimal,CHAR);
        lcdWriteByte(centesimal,CHAR);
        lcdWriteByte(milesimal,CHAR);
        lcdWriteByte(0x20, CHAR);


        lcdWriteByte (0xFF,CHAR); //verificar ifs após conversão

    }
}

#pragma vector = ADC12_VECTOR
__interrupt void ADCISR() {
    switch(ADC12IV){
    case 0x12:
        vetor1[posicao] = ADC12MEM2;
        vetor2[posicao] = ADC12MEM3;
        if(posicao++ == 15){
            atualizaDisplay = 1;
            posicao = 0;
        }
        break;
    }
}
