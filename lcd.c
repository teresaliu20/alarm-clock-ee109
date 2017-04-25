/********************************************
*
*  Name: Teresa Liu
*  Section: 2pm
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

#define MASKBITS 0xf0

void writenibble(unsigned char);

/*
  init_lcd - Configure the I/O ports and send the initialization commands
*/
void init_lcd()
{
    // Set the DDR register bits for ports B and D
    DDRD |= 0xf0; // sets upper four bits of PortD to output
    DDRB |= (1 << 0); // sets PortB bit 0 to output for RS
    DDRB |= (1 << 1); // sets PortB bt 1 to output for E

    _delay_ms(15);              // Delay at least 15ms

    writenibble(0x30);          // Use writenibble to send 0011
    _delay_ms(5);               // Delay at least 4msec

    writenibble(0x30);          // Use writenibble to send 0011
    _delay_us(120);             // Delay at least 100usec

    writenibble(0x30);          // Use writenibble to send 0011, no delay needed

    writenibble(0x20);          // Use writenibble to send 0010
    _delay_ms(2);               // Delay at least 2ms
    
    writecommand(0x28);         // Function Set: 4-bit interface, 2 lines

    writecommand(0x0f);         // Display and cursor on

}

/*
  moveto - Move the cursor to the row (0 or 1) and column (0 to 15) specified
*/
void moveto(unsigned char row, unsigned char col)
{
    unsigned char address = 0;

    if (row == 1) {
      address |= (0x40);
    }
    address |= col;

    writecommand(0x80 + address);
}

/*
  stringout - Write the string pointed to by "str" at the current position
*/
void stringout(char *str)
{
    int i = 0;
    while (str[i] != '\0') { // null byte
      writedata(str[i]);
      i++;
    }
}

/*
  writecommand - Send the 8-bit byte "cmd" to the LCD command register
*/
void writecommand(unsigned char cmd)
{
  PORTB &= ~(1 << 0); // set register select to write to command
  writenibble(cmd); // write upper four bits of cmd
  writenibble(cmd << 4); // write lower four bits of cmd
  _delay_ms(2); // wait 2 msec
}

/*
  writedata - Send the 8-bit byte "dat" to the LCD data register
*/
void writedata(unsigned char dat)
{
  PORTB |= (1 << 0); // set register select to write to data 
  writenibble(dat); // write upper four bits of dat
  writenibble(dat << 4); // write lower four bits of dat
  _delay_ms(2); // wait 2 msec
}

/*
  writenibble - Send four bits of the byte "lcdbits" to the LCD
*/
void writenibble(unsigned char lcdbits)
{ 
  // copies lcdbits to upper 4 bits of Port D
  PORTD &= ~MASKBITS; // clears destination bits
  PORTD |= (lcdbits & MASKBITS); // puts lcdbits into LCD

  // creates a pulse to enable, 230 nsec
  PORTB |= (1 << 1); // 1
  PORTB |= (1 << 1); // 1
  PORTB &= ~(1 << 1); // 0
}
