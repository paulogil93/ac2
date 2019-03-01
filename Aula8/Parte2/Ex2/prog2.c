#include <detpic32.h>
#include <limits.h>

void init();        // Initial setup
void setupADC();    // Setup ADC
void setupT1();     // Setup Timer1
void setupT3();     // Setup Timer3
void setupUART(unsigned int baudrate, char parity, unsigned int stopBits);
void putc(char byte2send);
void puts(char *str);
void setPWM(unsigned int dutyCycle);
int tension(int value);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);
void volt2UART(unsigned char value);

static const int factorsT1[] = {1, 8, 64, 256};
static const int factorsTn[] = {1, 2, 4, 8, 16, 32, 64, 256};

volatile unsigned char value2display = 0;   // Global variable
volatile int voltage = 0;                   // Global variable
volatile unsigned int voltMin = 0x3FF;
volatile unsigned int voltMax = 0;

void main(void) {
    init();
    setupADC();
    setupT1();
    setupT3();
    setupUART(115200, 'N', 1);
    EnableInterrupts();
    int dutyCycle;

    while(1) {
        char ctrl = PORTB & 0x0003;
        switch(ctrl) {
            case 0:
                T1CONbits.TON = 1;
                setPWM(0);  // LED OFF
                break;
            case 1:
                T1CONbits.TON = 0;
                setPWM(100);    // LED ON(Max bright)
                break;
            default:
                T1CONbits.TON = 1;
                dutyCycle = voltage * 3;
                setPWM(dutyCycle);
                break;
        }
    }
}

// B port and display config
void init() {
    TRISB = TRISB & 0x0003;   // sets pins B8-B15 as output and B0-B1 as input
    TRISD = TRISD & 0xFF9F;   // sets pins D5-D6 as output
    LATB = LATB & 0x0000;     // resets pins B8-B15
    LATD = LATD & 0x0000;     // resets pins D5-D6
}

// ADC setup
void setupADC() {
    TRISBbits.TRISB4 = 1;     // sets pin RB4 as input
    AD1PCFGbits.PCFG4 = 0;    // RB4 configured as analog input
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = 7;     // AD1CON2bits.SMPI = XX-1 => XX=3
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
    // Interrupt system
    IPC6bits.AD1IP = 2;       // configure priority of A/D interrupts
    IEC1bits.AD1IE = 1;       // enable A/D interrupts
    IFS1bits.AD1IF = 0;       // clear A/D interrupt flag
}

// Timer1 - ADC sample rate
void setupT1() {
    static int Fout = 4;    // Fout = 4Hz
    T1CONbits.TCKPS = 3;
    int K = factorsT1[T1CONbits.TCKPS];
    PR1 = PBCLK/(K*Fout) - 1;
    TMR1 = 0;
    T1CONbits.TON = 1;
    // Interrupt system
    IPC1bits.T1IP = 2;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
}

//Timer3 - Display refresh rate
void setupT3() {
    static int Fout = 100;   //Fout = 100Hz
    T3CONbits.TCKPS = 2;
    int K = factorsTn[T3CONbits.TCKPS];
    PR3 = PBCLK/(K*Fout) - 1;
    TMR3 = 0;
    T3CONbits.TON = 1;
    // Interrupt system
    IPC3bits.T3IP = 2;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
    EnableInterrupts();
    // PWM
    OC1CONbits.OCM = 6;         // PWM mode on OCx; fault pin disabled
    OC1CONbits.OCTSEL = 3;      // Use timer T3 as the time base for PWM generation
    OC1CONbits.ON = 1;          // Enable OC1 module
}

// UART setup
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

// ADC interrupt handler
void _int_(_TIMER_1_VECTOR) isr_T1(void) {
    AD1CON1bits.ASAM = 1;
    IFS0bits.T1IF = 0;
}

// Timer3 interrupt handler
void _int_(_TIMER_3_VECTOR) isr_T3(void) {
    static int counter = 0;
    send2displays(value2display);

    if(++counter == 100) {
        counter = 0;
        puts("\nVoltage: ");
        volt2UART(toBcd(voltage));
    }

    IFS0bits.T3IF = 0;
}

// ADC interrupt handler
void _int_(_ADC_VECTOR) isr_ADC(void) {
    int average = 0;
    int *p = (int *) (&ADC1BUF0);

    for(; p <= (int *) (&ADC1BUFF); p+=4) {
        average += *p;
    }
    
    average = (int) (average / 8);
    voltage = tension(average);
    if(voltage >= voltMax) voltMax = voltage;
    if(voltage < voltMin) voltMin = voltage;
    value2display = toBcd(tension(average));
    IFS1bits.AD1IF = 0;
}

/*
* UART1 interrupt handler
* Interrupt is triggered when RX has at least one char
* steps:
* char -> URXISEL -> U1RXIF = 1 -> isr_uart1 -> U1RXIF = 0 -> wait for char
*/
void _int_(_UART_1_VECTOR) isr_uart1(void) {
    if(U1RXREG == 'L') {
        puts("\n*************");
        puts("\nVoltMin: ");
        volt2UART(toBcd(voltMin));
        puts("\nVoltMax: ");
        volt2UART(toBcd(voltMax));
        puts("\n*************");
    }
    IFS0bits.U1RXIF = 0;
}

int tension(int value) {
    return (value*33+511)/1023;
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

// Send voltage to UART1
void volt2UART(unsigned char value) {
    putc('0' + (value >> 4));
    putc('.');
    putc('0' + (value & 0x0F));
    putc('V');
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

void send2displays(unsigned char value) {
    static const char codes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                                 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

    static char displayFlag = 0;    // saves value between calls to the function
    int digit_low = value & 0x0F;
    int digit_high = value >> 4;

    if(displayFlag == 0) {
        LATDbits.LATD5 = 1;
        LATDbits.LATD6 = 0;
        LATB = (LATB & 0x0000) | codes[digit_low] << 8;
    } else {
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;
        LATB = (LATB & 0x0000) | codes[digit_high] << 8 | 0x8000;
    }

    displayFlag = !displayFlag;   // alternates between displays low/high
}

void setPWM(unsigned int dutyCycle) {
    // assert dutyCycle >= 0 && dutyCycle <= 100
    if(dutyCycle >= 0 && dutyCycle <= 100) {
        OC1RS = ((PR3+1) * dutyCycle) / 100;
    } else {
        printStr("Invalid duty-cycle value...");
        exit(0);
    }
}