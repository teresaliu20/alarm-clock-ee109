#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define FOSC 16000000 // clock frequency
#define BAUD 9600 // baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // value for UBRR0

volatile unsigned char startFlag;
volatile unsigned char validFlag;
volatile char buffer[5];
volatile int count;

void init_serial();
char rx_char();
void tx_char(char ch);
void transmitData(unsigned int tempdata);
// unsigned int* convertToNumbers(unsigned char buff[5]);
