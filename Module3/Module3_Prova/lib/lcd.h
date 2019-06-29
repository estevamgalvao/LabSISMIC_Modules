#include <msp430.h> 
#include <stdint.h>

#define LCD_ADDR 0x27
#define BT     BIT3
#define EN     BIT2
#define RW     BIT1
#define CHAR   BIT0
#define INSTR  0

//void lcdWriteNibble(uint8_t nibble,uint8_t rs);
void lcdWriteByte(uint8_t byte, uint8_t rs);
void lcdInit(void);
