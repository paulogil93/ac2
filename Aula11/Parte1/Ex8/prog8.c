#include <detpic32.h>

void eeprom_writeData(int address, char value) {
    address = address & 0x01FF;
    while((eeprom_readStatus() & 0x01) == 1);   // Wait while write is in progress
    eeprom_writeStatusCommand(WREN);            // Enable write operations
    SPI2BUF = WRITE | ((address & 0x0100) >> 5);// Write and address command to TX FIFO
    SPI2BUF = address & 0x00FF;                 // 8LSBits of address to TX FIFO
    SPI2BUF = value;                            // Value to TX FIFO
    while(SPI2STATbits.SPIBUSY == 1);           // Wait while SPI is working
}