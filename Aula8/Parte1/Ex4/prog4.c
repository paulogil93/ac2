#include <detpic32.h>

void setupUART(unsigned int baudrate, char parity, unsigned int stopBits);

void setupUART(unsigned int baudrate, char parity, unsigned int stopBits) {
    // Default
    if(baudrate < 600 || baudrate > 115200) {
        baudrate = 115200;
    }

    if(parity != 'N' && parity != 'E' && parity != 'O') {
        parity = 'N';
    }

    if(stopBits != 1 && stopBits != 2) {
        stopBits = 1;
    }

    // Baudrate generator
    U1MODEbits.BRGH = 0;    // 0 for 16x, 1 for 4x
    U1BRG = ((PBCLK + 8*baudrate) / (16*baudrate)) - 1;

    // Parity and data bits
    if(parity == 'N') {
        U1MODEbits.PDSEL = 0;
    } else if(parity = 'E') {
        U1MODEbits.PDSEL = 1;
    } else if(parity = 'O') {
        U1MODEbits.PDSEL = 2;
    }
    
    // Stop bits
    U1MODEbits.STSEL = stopBits - 1;
    // Enable TX
    U1STAbits.UTXEN = 1;
    // Enable RX
    U1STAbits.URXEN = 1;
    // Turn UART1 ON
    U1MODEbits.ON = 1;
}