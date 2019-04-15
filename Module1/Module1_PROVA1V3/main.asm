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
;VET0:	.word
VET1:	.word	4, 9, -3, -7, 10
VET2:	.word	7, -5, -3, -1, 0, 2, 4, 6


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
		.text

		mov.b		#0, R9		;R9 será minha flag -> "0" para nenhuma troca foi feita

		mov		#VET1, R5
		mov		#VET1, R8

		call 	#INV

		mov		#VET2, R5
		mov		#VET2, R8
		jmp		$

INV:

			mov			@R5, R6		;guardo o tamanho do vetor em R6
			dec			R6			;decremento R6 para evitar que R8 aponte para fora do vetor

			add			#2, R5		;incremento R5, fazendo-o apontar, de fato, para os elementos do vetor
			add			#4, R8		;faço R8 sempre ficar 1 posição a frente de R5

			mov			@R5, R7		;inicializar nossa variável de controle com o primeiro elemento

compare:
			cmp.w	@R5, R7		; comparo valor de R5 com R7
			jn		troca

compare2parte:

			cmp.w	@R8, 0(R5)	;comparo R8 com R5
			;mov		@R8, R10
			;sub		R10, 0(R5)
			jn		naoinverte		;caso R8 for menor que R5, quero troca-los de posição
			nop
inverte:

			mov		@R5, R4			;variável auxiliar para não perder o valor de R5 no meio da troca
			;inv		R4
			mov		@R8, R10		;salvo o valor de R8 em R5
			;inv 	R10
			mov		R10, 0(R5)		;salvo o valor de R8 em R5
			mov		R4, 0(R8)		;salvo o antigo valor de R5 em R8

			mov		#1, R9			;seto a flag para 1, indicando que houve troca

			jmp			compare3parte
			nop

naoinverte:
			nop
compare3parte:
			add			#2, R5		;incremento os 2
			add			#2, R8
			dec			R6

			jnz			inverte
			nop

			jmp			flagverifier
			nop





troca:
			mov	@R5, R7		;troco o valor que R5 aponta, com R7

			jmp 	compare2parte	;retorno ao meio da execução de menor
			nop




flagverifier:
			cmp			#1, R9

			jeq			INV
			nop

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
            
