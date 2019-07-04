#include <msp430.h> 
#include <stdint.h>
#include "timer.h"

void configTA1 (uint16_t freq, uint8_t luminosity) {
	TA1CTL = TASSEL__ACLK | MC__UP | TACLR;

	TA1CCR0 = (32768/freq);
	TA1CCR1 = (luminosity*(32768/freq)/100);

	TA1CCTL0 |= CCIE;
	TA1CCTL1 |= CCIE;

}

void updateTA1 (uint16_t freq, uint8_t luminosity) {
	
	TA1CCR0 = (32768/freq);
	TA1CCR1 = (luminosity*(32768/freq)/100);

}
