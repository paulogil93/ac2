#include <detpic32.h>
#include "i2c.h"
#include "eeprom.h"

void init();        // Initial setup
void setupADC();    // Setup ADC
void setupT1();     // Setup Timer1
void setupT2();     // Setup Timer2
void setupT3();     // Setup Timer3
int getTemperature(int *temperature);
void setPWM(unsigned int dutyCycle);
int tension(int value);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);

static const int factorsT1[] = {1, 8, 64, 256};
static const int factorsTn[] = {1, 2, 4, 8, 16, 32, 64, 256};

volatile unsigned char value2display = 0;   // Global variable
volatile int voltage = 0;                   // Global variable
volatile int count = 0;                     // Number of samples saved on EEPROM

int main(void) {
    init();
    setupADC();
    setupT1();
    setupT2();
    setupT3();
    EnableInterrupts();
    int dutyCycle;
    static int num_samples = 0;
    int i = 0;
    char value;

    while(1) {
        char ctrl = PORTB & 0x0003;
        char opt = 0;
        
        switch(ctrl) {
            case 0:
                T1CONbits.TON = 1;
                T2CONbits.TON = 0;
                setPWM(0);  // LED OFF
                break;
            case 1:
                T1CONbits.TON = 0;
                T2CONbits.TON = 0;
                setPWM(100);    // LED ON(Max bright)
                break;
            case 2:
                T1CONbits.TON = 1;
                T2CONbits.TON = 0;
                setPWM(voltage);    // PWM generated by ADC
                break;
            case 3:
                // I2C mode - Temperature
                T1CONbits.TON = 0;
                T2CONbits.TON = 0;
                LATB = LATB & 0x0000;
                LATD = LATD & 0x0000;
                setPWM(0); // LED OFF

                printStr("\nR. Reset");
                printStr("\nL. Log");
                printStr("\nS. Show");
                printStr("\nOption: ");
                opt = getChar();

                switch(opt) {
                    case 'R':
                        eeprom_writeData(0, '\0');
                        printStr("\nEEPROM cleared...");
                        break;
                    case 'L':
                        T2CONbits.TON = 1;
                        printStr("\nLog mode started...");
                        break;
                    case 'S':
                        printStr("Show mode...");
                        num_samples = (int) eeprom_readData(0x0000);
                        for(i = 2; i <= num_samples; i++) {
                            printStr("Address: 0x");
                            printInt(i, 16);
                            printStr(": ");
                            value = eeprom_readData(i);
                            printInt10((int) value);
                        }
                        break;
                    default:
                        break;
                }
            default:
                T1CONbits.TON = 1;
                T2CONbits.TON = 0;
                dutyCycle = voltage * 3;
                setPWM(dutyCycle);
                break;  
        }
    }

    return 0;
}

// B port and display config
void init() {
    TRISB = TRISB & 0x0003;   // sets pins B8-B15 as output and B0-B1 as input
    TRISD = TRISD & 0xFF9F;   // sets pins D5-D6 as output
    LATB = LATB & 0x0000;     // resets pins B8-B15
    LATD = LATD & 0x0000;     // resets pins D5-D6
    i2c1_init(TC74_CLK_FREQ);
    spi2_setClock(EEPROM_CLOCK);
    spi2_init();
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
    int Fout = 4;    // Fout = 4Hz
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

// Timer2 - I2C receive rate
void setupT2() {
    int Fout = 4;   // Fout = 4Hz
    T2CONbits.TCKPS = 7;
    int K = factorsTn[T2CONbits.TCKPS];
    PR2 = PBCLK/(K*Fout) - 1;
    TMR2 = 0;
    T2CONbits.TON = 1;
    // Interrupt system
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;
    IFS0bits.T2IF = 0;
}

// Timer3 - Display refresh rate
void setupT3() {
    int Fout = 100;   // Fout = 100Hz
    T3CONbits.TCKPS = 2;
    int K = factorsTn[T3CONbits.TCKPS];
    PR3 = PBCLK/(K*Fout) - 1;
    TMR3 = 0;
    T3CONbits.TON = 1;
    // Interrupt system
    IPC3bits.T3IP = 2;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
    // PWM
    OC1CONbits.OCM = 6;         // PWM mode on OCx; fault pin disabled
    OC1CONbits.OCTSEL = 3;      // Use timer T3 as the time base for PWM generation
    OC1CONbits.ON = 1;          // Enable OC1 module
}

void _int_(_TIMER_1_VECTOR) isr_T1(void) {
    AD1CON1bits.ASAM = 1;
    IFS0bits.T1IF = 0;
}

void _int_(_TIMER_2_VECTOR) isr_T2(void) {
    int ack;
    int *tmp;
    static num_samples = 0;
    const int start_address = 0x02;
    
    if(count++ == 60) {
        printStr("\nSampling...");
        if(num_samples <= 63) {
            ack = getTemperature(&tmp);
            value2display = toBcd(tmp);
            eeprom_writeData((start_address+num_samples), tmp);
            num_samples++;
            eeprom_writeData(0x0000, num_samples);
            count = 0;
        } else {
            printStr("\nMaximum number of saved samples reached...");
        }
    }
    count++;
    IFS0bits.T2IF = 0;
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
    voltage = tension(average);
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

void setPWM(unsigned int dutyCycle) {
    // assert dutyCycle >= 0 && dutyCycle <= 100
    if(dutyCycle >= 0 && dutyCycle <= 100) {
        OC1RS = ((PR3+1) * dutyCycle) / 100;
    } else {
        printStr("Invalid duty-cycle value...");
        exit(0);
    }
}

int getTemperature(int *temperature) {
    int ack;
    i2c1_start();
    ack = i2c1_send(ADDR_WR);
    ack = i2c1_send(RTR);
    i2c1_start();
    ack = i2c1_send(ADDR_RD);

    if(ack != 0) {
        i2c1_stop();
        printStr("ERROR: ack != 0 after i2c1_send(ADDR_RD)");
        exit(0);
    }

    *temperature = i2c1_receive(I2C_NACK);
    i2c1_stop();
    return ack;
}