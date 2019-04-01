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
; DATA
;-------------------------------------------------------------------------------
			.data
vetor:	.byte	5, 4, 7, 3, 5, 1
			;.byte  	24, "ESTEVAMGALVAOALBUQUERQUE"

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
			.text

problema1:
			mov.b		#0, R9		;R9 será minha flag -> "0" para nenhuma troca foi feita

			mov.b		#vetor, R5 	;faço R5 apontar para o início do vetor, no caso, seu tamanho
			mov.b		#vetor, R8	;inicio R8
			mov.b		@R5, R6		;guardo o tamanho do vetor em R6

			dec			R6			;decremento R6 para evitar que R8 aponte para fora do vetor

			add			#1, R5		;incremento R5, fazendo-o apontar, de fato, para os elementos do vetor
			add			#2, R8		;faço R8 sempre ficar 1 posição a frente de R5

			call 		#ordena
			jmp			$
			nop

majorloop:
			mov.b		#0, R9		;R9 será minha flag -> "0" para nenhuma troca foi feita


			mov.b		#vetor, R5 	;faço R5 apontar para o início do vetor, no caso, seu tamanho
			mov.b		#vetor, R8	;inicio R8
			mov.b		@R5, R6		;guardo o tamanho do vetor em R6

			dec			R6			;decremento R6 para evitar que R8 aponte para fora do vetor

			add			#1, R5		;incremento R5, fazendo-o apontar, de fato, para os elementos do vetor
			add			#2, R8		;faço R8 sempre ficar 1 posição a frente de R5

			jmp			ordena
			nop

ordena:
			cmp.b		@R5, 0(R8)	;comparo R5 com R8

			jhs			troca		;caso R8 for maior que R5, quero troca-los de posição
			nop

ordena2parte:
			add			#1, R5		;incremento os 2
			add			#1, R8
			dec			R6

			jnz			ordena
			nop

			jmp			flagverifier
			nop

troca:
			mov.b		@R5, R7		;variável auxiliar para não perder o valor de R5 no meio da troca
			mov.b		@R8, R5		;salvo o valor de R8 em R5
			mov.b		@R7, R8		;salvo o antigo valor de R5 em R8

			mov.b		#1, R9		;seto a flag para 1, indicando que houve troca



			jmp			ordena2parte
			nop


flagverifier:
			cmp.b		@R9, #1

			jeq			majorloop
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
            
