#include <detpic32.h>

void eeprom_writeStatusCommand(char command) {
    while(eeprom_readStatus() & 0x01);  // Wait while write is in progress
    SPI2BUF = command;
    while(SPI2STATbits.SPIBUSY == 1);
}