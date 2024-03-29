#include <msp430.h> 
#include <math.h>

/*CCIFG = valor de TAxR se igualou ao CCRy -> o contador contou até o parâmetro do
                                              respectivo canal*/
//CIFG  = valor de TaxR deu overflow -> chegou ao teto de contagem
//CCIE  = habilita a flag de interrupções do canal
//TAIE  = habilita a flag de interrupções do contador
/*
void configTimer(void) {
    TA0CTL = TASSEL__SMCLK | MC__CONTINUOUS | TACLR;
    TA0CCTL1 = 0x4000 | CAP;
}
*/
void waitDelay(unsigned int microDelay) {

    TA4CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA4CCR0 = microDelay;

    while(!(TA4CCTL0 & CCIFG));
    TA4CCTL0 &= ~CCIFG;

}

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

	volatile unsigned int time;

    P1DIR |= BIT0 | BIT1;
    P1OUT &= ~(BIT0 | BIT1);    //configura as LED

	P5DIR &= ~BIT2;             //P5.2 será uma entrada - receberá o sinal do echo

	//P5REN |= BIT2;
	//P5OUT |= BIT2;

	P3DIR |= BIT3;              //P3.3 será uma saída - enviará o sinal ao HC-SR04
	P3OUT &= ~BIT3;

	while(1) {
        P3OUT |= BIT3;             //começo a enviar o sinal ultrassom
        waitDelay(20);             //continuo enviando por 100 micro segundos
        P3OUT &= ~BIT3;            //paro de enviar o sinal ultrassom

        while(!(P5IN & BIT2));      //espero meu pino conectado ao echo receber um sinal

        TA0CTL = TASSEL__SMCLK | MC__CONTINUOUS | TACLR; //começo a contar

        while(P5IN & BIT2);
        time = TA0R;                //guardo quanto tempo levou em microsegundos para o sinal ir e voltar

        if (time < 1176) {
            P1OUT &= ~BIT1;
            P1OUT |= BIT0;               //ligo led vermelho
            //while(1);
        }
        else if (time >= 1176 && time <= 2351) {
            P1OUT &= ~BIT0;
            P1OUT |= BIT1;              //ligo led verde
            //while(1);
        }
        else {
            P1OUT |= BIT0;              //ligo led vermelho
            P1OUT |= BIT1;              //ligo led verde
            //while(1);
        }

        waitDelay(20000);
	}
}
