/********************************************
*
*  Name: Teresa Liu
*  Section: 2pm
*  Assignment: Lab 7
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "clock.h"
#include "encoder.h"
#include "lcd.h"

#define MASKBITS 0xf0

int main () {

  // initialize 
  init_lcd();
  init_clock();
  init_encoder();
  writecommand(1);
  init_timer1(25000);
  init_timer0(250);

  // read data from arduino on start up
  hoursTens = eeprom_read_byte((void *)0);
  hoursOnes = eeprom_read_byte((void *)1);
  minsTens = eeprom_read_byte((void *)2);
  minsOnes = eeprom_read_byte((void *)3);
  secsTens = eeprom_read_byte((void *)4);
  secsOnes = eeprom_read_byte((void *)5);
  daysIndex = eeprom_read_byte((void *)6);

  // checking the ranges, reseting clock if out of range
  if (secsOnes >= 10 || secsOnes < 0 ||
  	secsTens >= 6 || secsTens < 0 ||
  	minsOnes >= 10 || minsOnes < 0 ||
  	minsTens >= 6 || minsTens < 0 ||
  	hoursOnes >= 10 || hoursOnes < 0 ||
  	hoursTens >= 10 || hoursOnes < 0 ||
  	(hoursTens == 2 && hoursOnes > 4) ||
  	daysIndex >= 7 || daysIndex < 0)
  {
	hoursTens = 0;
	hoursOnes = 0;
	minsTens = 0;
	minsOnes = 0;
	secsTens = 0;
	secsOnes = 0;
	daysIndex = 0;

  }

	// enabling interrupts for Port C and Port D
	PCICR |= (1 << PCIE1) | (1 << PCIE2);

	// enabling interrupts for button and temperature sensor
	PCMSK1 |= (1 << PCINT11) | (1 << PCINT12) | (1 << PCINT13);

	sei();

	// enable pull-up resistor for button
	PORTC |= (1 << PC3);

	// enable pull-up resistor for temperature sensor
	PORTC |= (1 << PC5) | (1 << PC4);

	// set PORTB bit 3 (D11) to output for buzzer
	DDRB |= (1 << PB3);

	while (1) {

		ADCSRA |= (1 << ADSC); // start conversion
		while (ADCSRA & (1 << ADSC)) { // wait for conversion to complete

		}
		// if set alarm time equals current time, generate alarm
		if (alarmHoursTens == hoursTens &&
			alarmHoursOnes == hoursOnes &&
			alarmMinsTens == minsTens &&
			alarmMinsOnes == minsOnes &&
			secsOnes == 0 &&
			secsTens == 0) {

			// set prescalar to 64
			TCCR0A |= (1 << CS11) | (1 << CS10);
			
		}
	}
}

ISR(PCINT1_vect) {
	// detect button presses
	if (!(PINC & (1 << PC3))) {
		state++;
		// keep state from 0 to 5
		if (state == 6) {
			state = 0;
		}
		writecommand(1);
		if (state == 1) {
			stringout("Day: ");
			stringout(days[(int)daysIndex]);
		}
		else if (state == 2) {
			stringout("Hour: ");
			writedata(hoursTens + '0');
			writedata(hoursOnes + '0');
		}
		else if (state == 3) {
			secsOnes = 0;
			secsTens = 0;
			stringout("Min: ");
			writedata(minsTens + '0');
			writedata(minsOnes + '0');
		}
		else if (state == 4) {
			stringout("Alarm hour: ");
			writedata(alarmHoursTens + '0');
			writedata(alarmHoursOnes + '0');
		}
		else if (state == 5) {
			stringout("Alarm min: ");
			writedata(alarmMinsTens + '0');
			writedata(alarmMinsOnes + '0');
		}
		_delay_ms(1000);
	}
}








