#include <msp430.h> 
#include <stdint.h>
#include "uart.h"

void UART_config (uint32_t baudrate, uint8_t bitOrder, uint8_t parity, uint8_t nStops, uint8_t cLength) {
	
	UCA3CTLW0 = UCSWRST;	

	switch(parity) {
	case ODD:
		UCA3CTLW0 |= UCPEN;
		UCA3CTLW0 &= ~UCPAR;
		break;			
	case EVEN:
		UCA3CTLW0 |= UCPEN| UCPAR;
		break;
	case WITHOUT:
		UCA3CTLW0 &= ~UCPEN;
		break;
	default: break; 
	}

	if (bitOrder) {
	    UCA3CTLW0 |= UCMSB;
	}
	else {
	    UCA3CTLW0 &= ~UCMSB;
	}

	if (cLength) {
		UCA3CTLW0 |= UC7BIT; //7 BITS
	}
	else {
		UCA3CTLW0 &= ~UC7BIT; //8 BITS
	}

	if (nStops) {
	    UCA3CTLW0 |= UCSPB;
	}
	else {
	    UCA3CTLW0 &= ~UCSPB;
	}


	UCA3CTLW0 |= UCMODE_0;
	UCA3CTLW0 &= ~UCSYNC;

	//Criar função que estima o melhor baudrate

	UCA3CTLW0 |= UCSSEL_3;
	uint32_t brDiv = (1000000  << 3)/baudrate; //descolo em 3bits (* 2^3) o 10^6 e faço a divisão pelo baudrate
	                                           //dessa forma, é como se eu fizesse o número 104,16 em binário ficar 1041,6
	UCA3BRW = brDiv >> 3;                      //então eu salvo a parte inteira dessa divisão, dividindo por 2^3, em BRW
	UCA3MCTLW = (brDiv & 0x07) << 8;           //e finalmente, salvo os 3 bits menos significativos (nesse exemplo seriam o 1 = 001) em MCTLW

	UCA3IE |= (UCRXIE | UCTXIE);

	//Portabilidade do pino PAG 107 datasheet
	P6SEL0 |= (BIT0|BIT1); 
	P6SEL1 &= ~(BIT0|BIT1);


	UCA3CTLW0 &= ~UCSWRST;
}
