#include <detpic32.h>

void i2c1_stop(void) {
    while((I2C1CON & 0x001F) == 0);
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
}