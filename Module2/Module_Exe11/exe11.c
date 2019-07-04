#include <msp430.h> 
#include <stdint.h>

//#define TA0_CCRN_INT 0x01

#define ALTERNATE   0xFF
#define CONTINUOUS  0x00
#define INCREMENT   0xFF
#define DECREMENT   0x00
#define MODE1HZ     1
#define MODE2HZ     2
#define MODE5HZ     5
#define MODE10HZ    10
#define MODE50HZ    50
#define SMCLK       0x0200
#define ACLK        0x0100

uint8_t modeBlink = CONTINUOUS;
uint8_t modePulse = INCREMENT;
uint8_t pulse = 1;

uint8_t dutyCycle;
uint8_t bright;





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

	configTA1(MODE50HZ, 10);

    __enable_interrupt();                           //Habilito o General Interrupt Enable (GIE) no meu Status Register (R2)


	while(1);



}




#pragma vector=TIMER1_A1_VECTOR
__interrupt void TA1_ISR (void) {
    switch(TA1IV) {
    case 0x02:
        //caso entre nesse case, significa que o timer contou até CCR1
        if (pulse) {
            P1OUT |= BIT1;
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


















            /*
            if (modePulse){ //INCREMENTAR

                if (( ((TA1CCR0)/100) + TA1CCR1) < TA1CCR0) {
                    TA1CCR1 += (TA1CCR0)/100;
                }
                else {
                    modePulse = !modePulse;
                }
            }
            else {
                if ( ((int16_t *) ((TA1CCR1 - ((TA1CCR0)/100))) > 0) && ((int16_t *) ((TA1CCR1 - ((TA1CCR0)/100))) < (int16_t *) (TA1CCR0))) {
                     TA1CCR1 -= (TA1CCR0)/100;
                 }
                 else {
                     modePulse = !modePulse;
                 }
             }
        }
*/
        else if (modeBlink) {                         //ALTERNADO
            P1OUT |= BIT0;
            P1OUT &= ~BIT1;
        }
        else {                              //SIMULTANEO
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

    if (modeBlink) {                              //ALTERNADO
        P1OUT &= ~BIT0;
        P1OUT |= BIT1;
    }
    else {                                   //SIMULTANEO
        P1OUT &= ~(BIT0 | BIT1);
    }

}
