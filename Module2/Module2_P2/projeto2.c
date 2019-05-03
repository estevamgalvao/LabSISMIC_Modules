#include <msp430.h> 
#include <stdlib.h>
#include <stdio.h>

/*CCIFG = valor de TAxR se igualou ao CCRy -> o contador contou até o parâmetro do
                                              respectivo canal*/
//CIFG  = valor de TaxR deu overflow -> chegou ao teto de contagem
//CCIE  = habilita a flag de interrupções do canal
//TAIE  = habilita a flag de interrupções do contador

void configTimer(void) {

    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA0CCTL1 = 0x4000 | CAP;

}


void waitDelayUs(unsigned int delay) {

    TA4CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA4CCR0 = delay;

    while(!(TA4CCTL0 & CCIFG));
    TA4CCTL0 &= ~CCIFG;

}

void configLed(void) {

    P1DIR |= BIT0 | BIT1;
    P1OUT &= ~BIT0;
    P1OUT &= ~BIT1;

}


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode



	unsigned int time;

	configLed();


	P5DIR &= ~BIT2;           //P5.2 será uma entrada
	P5REN |= BIT2;               //ativo as resistências
	P5OUT &= ~BIT2;               //pulldown - reajo a uma entrada em 1

	P3DIR |= BIT3;              //P3.3 será uma saída - enviará o sinal ao HC-SR04

	P1OUT |= BIT1;

	P3OUT |= BIT3;              //começo a enviar o sinal ultrassom
	configTimer();              //começo a contar o tempo que o som vai demorar pra retornar
	waitDelayUs(100);           //continuo enviando por 100 micro segundos
	P3DIR &= ~BIT3;              //paro de enviar o sinal ultrassom

    P1OUT |= BIT0;

    while(!(P5IN & BIT2));      //espero meu pino conectado ao echo receber um sinal
	TA0CCTL1 |= CCIFG;           //paro a contagem do tempo

	time = TA0CCR1;                //guardo quanto tempo levou




	printf("%d microsegundos", time);
	
	return 0;
}
