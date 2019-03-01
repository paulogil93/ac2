#include <detpic32.h>

int i2c1_send(unsigned char value) {
    I2C1TRN = value;
    while(I2C1STAT.TRSTAT == 1);

    return I2C1STAT.ACKSTAT;
}