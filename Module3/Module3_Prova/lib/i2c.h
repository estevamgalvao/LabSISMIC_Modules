#include <msp430.h> 
#include <stdint.h>

#define MASTER 1
#define SLAVE  0

void    i2cConfigB1 (uint8_t isMaster,uint8_t ownAddr,uint32_t baudrate_kHz);
void    i2cConfigB2 (uint8_t isMaster,uint8_t ownAddr,uint32_t baudrate_kHz);
uint8_t i2cWriteByte(uint8_t slaveAddr,uint8_t data);
uint8_t i2cReadByte (uint8_t slaveAddr,uint8_t dataRead);
