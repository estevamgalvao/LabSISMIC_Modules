#include <msp430.h> 
#include <stdint.h>


volatile int i;
/**
 * main.c
 */

void waitDelay(unsigned int delay) {

    TA2CTL = TASSEL__ACLK | MC__UP | TACLR;
    TA2CCR0 = delay;
    while(!(TA2CCTL0 & CCIFG));
    TA2CCTL0 &= ~CCIFG;

}

void startDialogue(uint16_t address) {

    UCB2I2CSA = address;
    UCB2CTLW0 |= UCTXSTT;

    while(!(UCB2IFG & UCTXIFG0));


/*
    if((UCB2IFG & UCNACKIFG)) {
        // O camarada mandou um NACK
    }
    else{
        // Ele ta me ouvindo
    }
*/
}

void talk(uint8_t data) {

    UCB2TXBUF = data; //troquei uma ideia
   //talvez quando eu atribuo data ao TXBUF ele automaticamente já transmita e fique vazio
    //então a UCTXIFG0 já fique em 1
    while(!(UCB2IFG & UCTXIFG0)); //esperei eu terminar a ideia
                                   //está pra sempre nesse loop


}


void stopDialogue(void) {

    UCB2CTLW0 |= UCTXSTP; //envio um STOP
    UCB2IFG &= ~UCTXIFG;
    while(UCB2CTLW0 & UCTXSTP); //espero o STOP ser enviado com sucesso
    //while(!(UCB2IFG & UCTXIFG0));
}


void configB1(void) {

    //UCB0SDA - P5.0
    //UCB0SCL - P5.1 pag 15
    //Selecionando o P5.0 para UCB1SDA
    P5SEL0 |= BIT0;
    P5SEL1 &= ~BIT0;

    //Selecionando o P5.1 para UCB1SLC
    P5SEL0 |= BIT1;
    P5SEL1 &= ~BIT0;

    //Agora preciso habilitar as resistências de pull up
    P5REN |= (BIT0 | BIT1);
    P5OUT |= (BIT0 | BIT1);

    //Vou habilitar o modo de edição do registrador que coordena a interface I2C
    UCB1CTLW0 |= UCSWRST;

    UCB1CTLW0 |= UCMODE_3 | UCSSEL__SMCLK;

    UCB1I2COA0 = UCOAEN__ENABLE | 0x0034;

    UCB0BRW = 10;

    UCB1CTLW0 &= ~UCSWRST;

    UCB1IE |= UCRXIE0;
}

void configB2(void) { //Configurar o Mestre

    //Por que não devo dar a DIR para os pinos de SDA e SLC?
    //P7SSEL pág 108
    //Selecionando o P7.0 para UCB2SDA
    P7SEL1 &= ~BIT0;
    P7SEL0 |= BIT0;
    //Selecionando o P7.1 para UCB2SCL
    P7SEL1 &= ~BIT1;
    P7SEL0 |= BIT1;

    //Agora preciso habilitar as resistências de pull up
    P7REN |= (BIT0 | BIT1);
    P7OUT |= (BIT0 | BIT1);


    //Habilitar o modo de edição do registrador
    UCB2CTLW0 |= UCSWRST;

    UCB2CTLW0 |= UCMST_1 | UCMODE_3 | UCSSEL__SMCLK | UCTR;
    UCB2I2COA0 = 0x0012;//pq o Own Adress 0 para o mestre? -> na verdade a flag UCOAEN define se é endereço de escravo ou mestre

    UCB2BRW = 10; //Selecionar o Baud-Rate - por que? e por que 10K?

    UCB2IE |= UCNACKIE;

    UCB2CTLW0 &= ~UCSWRST;
}



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode
    __enable_interrupt();

    P1DIR |= (BIT0 | BIT1);
    P1OUT &= ~(BIT0 | BIT1);

    configB1();
    configB2();

    startDialogue(0x0034);

    talk(0x0055);

    stopDialogue();

    if (UCB1RXBUF == 0x0055) {
        i = 10;
        while(i > 0) {
            P1OUT ^= BIT0;
            waitDelay(8191);
            i--;
        }
    }

    P1OUT &= ~BIT0;
    waitDelay(65528);

    startDialogue(0x0034);

    talk(0x0024);

    stopDialogue();

    if (UCB1RXBUF == 0x0024) {
        i = 10;
            while(i > 0) {
                P1OUT ^= BIT1;
                waitDelay(8191);
                i--;
            }
        }

    P1OUT &= ~BIT1;
    waitDelay(65528);

    startDialogue(0x0034);
    talk(0x0072);
    //startDialogue(0x0034);
    talk(0x0086); //não sobreescreveu
                   //parece só sobreescrever depois de 1 stop
                   //talvez não seja possível sobreescrever o buffer
                    //apenas com o stop, porque ele talvez limpe o buffer

    stopDialogue(); //aparentemente o while esperando enviar o stop nunca é satisfeito

    P1OUT |= BIT0;


    if (UCB1RXBUF == 0x0086) {
        i = 10;
        while(i > 0) {
                P1OUT ^= BIT1;
                P1OUT ^= BIT0;
                waitDelay(8191);
                i--;
            }
        }

/*
    if (UCB1RXBUF == 0x0055) {
        P1OUT |= BIT1;
    }
    else {
        P1OUT |= BIT0;
    }
*/


    return 0;
}
//preso no loop
//led verde interrupção só funciona quando eu faço






#pragma vector=EUSCI_B1_VECTOR
__interrupt void UCB1_ISR(void) {
    switch(UCB1IV) {
    case USCI_NONE:          break;     // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;     // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG: break;     // Vector 4: NACKIFG
    case USCI_I2C_UCSTTIFG:  break;     // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:  break;           // Vector 8: STPIFG
        //TXData = 0;
        //UCB2IFG &= ~UCSTPIFG;           // Clear stop condition int flag
        //break;
    case USCI_I2C_UCRXIFG3:  break;     // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;     // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;     // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;     // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;     // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;     // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:             // Vector 22: RXIFG0
        //P1OUT |= BIT1;

        break;
    case USCI_I2C_UCTXIFG0:  break;           // Vector 24: TXIFG0
        //UCB2TXBUF = TXData++;
        //break;
    case USCI_I2C_UCBCNTIFG: break;     // Vector 26: BCNTIFG
    case USCI_I2C_UCCLTOIFG: break;     // Vector 28: clock low timeout
    case USCI_I2C_UCBIT9IFG: break;     // Vector 30: 9th bit
    default: break;
    }

}




#pragma vector=EUSCI_B2_VECTOR
__interrupt void UCB2_ISR(void) {
    switch(UCB2IV) {
    case USCI_NONE:          break;     // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;     // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG:            // Vector 4: NACKIFG


        while((UCB2IFG & UCTXIFG0)); //Aguardo novamente o buffer ficar vazio para os casos em que a interrupção ocorreu no meio de uma transmissão
        UCB2CTLW0 |= UCTXSTP; //envio um STOP
        while(UCB2CTLW0 & UCTXSTP); //espero o STOP ser enviado com sucesso
        P1OUT |= BIT0;
        break;


    case USCI_I2C_UCSTTIFG:  break;     // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:  break;           // Vector 8: STPIFG
        //TXData = 0;
        //UCB2IFG &= ~UCSTPIFG;           // Clear stop condition int flag
        //break;
    case USCI_I2C_UCRXIFG3:  break;     // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;     // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;     // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;     // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;     // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;     // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:  break;     // Vector 22: RXIFG0
    case USCI_I2C_UCTXIFG0:  break;           // Vector 24: TXIFG0
        //UCB2TXBUF = TXData++;
        //break;
    case USCI_I2C_UCBCNTIFG: break;     // Vector 26: BCNTIFG
    case USCI_I2C_UCCLTOIFG: break;     // Vector 28: clock low timeout
    case USCI_I2C_UCBIT9IFG: break;     // Vector 30: 9th bit
    default: break;
    }

}




