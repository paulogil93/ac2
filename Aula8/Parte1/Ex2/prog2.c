#include <detpic32.h>

void delay(int ms);
void putc(char byte2send);
void setupUART(int baudrate, char parity, int dataBits, int stopBits);

int main(void) {
    setupUART(115200, 'N', 8, 1);
    while(1) {
        putc('+');
        delay(1000);
    }
    return 0;
}

void setupUART(int baudrate, char parity, int dataBits, int stopBits) {
    // Baudrate generator
    U1MODEbits.BRGH = 0;
    U1BRG = ((PBCLK + 8*baudrate) / (16*baudrate)) - 1;

    // Parity and data bits
    if(dataBits == 9) {
        U1MODEbits.PDSEL = 3;
    } else {
        if(parity == 'N') {
            U1MODEbits.PDSEL = 0;
        } else if(parity = 'E') {
            U1MODEbits.PDSEL = 1;
        } else {
            U1MODEbits.PDSEL = 2;
        }
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

void putc(char byte2send) {
    while(U1STAbits.UTXBF == 1); // wait while UTXBF == 1
    U1TXREG = byte2send;
}

void delay(int ms) {
    for(; ms > 0; ms--) {
        resetCoreTimer();
        while(readCoreTimer() < 20000);
    }
}