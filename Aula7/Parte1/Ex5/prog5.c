#include <detpic32.h>

void init();        // Initial setup
void setupT1();     // Setup Timer1
void setupT3();     // Setup Timer3
int tension(int value);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);

static const int factorsT1[] = {1, 8, 64, 256};
static const int factorsTn[] = {1, 2, 4, 8, 16, 32, 64, 256};

volatile unsigned char value2display = 0; // Global variable

void main(void) {
    init();
    setupT1();
    setupT3();

    while(1);
}

void init() {
    TRISB = TRISB & 0x00FF;   // sets pins B8-B15 as output
    TRISD = TRISD & 0xFF9F;   // sets pins D5-D6 as output
    LATB = LATB & 0x0000;     // resets pins B8-B15
    LATD = LATD & 0x0000;     // resets pins D5-D6
    //ADC setup
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
    EnableInterrupts();
}

// Timer1 - ADC sample rate
void setupT1() {
    static int Fout = 4;    // Fout = 4Hz
    T1CONbits.TCKPS = 3;
    int K = factorsT1[T1CONbits.TCKPS];
    PR1 = PBCLK/(K*Fout) - 1;
    TMR1 = 0;
    T1CONbits.TON = 1;
    IPC1bits.T1IP = 2;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    EnableInterrupts();
}

//Timer3 - Display refresh rate
void setupT3() {
    static int Fout = 100;   //Fout = 100Hz
    T3CONbits.TCKPS = 2;
    int K = factorsTn[T3CONbits.TCKPS];
    PR3 = PBCLK/(K*Fout) - 1;
    TMR3 = 0;
    T3CONbits.TON = 1;
    IPC3bits.T3IP = 2;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
    EnableInterrupts();
}

void _int_(_TIMER_1_VECTOR) isr_T1(void) {
    AD1CON1bits.ASAM = 1;
    IFS0bits.T1IF = 0;
}

void _int_(_TIMER_3_VECTOR) isr_T3(void) {
    send2displays(value2display);
    IFS0bits.T3IF = 0;
}

void _int_(_ADC_VECTOR) isr_ADC(void) {
    int average = 0;
    int *p = (int *) (&ADC1BUF0);

    for(; p <= (int *) (&ADC1BUFF); p+=4) {
        average += *p;
    }
    
    average = (int) (average / 8);
    value2display = toBcd(tension(average));
    IFS1bits.AD1IF = 0;
}

int tension(int value) {
    return (value*33+511)/1023;
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
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