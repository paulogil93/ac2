#include <detpic32.h>

void setupUART(int baudrate, int stopBits);

void setupUART(int baudrate, int stopBits) {
    // Baudrate generator
    U1MODEbits.BRGH = 0;
    U1BRG = ((PBCLK + 8*baudrate) / (16*baudrate)) - 1;
    // Parity and data bits
    U1MODEbits.PDSEL = 0;
    // Stop bits
    U1MODEbits.STSEL = stopBits - 1;
    // Enable TX
    U1STAbits.UTXEN = 1;
    // Enable RX
    U1STAbits.URXEN = 1;
    // Turn UART1 ON
    U1MODEbits.ON = 1;
}