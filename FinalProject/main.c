#include <msp430.h>
#include "lib/uart.h"

uint8_t password[] = {0x41, 0x42, 0x43, 0x44};
uint8_t counter = 0;
/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

//    uint8_t try[4];

	//Configurar pinos dos leds
	//Configurar UART ->UCA3 6.1 RX 6.0 TX
	P1DIR |= BIT0 | BIT1;
	P1OUT &= ~(BIT0 | BIT1);

	UART_config(460800, LSB, WITHOUT, 0, BITS8);


	
    __enable_interrupt();   //Habilito o General Interrupt Enable (GIE) no meu Status Register (R2)

    UCA3IE |= (UCRXIE | UCTXIE);

    while(1);

    return 0;
}


#pragma vector=EUSCI_A3_VECTOR
__interrupt void UAC3ISR(void) {
    switch(UCA3IV) {
    case 0x00:
    // Vector 0: No interrupts
    break;
    case 0x02:  // Vector 2: UCRXIFG

        //try[counter] = UCA3RXBUF;
        //data = UCA3RXBUF;

        if (UCA3RXBUF == 0x01) {
            P1OUT ^= BIT0;
            P1OUT &= ~BIT1;

        }
        else if (UCA3RXBUF == 0x42) {
            P1OUT &= ~BIT0;
            P1OUT |= BIT1;

        }
        else if (UCA3RXBUF == 0x43) {
            P1OUT |= BIT0;
            P1OUT &= ~BIT1;

        }
        else {
            P1OUT |= BIT0;
            P1OUT |= BIT1;

        }


        /*
        if (UCA3RXBUF != password[counter]) {
            counter = 0;
            P1OUT ^= BIT0;
        }
        counter++;
        if (counter == 4) {
            P1OUT ^= BIT1;
            counter = 0;
        }
*/

    break;
    case 0x04: // Vector 4: UCTXIFG
    break;
    case 0x06: // Vector 6: UCSTTIFG
    break;
    case 0x08: // Vector 8: UCTXCPTIFG
    break;
    default: break;
    }

}
