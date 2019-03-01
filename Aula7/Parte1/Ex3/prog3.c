#include <detpic32.h>

void setup();
static const int factors[] = {1, 2, 4, 8, 16, 32, 64, 256};

void main(void) {
    setup();
    while(1);
}

void setup() {
    static int Fout = 2;
    T3CONbits.TCKPS = 7;
    int K = factors[T3CONbits.TCKPS];
    PR3 = PBCLK/(K*Fout) - 1;           //See PDF folder for explanation
    TMR3 = 0;
    T3CONbits.TON = 1;
    IPC3bits.T3IP = 2;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
    EnableInterrupts();

}

void _int_(_TIMER_3_VECTOR) isr_T3(void) {
    static int count = 0;
    if(count % 2 == 0) putChar('.');
    count++;
    IFS0bits.T3IF = 0;
}
