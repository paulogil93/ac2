#include <detpic32.h>

char getc(void);
void putc(char byte2send);
void puts(char *str);
void setupUART(unsigned int baudrate, char parity, unsigned int stopBits);

int main(void) {
    setupUART(115200, 'N', 1);
    EnableInterrupts();

    while(1);
    
    return 0;
}

char getc(void) {
    // Check for overrun
    if(U1STAbits.OERR == 1) {
        U1STAbits.OERR == 0;
    }

    while(U1STAbits.URXDA == 0);    // Wait until buffer has data

    // Check for framming or parity errors
    if(U1STAbits.FERR || U1STAbits.PERR) {
        char junk = U1RXREG;
        return 0;
    } else {
        return U1RXREG;
    }
}

void setupUART(unsigned int baudrate, char parity, unsigned int stopBits) {
    // Default UART(115200, N, 1)
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
    } else if(parity == 'E') {
        U1MODEbits.PDSEL = 1;
    } else if(parity == 'O') {
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

    /*                  
    Interrupt system
    */
    // Enables
    U1STAbits.UTXSEL = 0;  // Interrupt is generated when the transmit buffer contains at least one empty space
    U1STAbits.URXISEL = 0;  // Interrupt flag bit is set when a character is received
    IEC0bits.U1RXIE = 1;    // Receive interrupt enable
    IEC0bits.U1TXIE = 0;    // Transmit interrupt enable
    IEC0bits.U1EIE = 1;     // Error interrupt enable
    // Priority
    IPC6bits.U1IP = 4;      // ???
    // Flags
    IFS0bits.U1EIF = 0;     // Error interrupt flag
    IFS0bits.U1RXIF = 0;    // Receive interrupt flag
    IFS0bits.U1TXIF = 0;    // Transmit interrupt flag

}

void putc(char byte2send) {
    while(U1STAbits.UTXBF == 1); // wait while UTXBF == 1
    U1TXREG = byte2send;
}

void puts(char *str) {
    while(*str) {
        putc(*str);
        str++;
    }
}

 /*
* Interrupt is triggered when RX has at least one char
* steps:
* char -> URXISEL -> U1RXIF = 1 -> isr_uart1 -> U1RXIF = 0 -> wait for char
*/
void _int_(_UART_1_VECTOR) isr_uart1(void) {
    putc(U1RXREG);
    IFS0bits.U1RXIF = 0;
}