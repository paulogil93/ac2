#include <detpic32.h>

#define WRITE           0x02
#define RDSR            0x05
#define READ            0x03
#define WRSR            0x01
#define WRDI            0x04
#define WREN            0x06
#define EEPROM_CLOCK    500000

char eeprom_readStatus(void) {
    volatile char trash;

    while(SPI2STATbits.SPIRBE == 0) {  // while RX FIFO not empty
        trash = SPI2BUF;                // discard data
    }

    SPI2CONbits.SPIROV = 0;         // Clear overflow error flag bit
    SPI2BUF = RDSR;                 // Send RDSR command
    SPI2BUF = 0;                    // Clocks data into SO
    while(SPI2STATbits.SPIBUSY);    // wait while SPI module is working
    trash = SPI2BUF;                // First char is garbage

    return SPI2BUF;
}