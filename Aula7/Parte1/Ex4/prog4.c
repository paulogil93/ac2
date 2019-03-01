#include <detpic32.h>

void setupT1(); // Setup Timer1
void setupT3(); // Setup Timer3
static const int factorsT1[] = {1, 8, 64, 256};
static const int factorsTn[] = {1, 2, 4, 8, 16, 32, 64, 256};

void main(void) {
    setupT1();
    setupT3();
    while(1);
}

void setupT1() {
    static int Fout = 2;    // Fout = 2Hz
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

void setupT3() {
    static int Fout = 20;   //Fout = 20Hz
    T3CONbits.TCKPS = 4;
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
    putChar('1');
    IFS0bits.T1IF = 0;
}

void _int_(_TIMER_3_VECTOR) isr_T3(void) {
    putChar('3');
    IFS0bits.T3IF = 0;
}
