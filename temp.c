#include "temp.h"
#include "lcd.h"
#include "ds1631.h"
#include "serial.h"

void init_temperature() {

  // initialize temperature sensor to read
  ds1631_init();
  ds1631_conv();

  // intialize variables

  localTemp[0] = 0;
  localTemp[1] = 0;

  remoteTemp = 0;

}

void displayTemperature() {

	moveto(1,0);
	stringout("In: ");

	writedata(computeFahrenheit(localTemp) / 10 + '0');
	writedata(computeFahrenheit(localTemp) % 10 + '0');

	stringout("  Out: ");

	moveto(1,15);
}

unsigned int computeFahrenheit(unsigned int* c) {

	unsigned int f = (90/5) * c[0]; 

	if (c[1] == 0x80) {
		f = f + 45 ;
	}

	f = f / 10;
	f = f + 32;

	return f;
}

unsigned int bufferToFahrenheit(char* bf) {
	
	unsigned int f = 0;

	int b1 = bf[1];
	int b2 = bf[2];

	f += (b1 * 10) + b2;

	return f;
}




