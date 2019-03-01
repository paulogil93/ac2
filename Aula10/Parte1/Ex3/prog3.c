#include <detpic32.h>

void i2c1_start(void) {
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN == 1);
}