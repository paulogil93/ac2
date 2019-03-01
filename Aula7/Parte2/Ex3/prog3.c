#include <detpic32.h>

void init();
void setPWM(unsigned int dutyCycle);
void setupT3();
static const int factorsTn[] = {1, 2, 4, 8, 16, 32, 64, 256};
volatile int value = 0;

void main(void) {
    init();
    setupT3();
    
    while(1) {}
}

void init() {
    LATDbits.LATD0 = 1;
    TRISDbits.TRISD0 = 0;
}

//Timer3 setup
void setupT3() {
    static int Fout = 100;   // Fout = 100Hz
    T3CONbits.TCKPS = 2;
    int K = factorsTn[T3CONbits.TCKPS];
    PR3 = PBCLK/(K*Fout) - 1;
    TMR3 = 0;
    T3CONbits.TON = 1;
    IPC3bits.T3IP = 2;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
    EnableInterrupts();
    //PWM
    OC1CONbits.OCM = 6;         // PWM mode on OCx; fault pin disabled
    OC1CONbits.OCTSEL = 3;      // Use timer T3 as the time base for PWM generation
    setPWM(90);
    OC1CONbits.ON = 1;          // Enable OC1 module
}

void _int_(_TIMER_3_VECTOR) isr_T3(void) {
    IFS0bits.T3IF = 0;
}

void setPWM(unsigned int dutyCycle) {
    // assert dutyCycle >= 0 && dutyCycle <= 100
    if(dutyCycle >= 0 && dutyCycle <= 100) {
        OC1RS = ((PR3+1) * dutyCycle) / 100;
    } else {
        printStr("Invalid duty-cycle value...");
    }
}