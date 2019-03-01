#include <detpic32.h>

void setup();
static const int factors[] = {1, 2, 4, 8, 16, 32, 64, 256};

void main(void) {
    setup();

    while(1) {
        while(IFS0bits.T3IF == 0);
        IFS0bits.T3IF = 0;
        putChar('.');
    }
}

void setup() {
    static int Fout = 2;    //Fout = 2Hz
    T3CONbits.TCKPS = 7;
    int K = factors[T3CONbits.TCKPS];
    PR3 = PBCLK/(K*Fout) - 1;
    TMR3 = 0;
    IFS0bits.T3IF = 0;
    T3CONbits.TON = 1;
}
