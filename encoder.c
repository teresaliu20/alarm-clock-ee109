#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "clock.h"
#include "encoder.h"

void init_encoder() {

	clockwise = 1;

	// enable pull-up resistor for rotary encoder
	PORTD |= (1 << PD2) | (1 << PD3);

	// enable interrupts for rotary encoder
	PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);

    // Determine the intial state
    bits = PINC;
    a = bits & (1 << PD2);
    b = bits & (1 << PD3);

    if (!b && !a)
	old_state = 0;
    else if (!b && a)
	old_state = 1;
    else if (b && !a)
	old_state = 2;
    else
	old_state = 3;

    new_state = old_state;

}


// ISR for rotary encoders on Port D
ISR(PCINT2_vect) {
	// read in the two bits from rotary encoder
	bits = PIND;
	a = bits & (1 << PD2);
	b = bits & (1 << PD3);

	// detect direction of turn
	if (old_state == 0) {
	    if (a) {
			new_state = 1;
			clockwise = 1;
	    }
	    else if (b) {
			new_state = 2;
			clockwise = 0;
	    }
	}
	else if (old_state == 1) {
	    if (!a) {
			new_state = 0;
			clockwise = 0;
	    }
	    else if (b) {
			new_state = 3;
			clockwise = 1;
	    }
	}
	else if (old_state == 2) {
	    if (a) {
			new_state = 3;
			clockwise = 0;
	    }
	    else if (!b) {
			new_state = 0;
			clockwise = 1;
	    }
	}
	else {   // old_state = 3
	    if (!a) {
			new_state = 2;
			clockwise = 1;
	    }
	    else if (!b) {
			new_state = 1;
			clockwise = 0;
	    }
	}
	changed = (old_state != new_state);
	old_state = new_state;

	// update values dependent on state
	if (changed) {
		if (state == 1) { // change day
			if (clockwise) {
				daysIndex++;
				if (daysIndex >= 7) {
					daysIndex = 0;
				}
			}
			else {
				daysIndex--;
				if (daysIndex < 0) {
					daysIndex = 6;
				}
			}
			writecommand(1);
			stringout("Day: ");
			stringout(days[(int)daysIndex]);
		}
		else if (state == 2) { // change hour
			if (clockwise) {
				hoursOnes++;
				if (hoursOnes > 4 && hoursTens == 2) {
					hoursTens = 0;
					hoursOnes = 0;
				}
				if (hoursOnes > 9) {
					hoursTens++;
					hoursOnes = 0;
				}
			}
			else {
				hoursOnes--;
				if (hoursOnes < 0 && hoursTens == 0) {
					hoursTens = 2;
					hoursOnes = 4;
				}
				if (hoursOnes < 0) {
					hoursTens--;
					hoursOnes = 9;
				}	
			}
			writecommand(1);
			stringout("Hour: ");
			writedata(hoursTens + '0');
			writedata(hoursOnes + '0');
		}
		else if (state == 3) { // change minute
			if (clockwise) {
				minsOnes++;
				if (minsOnes > 9) {
					minsTens++;
					minsOnes = 0;
				}
				if (minsTens >= 6) {
					minsOnes = 0;
					minsTens = 0;
				}
			}
			else {
				minsOnes--;
				if (minsTens == 0 && minsOnes < 0) {
					minsTens = 5;
					minsOnes = 9;
				}
				if (minsOnes < 0) {
					minsTens--;
					minsOnes = 9;
				}
			}
			writecommand(1);
			stringout("Min: ");
			writedata(minsTens + '0');
			writedata(minsOnes + '0');
		}
		else if (state == 4) { // change alarm hour
			if (clockwise) {
				alarmHoursOnes++;
				if (alarmHoursOnes > 4 && alarmHoursTens == 2) {
					alarmHoursTens = 0;
					alarmHoursOnes = 0;
				}
				if (alarmHoursOnes > 9) {
					alarmHoursTens++;
					alarmHoursOnes = 0;
				}
			}
			else {
				alarmHoursOnes--;
				if (hoursOnes < 0 && alarmHoursTens == 0) {
					alarmHoursTens = 2;
					alarmHoursOnes = 4;
				}
				if (alarmHoursOnes < 0) {
					alarmHoursTens--;
					alarmHoursOnes = 9;
				}	
			}
			writecommand(1);
			stringout("Alarm Hour: ");
			writedata(alarmHoursTens + '0');
			writedata(alarmHoursOnes + '0');
		}
		else if (state == 5) { // change minute
			if (clockwise) {
				alarmMinsOnes++;
				if (alarmMinsOnes > 9) {
					alarmMinsTens++;
					alarmMinsOnes = 0;
				}
				if (alarmMinsTens >= 6) {
					alarmMinsOnes = 0;
					alarmMinsTens = 0;
				}
			}
			else {
				alarmMinsOnes--;
				if (alarmMinsTens == 0 && alarmMinsOnes < 0) {
					alarmMinsTens = 5;
					alarmMinsOnes = 9;
				}
				if (alarmMinsOnes < 0) {
					alarmMinsTens--;
					alarmMinsOnes = 9;
				}
			}
			writecommand(1);
			stringout("Alarm Min: ");
			writedata(alarmMinsTens + '0');
			writedata(alarmMinsOnes + '0');
		}
	}
}


