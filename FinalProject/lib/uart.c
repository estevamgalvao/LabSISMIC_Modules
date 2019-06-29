#include <msp430.h> 
#include <stdint.h>
#include "uart.h"

void UART_config (uint32_t baudrate, uint8_t bitOrder, uint8_t parity, uint8_t nStops, uint8_t cLength) {
	//UCA3CTLW0 PÁG 784 userGuide
	
    UCA3CTLW0 = UCSWRST;	            //Zero completamente o registrador e levanto apenas o bit UCSWRST
	                                    //para garantir que o registrador irá começar de um estado que eu
	                                    //conheço e habilito o modo de configuração

	switch(parity) {                    //Crio um switch para as configurações de paridade do registrador (PAR, IMPAR, SEM)
	case ODD:                           //PARIDADE PAR
		UCA3CTLW0 |= UCPEN;
		UCA3CTLW0 &= ~UCPAR;
		break;			
	case EVEN:                          //PARIDADE ÍMPAR
		UCA3CTLW0 |= UCPEN| UCPAR;
		break;
	case WITHOUT:                       //SEM PARIDADE
		UCA3CTLW0 &= ~UCPEN;
		break;
	default: break; 
	}

	if (bitOrder) {                     //Verifico qual a ordem de envio de bits que o usuário deseja, MSB first ou LSB first
	    UCA3CTLW0 |= UCMSB;             //MSB first
	}
	else {
	    UCA3CTLW0 &= ~UCMSB;            //LSB first
	}

	if (cLength) {                      //Verifico o tamanho dos dados que o usuário deseja enviar
		UCA3CTLW0 |= UC7BIT;            //7 BITS
	}
	else {
		UCA3CTLW0 &= ~UC7BIT;           //8 BITS
	}

	if (nStops) {                       //Verifico o número de STOP que o usuário deseja configurar na comunicação (1 ou 2)
	    UCA3CTLW0 |= UCSPB;             //2 STOP
	}
	else {
	    UCA3CTLW0 &= ~UCSPB;            //1 STOP
	}


	UCA3CTLW0 |= UCMODE_0;              //Seleciono o UART mode do periférico
	UCA3CTLW0 &= ~UCSYNC;               //Seleciono o modo asíncrono do periférico, afinal estou trabalhando com uma comunicação UART

	UCA3CTLW0 |= UCSSEL_3;              //Sempre seleciono o SMCLK pois tem a melhor combinação com os baudrates suportados pelo dispositivo HC-05

	uint32_t brDiv = (1000000  << 8)/baudrate; //descolo em 8bits (* 2^8) o 10^6 e faço a divisão pelo baudrate
	                                           //dessa forma, é como se eu fizesse o número 104,16 em binário ficar 1041,6
	UCA3BRW = brDiv >> 8;                      //então eu salvo a parte inteira dessa divisão, dividindo por 2^3, em BRW
	UCA3MCTLW = (brDiv & 0xFF) << 8;           //e finalmente, salvo os 3 bits menos significativos (nesse exemplo seriam o 1 = 001) em MCTLW
	                                           //Desloco 8 bits pois UCBRS, na MSP430FR5994, fica entre os bits 15-8 // PÁG 786 userGuide

	//Portabilidade dos pinos 6.1 e 6.0 // PAG 107 datasheet
	//PINO 6.0 UCA3TXD
	//PINO 6.1 UCA3RXD
	P6SEL0 |= (BIT0|BIT1); 
	P6SEL1 &= ~(BIT0|BIT1);

	UCA3CTLW0 &= ~UCSWRST;                     //Desabilito o modo de configuração do registrador
	
	UCA3IE |= UCRXIE;               	  //Habilito as interrupções dos buffers de transmissão e recepção

}
