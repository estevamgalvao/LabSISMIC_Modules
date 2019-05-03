#include <msp430.h> 


/**
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    P1DIR = P1DIR | BIT0;
    P1OUT = P1OUT ~& BIT0;

	TA0CTL = TASSEL__ACLK | MC__UP | TACLR;

	TA0CCR0 = (32768/2);

	while(1) {

	    while(!(TA0CCTL0 & CCIFG));
	    TA0CCTL0 &= ~CCIFG;
	    P1OUT = P1OUT ^ BIT0;
	    //TA0CTL = MC__STOP;

	}

}
