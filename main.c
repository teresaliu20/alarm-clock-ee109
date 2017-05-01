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

	// set Pxx to output to enable for tri-state buffer
	DDRC |= (1 << PC1);

	// clear tri-state to output 0 to enable the buffer
	PORTC &= ~(1 << PC1);

	// set PORTB bit 3 and bit 4 to output for LEDs
	DDRB |= (1 << PB4) | (1 << PB3);
	// PB3 (D11) = red LED
	// PB4 (D12) = green LED

	// display initial temperature
	ds1631_temp(localTemp);
	displayTemperature();

	while (1) {
		// save previous temperature
		unsigned int prev = computeFahrenheit(localTemp);
		ds1631_temp(localTemp);
		// save current temperature
		unsigned int curr = computeFahrenheit(localTemp);

		// call routine to send characters to remote unit if tempurature changed
		if (state == 0 && curr != prev) {
			unsigned int data = computeFahrenheit(localTemp);
			displayTemperature();
			transmitData(data);
		}
		// check valid data flag for incoming data
		if (state == 0 && validFlag) {
			moveto(1,13);
			writedata(buffer[1] + '0');
			writedata(buffer[2] + '0');
			validFlag = 0;
		}
		// if remote temperature is higher, turn on red LED
		if (computeFahrenheit(localTemp) < bufferToFahrenheit(buffer)) {
			PORTB |= (1 << PB3);
			PORTB &= ~(1 << PB4);
		}
		// if local temperature is higher, turn on green LED
		else if (computeFahrenheit(localTemp) >= bufferToFahrenheit(buffer)) {
			PORTB |= (1 << PB4);
			PORTB &= ~(1 << PB3);
		}

		// if set alarm time equals current time, generate alarm
		if (state == 0 && 
			alarmHoursTens == hoursTens &&
			alarmHoursOnes == hoursOnes &&
			alarmMinsTens == minsTens &&
			alarmMinsOnes == minsOnes &&
			secsOnes == 1 &&
			secsTens == 0) {

			// set prescalar to 64 to turn on buzzer timer
			TCCR0B |= (1 << CS11) | (1 << CS10);
			alarmOn = 1;
		}
		if (alarmOn && !(PINC & (1 << PC2))) {
			// set snooze to true
			snooze = 1;
			// turn buzzer timer off
			TCCR0B &= ~((1 << CS11) | (1 << CS10));
			alarmOn = 0;
			PORTB &= ~(1 << PB5);
			buzzCounter = 0;
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
			stringout("Alarm Hour: ");
			writedata(alarmHoursTens + '0');
			writedata(alarmHoursOnes + '0');
		}
		else if (state == 5) {
			stringout("Alarm Min: ");
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







