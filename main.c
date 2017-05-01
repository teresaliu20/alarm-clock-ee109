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
#include "ds1631.h"
#include "temp.h"
#include "serial.h"

int main () {

	// initializations
	init_lcd();
	init_clock();
	init_encoder();
	writecommand(1);
	init_timer1(15625);
	init_timer0(250);
	init_temperature();
	init_serial();

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

	// enable pull-up resistor for change time button
	PORTC |= (1 << PC3);

	// enable pull-up resistor for snooze button
	PORTC |= (1 << PC2);

	// enable pull-up resistor for temperature sensor
	PORTC |= (1 << PC5) | (1 << PC4);

	// set PORTB bit 5 (D13) to output for buzzer
	DDRB |= (1 << PB5);

	ds1631_temp(temp);
	displayTemperature();

	while (1) {

		// save previous temperature
		unsigned int prev = computeFahrenheit(temp);
		ds1631_temp(temp);
		// save current temperature
		unsigned int curr = computeFahrenheit(temp);

		// call routine to send characters to remote unit if tempurature changed
		if (curr != prev) {
			unsigned int data = computeFahrenheit(temp);
			displayTemperature();
			transmitData(data);
		}
		// check valid data flag for incoming data
		if (validFlag) {
			// char num1, num2, num3;
			// sscanf(buffer, "%hhd %hhd %hhd", &num1, &num2, &num3);
			validFlag = 0;
		}
		// if set alarm time equals current time, generate alarm
		if (state == 0 && 
			alarmHoursTens == hoursTens &&
			alarmHoursOnes == hoursOnes &&
			alarmMinsTens == minsTens &&
			alarmMinsOnes == minsOnes &&
			secsOnes == 1 &&
			secsTens == 0) {

			// set prescalar to 64 to turn on buzzer
			TCCR0B |= (1 << CS11) | (1 << CS10);
		}
		if (!(PINC & (1 << PC2))) {
			buzzCounter = -30000;
			PORTB &= ~(1 << PB5);
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
		else if (state == 0) {
			secsOnes = 0;
			secsTens = 0;
		}
		_delay_ms(1000);
	}
}







