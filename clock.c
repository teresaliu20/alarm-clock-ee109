#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "clock.h"

void init_clock() {
	hoursTens = 0;
	hoursOnes = 0;
	minsTens = 0;
	minsOnes = 0;
	secsTens = 0;
	secsOnes = 0;

	alarmHoursTens = 2;
	alarmHoursOnes = 4;
	alarmMinsTens = 0;
	alarmMinsOnes = 0;

	daysIndex = 0;

	days[0] = "Sun.    ";
	days[1] = "Mon.    ";
	days[2] = "Tues.   ";
	days[3] = "Wed.    ";
	days[4] = "Thurs.  ";
	days[5] = "Fri.    ";
	days[6] = "Sat.    ";
}

void init_timer1(unsigned short m) {

  // set to CTC mode
  TCCR1B |= (1 << WGM12); 

  // enable timer interrupt
  TIMSK1 |= (1 << OCIE1A);

  // load 16-bit counter modulus
  OCR1A = m;

  // set prescalar to 1024 to update every 1s
  TCCR1B |= (1 << CS12) | (1 << CS10);

  sei();

}

void init_timer0(unsigned short m) {

	// set to CTC mode
	TCCR0A |= (1 << WGM01);

	// enable timer interrupt
	TIMSK0 |= (1 << OCIE0A);

	// load 8-bit counter modulus
	OCR0A = m;

	buzzCounter = 0;
}

void updateTime() {

	if (state == 0) {
		secsOnes++;
	}
	if (secsOnes == 10) {
		secsOnes = 0;
		secsTens++;
	}
	if (secsTens == 6) {
		secsTens = 0;
		minsOnes++;

		// minutes changed, write data to arduino for storage
		eeprom_update_byte((void*) 0, hoursTens);
		eeprom_update_byte((void*) 1, hoursOnes);
		eeprom_update_byte((void*) 2, minsTens);
		eeprom_update_byte((void*) 3, minsOnes);
		eeprom_update_byte((void*) 4, secsTens);
		eeprom_update_byte((void*) 5, secsOnes);
		eeprom_update_byte((void*) 6, daysIndex);
	}
	if (minsOnes == 10) {
		minsOnes = 0;
		minsTens++;
	}
	if (minsTens == 6) {
		minsTens = 0;
		hoursOnes++;
	}
	if (hoursOnes == 10) {
		hoursOnes = 0;
		hoursTens++;
	}
	if (hoursOnes == 4 && hoursTens == 2) {
		hoursTens = 0;
		hoursOnes = 0;
		daysIndex++;
	}
	if (daysIndex == 7) {
		daysIndex = 0;
	}
}


ISR(TIMER1_COMPA_vect) {
   // updates every 1s
	updateTime();
	if (state == 0) {
		// updates time and writes every 0.1s
		moveto(0,0);
		stringout(days[(int)daysIndex]);
		writedata(hoursTens + '0');
		writedata(hoursOnes + '0');
		writedata(':');
		writedata(minsTens + '0');
		writedata(minsOnes + '0');
		writedata(':');
		writedata(secsTens + '0');
		writedata(secsOnes + '0');
	}
}

ISR(TIMER0_COMPA_vect) {
	// turn buzzer on by flipping bit
	buzzCounter++;
	if (buzzCounter > 0) {
		PORTB ^= (1 << PB5);
	}
	if (buzzCounter == 5000) {
		// turn off timer
		TCCR0B &= ~((1 << CS11) | (1 << CS10));
		PORTB &= ~(1 << PB5);
	}
}





