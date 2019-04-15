;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
configPins:
			bic.b	#LOCKLPM5, &PM5CTL0		; Disable the GPIO power-on default high-impedance mode

			bic.b	#(BIT5|BIT6), &P5DIR	; P5.5 e P5.6 definidos p/ entrada
			bis.b	#(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7), &P5IN			; garantir que as entradas de P5 estão todas 1 no início
			;mov.b	#255, &P5IN				; EU TENTEI
			bis.b	#(BIT5|BIT6), &P5REN 	; coloco as resistências
			bis.b	#(BIT5|BIT6), &P5OUT 	; pullup

			bis.b	#(BIT0|BIT1), &P1DIR	; defino as LEDs como saída
			bic.b	#(BIT0|BIT1), &P1OUT 	; fazer leds começarem apagadas

loop:

           	cmp		#0x9F, &P5IN
           	jz		onGreen
 			cmp		#0xDF, &P5IN
 			jz		onRed
off:
			bic.b	#(BIT0|BIT1), &P1OUT
			jmp		loop
			nop

onGreen:
			bis.b	#BIT1, &P1OUT
			jmp 	loop
			nop

onRed:
			bis.b	#BIT0, &P1OUT
			jmp		loop
			nop

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
