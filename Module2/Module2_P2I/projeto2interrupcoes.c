#include <msp430.h> 
#include "pins.h"
#include "msp_timers.h"

volatile unsigned int time1;



void waitDelay(unsigned int microDelay) {

    TA4CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA4CCR0 = microDelay;

    while(!(TA4CCTL0 & CCIFG));
    TA4CCTL0 &= ~CCIFG;

}


void main(void){

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    setPin(P3_3, OUTPUT);       //P3.3 será a saída da placa -> entrada no sensor TRIGGER
    setPin(P1_3, INPUT);        //P1.3 será uma entrada na placa -> saída no sensor ECHO
                                //O P1.3 foi escolhido porque ele está ligado a um dos timers que utilizarei para realizar as capturas - TA1.2
    SET_REG(P1SEL0, BIT3);      //Irei fazer a portabilidade do pino 1.3 para o timer. Isso é feito com os P1SEL0 E P1SEL1
    CLEAR_REG(P1SEL1, BIT3);    //para cada pino com portabilidade, os "periféricos" que são portáveis para ele, ou seja, que se comunicam com ele "naturalmente"
                                //devem ser selecionados atraveś dos P1SEL0 e P1SEL1

    writePin(P1_0, LOW);        //digo que as LED's devem começar apagadas
    writePin(P1_1, LOW);        //^


    setPin(P1_0, OUTPUT);       //defino as LED's como saída
    setPin(P1_1, OUTPUT);

    writePin(P3_3, LOW);        //digo que o TRIGGER deve começar sem enviar nenhum sinal

    setPin(P5_6, INPUT_PULL_UP);    //habilito o funcionamento do switch - "botão" - entrada com resistores de pull up

//    setInterrupt(P5_6, HIGH_TO_LOW);

    P5IES |= BIT6;              //detectar o flanco de descida -> botão pressionado vai pra 0
                                //Port x interrupt edge select
                                //0b = PxIFG flag is set with a low-to-high transition
                                //1b = PxIFG flag is set with a high-to-low transition
    P5IE |= BIT6;               //habilitar interrupção do botão


	

   __enable_interrupt();
/*
    while(1) {
        while(!(P5IN & BIT6)) {
             writePin(P1_0, HIGH);
             waitDelay(60000);
             writePin(P1_0, LOW);
             waitDelay(60000);
            }
        writePin(P1_0, LOW);

    }

*/


}


#pragma vector=PORT5_VECTOR
__interrupt void P5_ISR (void) {
    switch(P5IV) {
    case 0x00: break;
    case 0x02: break;
    case 0x04: break;
    case 0x06: break;
    case 0x08: break;
    case 0x0A: break;
    case 0x0C: break;
    case 0x0E:
        //P5IE &= ~BIT6;

        waitDelay(1000);
        //writePin(P1_0, HIGH);

        P1OUT ^= BIT0;
        waitDelay(60000);
        P1OUT ^= BIT0;
        //writePin(P1_1, HIGH);


/*
        writePin(P3_3, HIGH);
        waitDelay(20);
        writePin(P3_3, LOW);
        TA1CTL = TASSEL__SMCLK | MC_2 | TACLR; //seleciono o SM clock, coloco em modo continuo porque quero apenas
                                               //capturar o tempo
        TA1CCTL2 = CM__BOTH | CCIS_0 | CAP | CCIE_1; //modo BOTH pois quero detectar tanto flancos de subida quanto flancos de descida
                                                    //CCIS funciona pra identificar ao timer qual dispositivo irá acionar a captura -datasheet-
*/
    case 0x10: break;
    }
}



#pragma vector=TIMER1_A1_VECTOR
__interrupt void TA1_ISR (void) {
    switch(TA1IV) {
    case 0x00: break;
    case 0x02: break;
    case 0x04:
        if (TA1CCTL2 & CCI) {
            time1 = TA1CCR2;
        }
        else { //CCI (bit3) me diz se foi um flanco de subida ou descida -> 1 é subida 0 é descida
            time1 = time1 - TA1CCR2;
            if (time1 < 1176) {
                P1OUT &= ~BIT1;
                P1OUT |= BIT0;               //ligo led vermelho
                //while(1);
            }
            else if (time1 >= 1176 && time1 <= 2351) {
                P1OUT &= ~BIT0;
                P1OUT |= BIT1;              //ligo led verde
                //while(1);
            }
            else {
                P1OUT |= BIT0;              //ligo led vermelho
                P1OUT |= BIT1;              //ligo led verde
                //while(1);
            }
        }
        break;
    case 0x06: break;
    case 0x08: break;
    case 0x0A: break;
    case 0x0C: break;
    case 0x0E: break;
    case 0x10: break;
    }
    P5IE |= BIT6;
}
