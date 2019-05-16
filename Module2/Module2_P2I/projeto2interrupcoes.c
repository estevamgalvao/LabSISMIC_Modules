#include <msp430.h> 


volatile unsigned int timeEmition;
volatile unsigned int timeReception;
volatile unsigned int timeDiff;


void waitDelaySMCLK(unsigned int microDelay) {

    TA4CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA4CCR0 = microDelay;

    while(!(TA4CCTL0 & CCIFG));
    TA4CCTL0 &= ~CCIFG;
}


void main(void){

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    //Configurar pinos que serão usados:
    //  P5.5 - BOTÃO DIREITO    - ENTRADA PULL UP
    //  P1.0 - LED VERMELHO     - SAÍDA APAGADO
    //  P1.1 - LED VERDE        - SAÍDA APAGADO
    //  P3.3 - TRIGGER SAÍDA    - SAÍDA "ENVIANDO 0"
    //  P1.3 - ECHO ENTRADA     - ENTRADA -> sem pull up pois o sensor envia corretamente 0's e 1's

    P5DIR &= ~BIT5;
    P5REN |= BIT5;
    P5OUT |= BIT5;

    P1DIR |= (BIT0 | BIT1);
    P1OUT &= ~(BIT0 | BIT1);

    P3DIR |= BIT3;

    P1DIR &= ~BIT3;

    //Quero que o sensor meça a distância no momento em que o botão é pressionado, logo, irei montar uma interrupção nesse botão que irá desencadear a contagem
    //Vou precisar configurar um timer no modo de captura, no caso, o timer ligado ao pino P1.3
    //Antes de configurar o timer, tenho que fazer a portabilidade do pino para o timer

    //Para fazer a portabilidade do pino, preciso selecionar qual dos periféricos conectados ao P1.3 irei usar utilizando o PxSEL
    //No Datasheet, página 87, encontramos que o TA1.2, no modo de saída, onde é controlado pelo CCI2A, faz P1SEL = 01
    //Então fazemos:

    P1SEL0 |= BIT3;    // BIT3 pois é o P1.*3*
    P1SEL1 &= ~BIT3;

    //Agora, quando o P1.3 receber um flanco, algo acontecerá com o CCIFG do TA1.2 dependendo do modo que o configurarmos
    //datasheet PAG 77 P1.3 TA1.2 = CCI2A
    //datasheet PAG 87 P1.3 TA1.2 MODO SAÍDA (CCI2A) -> P1SEL0 = 1 P1SEL1 = 0

    //Me resta, na main, apenas habilitar a interrupção no meu botão (P5.5) e informar se a interrupção é acionada em flanco de descida ou subida
    //O flanco que acionará a interrupção deve ser de descida pois meu botão ao ser pressionado irá pra 0

    P5IE |= BIT5;        //P5 Interrupt Enable
    P5IES |= BIT5;       //P5 Interrupt Edge Selection (1 = high-to-low)

    __enable_interrupt();   //Habilito o General Interrupt Enable (GIE) no meu Status Register (R2)

}


#pragma vector=PORT5_VECTOR
__interrupt void P5_ISR (void) {
    switch(P5IV) {
    case 0x0C:
        //No momento em que eu entro nesse switch-case, significa que a interrupção ocorreu, ou seja, o botão foi pressionado
        //Devivo ao boucing do botão, eu irei desligar a flag de interrupções do respectivo botão
        //Caso contrário, o processo da interrupção poderia ocorrer várias vezes e segurar o andamento da main, quando na verdade, eu só queria que acontecesse uma vez

        P5IE &= ~BIT5;

        //Agora, quero que o programa comece de fato, então irei configurar o timer, TA1.2, para contar o tempo do sinal do ECHO

        TA1CTL = TASSEL__SMCLK | MC__CONTINUOUS | TACLR;
        //TASSEL__SMCLK: Escolho a "source" sendo o SM Clock - 1MHz
        //MC__CONTINUOUS: Modo de contagem contínuo porque não vou me importar com o valor de fato, mas sim, com a diferença dos 2 limiares da captura
        //TACLR: Dou um clear só pra garantir que o timer comece do 0
        //Eu não habilito a interrupção do timer pois lembre que o TAIFG é setado quando acontece o overflow e eu não quero saber quando acontece o overflow
        //Lembre-se também que quem está conectado ao pino é o canal 2 do timer A1, então ele que irá reagir aos flancos do P1.3 (ECHO)
        TA1CCTL2 = CM__BOTH | CCIS_0 | CAP | CCIE;
        //CM__BOTH: Quero que ele capture em flancos de subida e de descida, pois meu ECHO vai para 1 quando emite o sinal ultrassom e para 0 quando recebe, então quero capturar nesses dois momentos
        //CCIS_0: Capture/Compare Input Select. Basicamente, isso diz ao canal quem de onde virá o sinal dizendo que uma captura deve ser efetuada
        //datasheet PAG 77 P1.3 TA1.2 = CCI2A
        //CAP: Habilito as capturas
        //CCIE: Agora sim habilito as interrupções do canal do timer

        //Depois de ter o timer e seu canal configurados, irei emitir o sinal pro TRIGGER e receber os sinais do ECHO
        //Note que não faz diferença começar o timer antes do sinal do TRIGGER porque estou interessado na diferença dos limiares e não nos limiares propriamente ditos

        P3OUT |= BIT3;      //Emito o sinal do TRIGGER
        waitDelaySMCLK(20); //Espero 20 Microsegundos
        P3OUT &= ~BIT3;     //Desligo o sinal do TRIGGER

        //Com isso, terminei tudo que tinha que fazer nessa interrupção
        //Agora a interrupção do TA1.2 irá cuidar das capturas e ligar os LED
    }
}

#pragma vector=TA1_A1_VECTOR
__interrupt void TA1C2_ISR (void) {
    switch(TA1IV) {
    case 0x04:
        if (TA1CCTL2 & CCI) {
            timeEmition = TA1CCR2;
        }
        else {
            timeReception = TA1CCR2;
        }


        if (timeReception < timeEmition) {
            timeDiff = 65536 - timeEmition + timeReception;
        }
        else {
            timeDiff = timeReception - timeEmition;
        }

        if (timeDiff < 1176) {
            P1OUT &= ~BIT1;
            P1OUT |= BIT0;
        }

        else if (timeDiff >= 1176 && timeDiff <= 2351) {
            P1OUT &= ~BIT0;
            P1OUT |= BIT1;
        }

        else {
            P1OUT |= (BIT0 | BIT1);
        }
        break;
    default: break;
    }
}




