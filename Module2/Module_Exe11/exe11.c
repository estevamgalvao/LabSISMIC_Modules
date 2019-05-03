#include <msp430.h> 


//#define TA0_CCRN_INT 0x01



/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

	TA0CTL = TASSEL__ACLK | MC__UP | TACLR | TAIE;
	__enable_interrupt();

	P1DIR |= BIT0 | BIT1;
	P1OUT |= BIT0 | BIT1;

	TA0CCR1 = 32768;

    #pragma vector = TIMER0_AO_VECTOR;
	__interrupt void timerA0 (void) {
	    switch (TA0IV) {
	        case 0x02:
	            P1OUT ^= BIT0;

	    }
	}


	return 0;
}
