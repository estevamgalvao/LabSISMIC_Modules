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
vetorEXE1:	.byte	6, 1, 2, 7, 3, 1, 5
			;.byte  	24, "ESTEVAMGALVAOALBUQUERQUE"

;vetorEXE2:	.byte	24, 0, "ESTEVAMGALVAOALBUQUERQUE", 0


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

;-------------------------------------------------------------------------------
; Exercício1
;-------------------------------------------------------------------------------
			.text
EXE1:
			mov		#vetorEXE1, R5	;inicializar R5 com o endereço do vetor
			mov.b	@R5, R6		;inicializar o contador do laço
			mov.b	#0, R8		;inicializar nosso contador de frequência

			add		#1, R5		;incremento R5 para começar nos valores do vetor de fato

			mov.b	@R5, R7		;inicializar nossa variável de controle com o menor elemento

			dec		R6			;decremento R6 pois já inicializo R7 com a primeira variável, fazendo a primeira cmp ser desnecessária

			call	#menor		;chamar subrot menor

			mov		#vetorEXE1, R5	;inicializar R5 com o endereço do vetor
			mov.b	@R5, R6		;inicializar o contador do laço

			add		#1, R5		;incremento R5 para começar nos valores do vetor de fato

			call 	#freq

			jmp		$			;travar execução para resolver break em ret 2 vezes
			nop
menor:
			cmp.b		@R5, R7	;comparo valor .byte de R5 com R7

			jhs		troca		;cmp -> dst - src, logo caso R7 seja maior que R5, significa que o valor de R5 deve ir para R7, então deve entrar em troca
			nop					;todo jmp tem que ter nop após? por que?

menor2parte:
			add		#1, R5		;incremento R5 e 1 byte, ou seja, o próximo elemento, já que o vetor é do tipo .byte
			dec 	R6			;decremento R6, pois uma execução do laço foi feita

			jnz		menor		;enquanto R6 não for zero, volto para o início de menor, criando o laço
			nop

			ret 				;se passou do jnz, significa que o laço deve acabar, então retorno a call

troca:
			mov.b	@R5, R7		;troco o valor que R5 aponta, com R7

			jmp 	menor2parte	;retorno ao meio da execução de menor
			nop

freq:							;após ler todo o vetor e identificar quem é o menor, começo outra leitura para contar quantas vezes ele aparece
			cmp.b		@R5, R7

			jeq		incrementa	;PARECE NÃO ESTAR ENTRANDO NESSE JMP ;PQ "todo" tá destacado? ;não estava entrando pq era "cmp.b"
			nop

freq2parte:
			add		#1, R5		;incremento R5 e 1 byte, ou seja, o próximo elemento, já que o vetor é do tipo .byte
			dec 	R6			;decremento R6, pois uma execução do laço foi feita

			jnz 	freq		;entro na subrotina
			nop

			ret

incrementa:
			inc 	R8			;incremento R8 quando encontro o menor elemento guardado em R7

			jmp		freq2parte	;retorno a subrotina
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
            
