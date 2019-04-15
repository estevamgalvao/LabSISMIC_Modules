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
			bic.b	#(BIT5|BIT6), &P5DIR
			bis.b	#(BIT5|BIT6), &P5REN 	;botão é bit clear nas resistências?
			bis.b	#(BIT5|BIT6), &P5OUT 	;pullup

			bis.b	#(BIT0|BIT1), &P1DIR	;defino as LEDs como saída
			bis.b	#(BIT0|BIT1), &P1OUT 	;fazer leds começarem acesas

loop:
			bit.b	#BIT5, &P5IN
			jc		off				;se o botão estiver pressionado, bit5 é 0, logo, no teste, o carry quando não identifica bit em 1, é 0 também
									;se o carry estiver em 1, significa que o bit testado é 1
on:
			bis.b	#BIT0, &P1OUT
			jmp		loop
off:
			bic.b	#BIT0, &P1OUT
			jmp		loop
			nop

			;bis.b	#(BIT0|BIT1), &P1OUT ;fazer leds começarem acesas




                                            

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
            
