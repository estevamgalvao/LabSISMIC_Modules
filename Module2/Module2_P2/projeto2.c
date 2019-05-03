#include <msp430.h> 
#include <math.h>

/*CCIFG = valor de TAxR se igualou ao CCRy -> o contador contou até o parâmetro do
                                              respectivo canal*/
//CIFG  = valor de TaxR deu overflow -> chegou ao teto de contagem
//CCIE  = habilita a flag de interrupções do canal
//TAIE  = habilita a flag de interrupções do contador

void configTimer(void) {

    TA0CTL = TASSEL__SMCLK | MC__CONTINUOUS | TACLR;
    TA0CCTL1 = 0x4000 | CAP;

}


void waitDelay(unsigned int microDelay) {

    TA4CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA4CCR0 = microDelay;

    while(!(TA4CCTL0 & CCIFG));
    TA4CCTL0 &= ~CCIFG;

}

void configLed(void) {

    P1DIR |= BIT0 | BIT1;
    P1OUT &= ~(BIT0 | BIT1);

}


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

	volatile unsigned int time;

	configLed();


	P5DIR &= ~BIT2;             //P5.2 será uma entrada
	P3DIR |= BIT3;              //P3.3 será uma saída - enviará o sinal ao HC-SR04

	P3OUT |= BIT3;              //começo a enviar o sinal ultrassom
	waitDelay(100);             //continuo enviando por 100 micro segundos
	P3DIR &= ~BIT3;             //paro de enviar o sinal ultrassom

    while(!(P5IN & BIT2));      //espero meu pino conectado ao echo receber um sinal
    configTimer();              //começo a contar o tempo que o som vai demorar pra retornar
    while(P5IN & BIT2);

    time = TA0R;             //guardo quanto tempo levou em microsegundos

    unsigned int distance = 0.034 * time; //tá dando menor que 5

    //distance += 15;   assim que eu descobri que tá dando menor que 5

    if (distance < 20) {
        P1OUT |= BIT0;               //ligo led vermelho
        while(1){
                   waitDelay(64000);
                   P1OUT = P1OUT ^ BIT1;
               }
    }
    else if (distance >= 20 || distance <= 40) {
        P1OUT |= BIT1;              //ligo led verde
        while(1);
    }
    else if (distance > 40) {
        P1OUT |= BIT0;              //ligo led vermelho
        P1OUT |= BIT1;              //ligo led verde
        while(1);
    }
    //P1OUT &= ~BIT0;              //desligo o led vermelho



	return 0;
}
