#include <detpic32.h>

#define DisableUart1RxInterrupt()       IEC0bits.U1RXIE = 0;
#define EnableUart1RxInterrupt()        IEC0bits.U1RXIE = 1;
#define DisableUart1TxInterrupt()       IEC0bits.U1TXIE = 0;
#define EnableUart1TxInterrupt()        IEC0bits.U1TXIE = 1;
#define BUF_SIZE                        8
#define INDEX_MASK                      (BUF_SIZE - 1)
#define FALSE                           0
#define TRUE                            1

void comDrv_config(unsigned int baudrate, char parity, unsigned int stopBits);
void comDrv_flushRx(void);
void comDrv_flushTx(void);
void comDrv_putc(char ch);
void comDrv_puts(char *str);
char comDrv_getc(char *pchar);

typedef struct {
    unsigned char data[BUF_SIZE];
    unsigned int head;
    unsigned int tail;
    unsigned int count;
} circularBuffer;

volatile circularBuffer txb, rxb;

int main(void) {
    char ch;
    comDrv_config(115200, 'N', 1);
    comDrv_flushRx();
    comDrv_flushTx();
    EnableInterrupts();
    
    comDrv_puts("PIC32 UART Device-driver\n");
    while(1) {
        comDrv_getc(&ch);
        comDrv_putc(ch);
        if(ch == 'S') {
            comDrv_puts("\nEsta string serve unica e exclusivamente para encher chouriços.\n");
        }
        ch = 0;
    }

    return 0;
}

void comDrv_flushRx(void) {
    rxb.head = 0;
    rxb.tail = 0;
    rxb.count = 0;
}

void comDrv_flushTx(void) {
    txb.head = 0;
    txb.tail = 0;
    txb.count = 0;
}

void comDrv_putc(char ch) {
    while(txb.count == BUF_SIZE);
    txb.data[txb.tail] = ch;
    txb.tail = (txb.tail + 1) & INDEX_MASK;
    DisableUart1TxInterrupt();
    txb.count++;
    EnableUart1TxInterrupt();
}

void comDrv_puts(char *str) {
    while(*str != 0) {
        comDrv_putc(*str);
        str++;
    }
}

char comDrv_getc(char *pchar) {
    if(rxb.count == 0) return FALSE;
    DisableUart1RxInterrupt();
    *pchar = rxb.data[rxb.head];
    rxb.count--;
    rxb.head = (rxb.head + 1) & INDEX_MASK;
    EnableUart1RxInterrupt();

    return TRUE;
}

void _int_(_UART_1_VECTOR) isr_uart(void) {
    if(IFS0bits.U1TXIF) {
        if(txb.count > 0) {
            U1TXREG = txb.data[txb.head];
            txb.head = (txb.head + 1) & INDEX_MASK;
            txb.count--;
        }

        if(txb.count == 0) {
            DisableUart1TxInterrupt();
        }

        IFS0bits.U1TXIF = 0;
    }

    if(IFS0bits.U1RXIF) {
        rxb.data[rxb.tail] = U1RXREG;
        rxb.tail = (rxb.tail + 1) & INDEX_MASK;
        if(rxb.count < BUF_SIZE) {
            rxb.count++;
        } else {
            rxb.head = (rxb.head + 1) & INDEX_MASK;
        }

        IFS0bits.U1RXIF = 0;
    }
}

void comDrv_config(unsigned int baudrate, char parity, unsigned int stopBits) {
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
    * Interrupt system
    */
    // Enables
    U1STAbits.UTXSEL = 0;  // Interrupt is generated when the transmit buffer contains at least one empty space
    U1STAbits.URXISEL = 0;  // Interrupt flag bit is set when a character is received
    IEC0bits.U1RXIE = 0;    // Receive interrupt enable
    IEC0bits.U1TXIE = 0;    // Transmit interrupt enable
    IEC0bits.U1EIE = 1;     // Error interrupt enable
    // Priority
    IPC6bits.U1IP = 4;      // ???
    // Flags
    IFS0bits.U1EIF = 0;     // Error interrupt flag
    IFS0bits.U1RXIF = 0;    // Receive interrupt flag
    IFS0bits.U1TXIF = 0;    // Transmit interrupt flag
}