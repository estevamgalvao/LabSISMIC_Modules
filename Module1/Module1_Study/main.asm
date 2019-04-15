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

;-------------------------------------------------------------------------------
; PRIMEIRA TENTATIVA
;
configPins:
			bic.b	#LOCKLPM5, &PM5CTL0		; Disable the GPIO power-on default high-impedance mode

			bic.b	#(BIT5|BIT6), &P5DIR	; P5.5 e P5.6 definidos p/ entrada
			bis.b	#(BIT5|BIT6), &P5REN 	; coloco as resistências
			bis.b	#(BIT5|BIT6), &P5OUT 	; pullup

			bis.b	#(BIT0|BIT1), &P1DIR	; defino as LEDs como saída
			bic.b	#(BIT0|BIT1), &P1OUT 	; fazer leds começarem acesas

loop:
			bit.b	#BIT5, &P5IN	;verifico se o botão está pressionado, caso esteja, o carry vai estar em 0
			jc		loop			;se o botão não estiver pressionado, continuo esperando ele ser pressionado
			call 	#reboteManager	;caso o botão tenha sido pressionado, chamo a função que esperará 2ms para cuidar do rebote
			bit.b	#BIT5, &P5IN	;após os 2ms, onde o rebote já deve ter se estabilizado, verifico se o botão ainda está sendo pressionado, caso sim, sigo o código e caio em "on", onde o botão será ligado
			jc		off				;se o botão estiver pressionado, bit5 é 0, logo, no teste, o carry quando não identifica bit em 1, é 0 também
									;se o carry estiver em 1, significa que o bit testado é 1
on:
			bis.b	#BIT0, &P1OUT
			jmp		loop
			nop
off:
			bic.b	#BIT0, &P1OUT
			jmp 	loop
			nop

reboteManager:
			mov.w	#10000, R6 	;faço R6 decrementar 10000 -> 16Mhz é o clock da MSP430
								;jmp = 2 instr - dec = 1 -> 30000 instruções ---> 30000/16000000 -> 0.001875s
loopRM:
			dec		R6
			jnz		loopRM
			ret


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
            
