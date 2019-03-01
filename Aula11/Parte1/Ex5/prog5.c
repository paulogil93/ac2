#include <detpic32.h>

#define WRITE           0x02
#define RDSR            0x05
#define READ            0x03
#define WRSR            0x01
#define WRDI            0x04
#define WREN            0x06
#define EEPROM_CLOCK    500000

void spi2_setClock(unsigned int clock_freq);
void spi2_init(void);
char eeprom_readStatus(void);
void delay(int ms);

int main(void) {
    char status;
    spi2_init();
    spi2_setClock(EEPROM_CLOCK);

    while(1) {
        status = eeprom_readStatus();
        printInt(status, 2 | 4 << 16);
        delay(500);
    }
}

void spi2_setClock(unsigned int clock_freq) {
    SPI2BRG = (PBCLK + clock_freq) / (2 * clock_freq) - 1;
}

void spi2_init(void) {
    volatile char trash;
    SPI2CONbits.ON = 0;     // Disable SPI2 module
    SPI2CONbits.CKP = 0;    // Clock idle state as logic level '0'
    SPI2CONbits.CKE = 1;     // Clock active transition: 1 > 0
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
    SPI2CONbits.ON = 1;     // Enable SPI2 module
}

char eeprom_readStatus(void) {
    volatile char trash;

     while(SPI2STATbits.SPIRBE == 0) {  // while RX FIFO not empty
        trash = SPI2BUF;                // discard data
    }

    SPI2STATbits.SPIROV = 0;         // Clear overflow error flag bit
    SPI2BUF = RDSR;                 // Send RDSR command
    SPI2BUF = 0;                    // Clocks data into SO
    while(SPI2STATbits.SPIBUSY);    // wait while SPI module is working
    trash = SPI2BUF;                // First char is garbage

    return SPI2BUF;
}

void delay(int ms) {
    for(; ms > 0; ms--) {
        resetCoreTimer();
        while(readCoreTimer() < 20000);
    }
}