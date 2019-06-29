#include <msp430.h> 
#include <stdint.h>

#define MSB	1
#define LSB	0

#define WITHOUT 2
#define ODD		1
#define EVEN	0
#define BITS8	0
#define BITS7	1


void UART_config (uint32_t baudrate, uint8_t bitOrder, uint8_t parity, uint8_t nStops, uint8_t cLength);
