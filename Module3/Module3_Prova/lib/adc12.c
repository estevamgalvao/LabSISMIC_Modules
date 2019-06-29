#include <msp430.h> 
#include <stdint.h>
#include "adc12.h"

void initAdc12(void){
	ADC12CTL0  &= ~ADC12ENC;

	ADC12CTL0 = ADC12SHT0_4 |
				ADC12ON;

	ADC12CTL1 = ADC12CSTARTADD_2 |
	            ADC12SHS_0 |
	            ADC12SHP |
	            ADC12SSEL_0 |
	            ADC12CONSEQ_3;

	ADC12CTL2 = ADC12RES_2;

	ADC12MCTL2 = ADC12INCH_2;

	ADC12MCTL3 = ADC12EOS |
	            ADC12INCH_3;

	ADC12IER0 = ADC12IE3;


}

void timer(void){
    TA0CTL = TACLR | TASSEL__ACLK | MC__UP;
    TA0CCTL0 = CCIE;
    TA0CCR0 = 2048 - 1;
}

