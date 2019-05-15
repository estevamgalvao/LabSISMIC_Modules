/*
 * msp_timers.h
 *
 *  Created on: Apr 21, 2018
 *      Author: hugonfonseca
 */

#ifndef MSP_TIMERS_H_
#define MSP_TIMERS_H_

#include <msp430.h>
#include "pins.h"

inline void delay_mili(uint16_t t_mili){
    //Seta timer: Clock = ACLK dividido por 9, modo de subida.
    TA2CTL = (TASSEL__ACLK | MC__UP | ID__8);

    //Divide o valor do clock por 4
    TA2EX0 = TAIDEX_4;

    //Seta modo de saida para SET
    TA2CCTL0 = OUTMOD_1;

    //Limiar da contagem
    TA2CCR0 = t_mili;

    // Reseta o timer A1 e inicia a contagem
    SET_REG(TA2CTL, TACLR);

    // Enquanto o tempo for menor aguarda o delay
    while(!READ_REG(TA2CCTL0,CCIFG));
}

inline void delay_micro(uint16_t t_micro){
    //Seta timer: Clock = SMCLK.
    TA2CTL = (TASSEL__SMCLK | MC__UP);

    //Seta modo de saida para SET
    TA2CCTL0 = OUTMOD_1;

    //Limiar da contagem
    TA2CCR0 = t_micro;

    // Reseta o timer A1 e inicia a contagem
    SET_REG(TA2CTL, TACLR);

    // Enquanto o tempo for menor aguarda o delay
    while(!READ_REG(TA2CCTL0,CCIFG));
}

#endif /* MSP_TIMERS_H_ */
