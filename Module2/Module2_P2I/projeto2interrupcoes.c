#include <msp430.h> 

void main(void){

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    //Configurar pinos que serão usados:
    //  P5.5 - BOTÃO DIREITO    - ENTRADA PULL UP
    //  P1.0 - LED VERMELHO     - SAÍDA APAGADO
    //  P1.1 - LED VERDE        - SAÍDA APAGADO
    //  P3.3 - TRIGGER SAÍDA    - SAÍDA "ENVIANDO 0"
    //  P1.3 - ECHO ENTRADA     - ENTRADA -> sem pull up pois o sensor envia corretamente 0's e 1's

    P5DIR &= ~BIT5
    P5REN |= BIT5
    P5OUT |= BIT5

    P1DIR |= (BIT0 | BIT1)
    P1OUT &= ~(BIT0 | BIT1)

    P3DIR |= BIT3

    P1DIR &= ~BIT3

    //Quero que o sensor meça a distância após o botão ser pressionado
    //Vou precisar configurar um timer no modo de captura, no caso, o timer ligado ao pino P1.3
    //Antes de configurar o timer, tenho que fazer a portabilidade do pino para o timer

    //Para fazer a portabilidade do pino, preciso selecionar qual dos periféricos conectados ao P1.3 irei usar utilizando o PxSEL
    //No Datasheet, página 87, encontramos que o TA1.2, no modo de saída, onde é controlado pelo CCI2A, faz P1SEL = 01
    //Então fazemos:

    P1SEL0 |= BIT3      // BIT3 pois é o P1.*3*
    P1SEL1 &= ~BIT3

    //Agora, quando o P1.3 receber um flanco, algo acontecerá com o CCIFG do TA1.2 dependendo do modo que o configurarmos
    //datasheet PAG 77 P1.3 TA1.2 = CCI2A
    //datasheet PAG 87 P1.3 TA1.2 MODO SAÍDA (CCI2A) -> P1SEL0 = 1 P1SEL1 = 0





}
