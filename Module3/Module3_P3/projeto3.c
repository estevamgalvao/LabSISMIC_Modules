#include <msp430.h> 
#include <msp430.h>
#include <stdint.h>
#include "lib/clock.h"
#include "lib/i2c.h"
#include "lib/lcd.h"
#include "lib/adc12.h"

uint16_t vetor1[8];
uint16_t vetor2[8];
uint8_t posicao= 0;
uint8_t atualizaDisplay = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
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

    while(1){
    	while(!(atualizaDisplay));
    	atualizaDisplay=0;
    	lcdWriteByte (0x01,INSTR);
    	mediaVetor1 = (vetor1[0]+vetor1[1]+vetor1[2]+vetor1[3]+vetor1[4]+vetor1[5]+vetor1[6]+vetor1[7])/8;
    	mediaVetor2 = (vetor2[0]+vetor2[1]+vetor2[2]+vetor2[3]+vetor2[4]+vetor2[5]+vetor2[6]+vetor2[7])/8;

    	hex1 = mediaVetor1;
    	hex2 = mediaVetor2;

    	mediaVetor1= mediaVetor1*3.3/4096;
    	mediaVetor2= mediaVetor2*3.3/4096;

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

    	aux = hex1>>8;
    	if (aux>9) {
    	    aux += 0x37;
    	}
    	else {
    	    aux +=0x30;
    	}
    	lcdWriteByte (aux, CHAR);

    	aux = (hex1>>4)&(0x0F);
        if (aux>9) {
            aux += 0x37;
        }
        else {
            aux +=0x30;
        }
        lcdWriteByte (aux, CHAR);

        aux = hex1 & 0x0F;
        if (aux>9) {
            aux += 0x37;
        }
        else {
            aux +=0x30;
        }
        lcdWriteByte (aux, CHAR);

        lcdWriteByte (0xC0, INSTR);

        inteiro = (int) mediaVetor2;
        decimal = (int) ((mediaVetor2 - inteiro)*10);
        centesimal = (int) (((mediaVetor2 - inteiro)*10-decimal)*10);
        milesimal = (int) ((((mediaVetor2 - inteiro)*10-decimal)*10-centesimal)*10);

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

        aux = hex2>>8;
        if (aux>9) {
        aux += 0x37;
        }
        else {
        aux +=0x30;
        }
        lcdWriteByte (aux, CHAR);

        aux = (hex2>>4)&(0x0F);
        if (aux>9) {
        aux += 0x37;
        }
        else {
        aux +=0x30;
        }
        lcdWriteByte (aux, CHAR);

        aux = hex2 & 0x0F;
        if (aux>9) {
        aux += 0x37;
        }
        else {
        aux +=0x30;
        }
        lcdWriteByte (aux, CHAR);

    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR() {
    ADC12CTL0 |= ADC12ENC;
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;
}

#pragma vector = ADC12_VECTOR
__interrupt void ADCISR() {
    switch(ADC12IV){
    case 0x12:
        vetor1[posicao] = ADC12MEM2;
        vetor2[posicao] = ADC12MEM3;
        if(posicao++ == 7){
            atualizaDisplay = 1;
            posicao = 0;
        }
        break;
    }
}


