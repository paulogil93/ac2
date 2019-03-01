#include <detpic32.h>
#include "i2c.h"

int getTemperature(int *temperature);
void delay(int ms);

int main(void) {
    int *temp;
    int ack;
    i2c1_init(TC74_CLK_FREQ);
   
    while(1) {
        ack = getTemperature(&temp);
        printStr("\e[H\e[J");
        printStr("Temperature: ");
        printInt10(temp);
        printStr("ºC\n");
        delay(250);     
    }

    return 0;
}

int getTemperature(int *temperature) {
    int ack;
    i2c1_start();
    ack = i2c1_send(ADDR_WR);
    ack = i2c1_send(RTR);
    i2c1_start();
    ack = i2c1_send(ADDR_RD);

    if(ack != 0) {
        i2c1_stop();
        printStr("ERROR: ack != 0 after i2c1_send(ADDR_RD)");
        exit(0);
    }

    *temperature = i2c1_receive(I2C_NACK);
    i2c1_stop();
    return ack;
}

void delay(int ms) {
    for(; ms > 0; ms--) {
        resetCoreTimer();
        while(readCoreTimer() < 20000);
    }
}