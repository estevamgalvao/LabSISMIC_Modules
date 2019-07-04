#include <msp430.h> 
#include <stdint.h>

#define MODE1HZ         1
#define MODE2HZ         2
#define MODE5HZ         5
#define MODE10HZ        10
#define MODE50HZ        50


void TA1_config (uint16_t freq, uint8_t luminosity);
void TA1_update (uint16_t freq, uint8_t luminosity);
