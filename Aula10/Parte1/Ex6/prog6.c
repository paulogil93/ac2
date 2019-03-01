#include <detpic32.h>

char i2c1_receive(char ack_bit) {
    while((I2C1CON & 0x001F) == 0);
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);

    if(ack_bit == 1 || ack_bit == 0) {
        I2C1CONbits.ACKDT = ack_bit;
    } else {
        printStr("ERROR: ack bit not 0 or 1");
        exit(0);
    }

     while((I2C1CON & 0x001F) == 0);
     I2C1CONbits.ACKEN = 1;
     while(I2C1CONbits.ACKEN == 1);

     return I2C1RCV;
}