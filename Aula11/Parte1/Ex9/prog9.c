#include <detpic32.h>

char eeprom_readData(int address) {
    volatile char trash;

    while(SPI2STATbits.SPIRBE == 0) {           // While RX FIFO not empty
        trash = SPI2BUF;                        // Discard data
    }

    SPI2CONbits.SPIROV = 0;                     // Clear overflow error flag bit
    address = address & 0x01FF;                 // Limit address to 9 bits
    while((eeprom_readStatus() & 0x01) == 1);   // Wait while write is in progress
    SPI2BUF = READ | ((address & 0x0100) >> 5); // Send READ command and address to TX FIFO
    SPI2BUF = address & 0x00FF;                 // Send address to TX FIFO
    SPI2BUF = 0;                                // Send some char to TX FIFO
    while(SPI2STATbits.SPIBUSY == 1);           // Wait while SPI is working
    trash = SPI2BUF;                            // Discard char from RX FIFO
    trash = SPI2BUF;                            // Discard char from RX FIFO

    return SPI2BUF;                             // Return char from RX FIFO
}