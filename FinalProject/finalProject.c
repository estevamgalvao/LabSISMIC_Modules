#include <msp430.h>
#include <stdint.h>
#include "lib/uart.h"
#include "lib/timer.h"


#define ALTERNATE       0xFF
#define SIMULTANEOUS    0x00
#define INCREMENT       0xFF
#define DECREMENT       0x00
#define MODE1HZ         1
#define MODE2HZ         2
#define MODE5HZ         5
#define MODE10HZ        10
#define MODE50HZ        50

uint8_t password[] = {0x41, 0x42, 0x43, 0x44};  //Vetor que administrará a senha para ascender o LED
uint8_t counter = 0;                            //Contador para saber quantos caracteres o usuário acertou
                                                //Dessa forma não preciso de outro vetor para guardar as tentativas do usuário
uint8_t mode  = ALTERNATE;
uint8_t pulse = 0;
uint8_t modePulse = INCREMENT;
uint8_t dutyCycle = 1;
uint8_t step = 1;


/**
 * main.c
 */

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    //Configurar pinos dos leds
    P1DIR |= BIT0 | BIT1;                           //Configuro as LED como saída
    P1OUT &= ~(BIT0 | BIT1);                        //Inicio as LED apagadas
	
    //Configurar UART -> UCA3 6.1 RX 6.0 TX
    UART_config(9600, LSB, WITHOUT, 0, BITS8);     //Baudrate de 9600bps, LSB first, sem paridade, 1 bit de STOP e 8BITS por comunicação

    __enable_interrupt();                           //Habilito o General Interrupt Enable (GIE) no meu Status Register (R2)

    //UCA3IE |= (UCRXIE);                           //Tive de habilitar as interrupções do RXBUF e TXBUF também na main porque mesmo habilitando
                                                    //na chamada da função UART_config, os bits não estavam sendo "setados"
                                                    // |-> o erro era que só devíamos habilitar a interrupção após zerar a flag de configuração
    TA1_config(MODE1HZ, 50);

    while(1);
}


#pragma vector=EUSCI_A3_VECTOR
__interrupt void UAC3ISR(void) {
	// A leitura 
    switch(UCA3RXBUF) {
    case 0x41:
        TA1_update(MODE1HZ, 50);
        break;
    case 0x42:
        TA1_update(MODE2HZ, 50);
        break;
    case 0x43:
        TA1_update(MODE5HZ, 50);
        break;
    case 0x44:
        TA1_update(MODE10HZ, 50);
        break;
    case 0x45:
        TA1CTL &= ~MC__UP;
        P1OUT &= ~(BIT0 | BIT1);
        break;
    case 0x46:
        if (pulse) {
            pulse = !pulse;
            TA1_config(MODE1HZ, 50);
        }
        else {
            pulse = !pulse;
            //TA1_update(MODE50HZ, 1);
            TA1CCR0 = 665;
            TA1CCR1 = 6;
        }
    case 0x47:
        TA1_config(MODE1HZ, 50);
        break;
    case 0x48:
        mode = !mode;
    default: break;
    }
/*
	if (UCA3RXBUF != password[counter]) {   //Se a enésima tentativa (counter) do usuário está incorreta, se estiver, reinicio a contagem
	    counter = 0;                        //e dou toggle no LED vermelho indicando o erro
	    //P1OUT ^= BIT0;
	}
	else {
	    counter++;                          //Se o byte enviado foi correto, incremento o counter indicando que +1 byte foi acertado
	}
	if (counter == 4) {                     //No momento em que o 4º byte correto for lido, counter será = 4 e então devo ascender o LED verde
	    //P1OUT ^= BIT1;                      //indicando que a senha correta foi inserida. Após isso zero o counter para receber uma nova tentativa
	    counter = 0;
	}
*/
}


#pragma vector=TIMER1_A1_VECTOR
__interrupt void TA1_ISR (void) {
    switch(TA1IV) {
    case 0x02:
        //caso entre nesse case, significa que o timer contou até CCR1


        if (mode) {                         //ALTERNADO
            P1OUT |= BIT0;
            P1OUT &= ~BIT1;
            if(pulse) {
                if (modePulse){             //INCREMENTAR
                    if ( ( ( (TA1CCR0) / 100) + TA1CCR1) < TA1CCR0) {
                        TA1CCR1 += (TA1CCR0)/100;
                        //TA1_update(MODE50HZ, dutyCycle + step);
                    }
                    else {
                        modePulse = !modePulse;
                    }
                }
                else {
                    if ( ( (int16_t *) ( (TA1CCR1 - ( (TA1CCR0)/100) ) ) > 0) &&
                         ( (int16_t *) ( (TA1CCR1 - ( (TA1CCR0)/100) ) ) < (int16_t *) (TA1CCR0) ) ) {

                        TA1CCR1 -= (TA1CCR0)/100;
                       // TA1_update(MODE50HZ, dutyCycle - step);
                     }
                     else {
                         modePulse = !modePulse;
                     }
                 }
            }
        }
        else {                              //SIMULTANEO
            P1OUT |= (BIT0 | BIT1);
            if(pulse) {
                if (modePulse){             //INCREMENTAR

                    if ( ( ( (TA1CCR0) / 100) + TA1CCR1) < TA1CCR0) {
                        TA1CCR1 += (TA1CCR0)/100;
                        //TA1_update(MODE50HZ, dutyCycle + step);
                    }
                    else {
                        modePulse = !modePulse;
                    }
                }
                else {

                    if ( ( (int16_t *) ( (TA1CCR1 - ( (TA1CCR0)/100) ) ) > 0) &&
                         ( (int16_t *) ( (TA1CCR1 - ( (TA1CCR0)/100) ) ) < (int16_t *) (TA1CCR0) ) ) {

                        TA1CCR1 -= (TA1CCR0)/100;
                        //TA1_update(MODE50HZ, dutyCycle - step);
                     }
                     else {
                         modePulse = !modePulse;
                     }
                 }
            }
        }
        break;


    default:
        break;
    }
}


#pragma vector=TIMER1_A0_VECTOR
__interrupt void TA1C0_ISR (void) {
    //caso eu entre nessa interrupção, significa que o timer contou até CCR0
    if (mode) {                              //ALTERNADO
        P1OUT &= ~BIT0;
        P1OUT |= BIT1;
    }
    else {                                   //SIMULTANEO
        P1OUT &= ~(BIT0 | BIT1);
    }
}
