#include <detpic32.h>

void spi2_init(void) {
    volatile char trash;
    SPI2CONbits.ON = 0;     // Disable SPI2 module
    SPI2CONbits.CKP = 0;    // Clock idle state as logic level '0'
    SPI2CONbits.CKE = 1     // Clock active transition: 1 > 0
    SPI2CONbits.SMP = 0;    // SPI data input phase bit: middle of data
    SPI2CONbits.MODE32 = 0; // 8 bits word length
    SPI2CONbits.MODE16 = 0; // 8 bits word length
    SPI2CONbits.ENHBUF = 1; // Enhanced buffer mode
    SPI2CONbits.MSSEN = 1;  // Master mode slave select
    SPI2CONbits.MSTEN = 1;  // Master mode
    
    while(SPI2STATbits.SPIRBE == 0) {   // while RX FIFO not empty
        trash = SPI2BUF;                // discard data
    }

    SPI2CONbits.SPIROV = 0; // Clear overflow error flag bit
    SPI2CONbits.ON = 1;     // Enable SPI2 module
}