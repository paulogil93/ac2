#include <detpic32.h>

void i2c1_init(unsigned int clock_freq) {
    I2C1BRG = (PBCLK + clock_freq) / (2 * clock_freq) - 1;
    I2C1CONbits.ON = 1;
}