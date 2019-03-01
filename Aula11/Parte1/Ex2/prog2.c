#include <detpic32.h>

void spi2_setClock(unsigned int clock_freq) {
    SPI2BRG = (PBCLK + clock_freq) / (2 * clock_freq) - 1;
}