#include <detpic32.h>

void delay(int ms);
int tension(int value);


int main(void) {
  TRISBbits.TRISB4 = 1;     // sets pin RB4 as input
  AD1PCFGbits.PCFG4 = 0;    // RB4 configured as analog input
  AD1CON1bits.SSRC = 7;
  AD1CON1bits.CLRASAM = 1;
  AD1CON3bits.SAMC = 16;
  AD1CON2bits.SMPI = 3;     // AD1CON2bits.SMPI = XX-1 => XX=1
  AD1CHSbits.CH0SA = 4;
  AD1CON1bits.ON = 1;

  while(1) {
    AD1CON1bits.ASAM = 1;         // starts conversion
    while(IFS1bits.AD1IF == 0);   // wait until end of conversion
    int *p = (int *)(&ADC1BUF0);
    int sum;
    int average;
    for(; p <= (int *)(&ADC1BUFF); p+=4 ){
      printInt( *p, 10 | 4 << 10 );
      putChar(' ');
    }
    sum = ADC1BUF0 + ADC1BUF1 + ADC1BUF2 + ADC1BUF3;
    average = (int) (sum/4);
    putChar('\n');
    printStr("Average: ");
    printInt(average, 10 | 4 << 10);
    putChar('\n');
    printStr("Voltage: ");
    printInt(tension(average), 10);
    putChar(' ');
    delay(500);
    IFS1bits.AD1IF = 0;           // reset bit AD1IF
  }

  return 0;
}

int tension(int value) {
  return (value*33+511)/1023;
}

void delay(int ms) {
   for(; ms > 0; ms--) {
      resetCoreTimer();
      while(readCoreTimer() < 20000);
   }
}
