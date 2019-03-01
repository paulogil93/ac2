// eeprom.c
#include <detpic32.h>
#include "eeprom.h"

void spi2_setClock(unsigned int clock_freq) {
    SPI2BRG = (PBCLK + clock_freq) / (2 * clock_freq) - 1;
}

void spi2_init(void) {
    volatile char trash;
    SPI2CONbits.ON = 0;     // Disable SPI2 module
    SPI2CONbits.CKP = 0;    // Clock idle state as logic level '0'
    SPI2CONbits.CKE = 1;    // Clock active transition: 1 > 0
    SPI2CONbits.SMP = 0;    // SPI data input phase bit: middle of data
    SPI2CONbits.MODE32 = 0; // 8 bits word length
    SPI2CONbits.MODE16 = 0; // 8 bits word length
    SPI2CONbits.ENHBUF = 1; // Enhanced buffer mode
    SPI2CONbits.MSSEN = 1;  // Master mode slave select
    SPI2CONbits.MSTEN = 1;  // Master mode
    
    while(SPI2STATbits.SPIRBE == 0) {   // while RX FIFO not empty
        trash = SPI2BUF;                // discard data
    }

    SPI2STATbits.SPIROV = 0; // Clear overflow error flag bit
    SPI2CONbits.ON = 1;      // Enable SPI2 module
}

char eeprom_readStatus(void) {
    volatile char trash;

     while(SPI2STATbits.SPIRBE == 0) {  // while RX FIFO not empty
        trash = SPI2BUF;                // discard data
    }

    SPI2STATbits.SPIROV = 0;            // Clear overflow error flag bit
    SPI2BUF = RDSR;                     // Send RDSR command
    SPI2BUF = 0;                        // Clocks data into SO
    while(SPI2STATbits.SPIBUSY == 1);   // wait while SPI module is working
    trash = SPI2BUF;                    // First char is garbage

    return SPI2BUF;
}

void eeprom_writeStatusCommand(char command) {
    while((eeprom_readStatus() & 0x01) == 1) ;  // Wait while write is in progress
    SPI2BUF = command;
    while(SPI2STATbits.SPIBUSY == 1);
}

void eeprom_writeData(int address, char value) {
    address = address & 0x01FF;
    while((eeprom_readStatus() & 0x01) == 1);   // Wait while write is in progress
    eeprom_writeStatusCommand(WREN);            // Enable write operations
    SPI2BUF = WRITE | ((address & 0x0100) >> 5);// Write and address command to TX FIFO
    SPI2BUF = address & 0x00FF;                 // 8LSBits of address to TX FIFO
    SPI2BUF = value;                            // Value to TX FIFO
    while(SPI2STATbits.SPIBUSY == 1);           // Wait while SPI is working
}

char eeprom_readData(int address) {
    volatile char trash;

    while(SPI2STATbits.SPIRBE == 0) {           // While RX FIFO not empty
        trash = SPI2BUF;                        // Discard data
    }

    SPI2STATbits.SPIROV = 0;                     // Clear overflow error flag bit
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