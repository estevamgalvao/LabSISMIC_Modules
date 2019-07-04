#include <msp430.h>
#include "lib/uart.h"
#include "lib/timer.h"


#define ALTERNATE   1
#define CONTINUOUS  0
#define MODE1HZ     1
#define MODE2HZ     2
#define MODE5HZ     5
#define MODE10HZ    10

uint8_t password[] = {0x41, 0x42, 0x43, 0x44};  //Vetor que administrará a senha para ascender o LED
uint8_t counter = 0;                            //Contador para saber quantos caracteres o usuário acertou
                                                //Dessa forma não preciso de outro vetor para guardar as tentativas do usuário
uint8_t mode = ALTERNATE;

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
    UART_config(9600, LSB, WITHOUT, 0, BITS8);     //Baudrate de 38400bps, LSB first, sem paridade, 1 bit de STOP e 8BITS por comunicação

    __enable_interrupt();                           //Habilito o General Interrupt Enable (GIE) no meu Status Register (R2)

    //UCA3IE |= (UCRXIE);                           //Tive de habilitar as interrupções do RXBUF e TXBUF também na main porque mesmo habilitando
                                                    //na chamada da função UART_config, os bits não estavam sendo "setados"
                                                    // |-> o erro era que só devíamos habilitar a interrupção após zerar a flag de configuração
    configTA1(MODE1HZ, 50);

    while(1);
}


#pragma vector=EUSCI_A3_VECTOR
__interrupt void UAC3ISR(void) {
	// A leitura 
    switch(UCA3RXBUF) {
    case 0x41:
        updateTA1(MODE1HZ, 50);
        break;
    case 0x42:
        updateTA1(MODE2HZ, 50);
        break;
    case 0x43:
        updateTA1(MODE5HZ, 50);
        break;
    case 0x44:
        updateTA1(MODE10HZ, 50);
        break;
    case 0x45:
        TA1CTL &= ~MC__UP;
        P1OUT &= ~(BIT0 | BIT1);
        break;
    case 0x46:
        configTA1(MODE1HZ, 50);
        break;
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
        }
        else {                              //CONTINUO
            P1OUT |= (BIT0 | BIT1);
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
    else {                                   //CONTINUO
        P1OUT &= ~(BIT0 | BIT1);
    }
}
