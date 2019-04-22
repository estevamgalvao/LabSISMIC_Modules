#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    P1DIR = P1DIR | BIT0;
    P1OUT = P1OUT & ~BIT0;

    volatile unsigned int delay;

    while(1){
        for (delay = 30000; delay > 0; delay--) {}
        P1OUT = P1OUT ^ BIT0;
    }
	return 0;
}
