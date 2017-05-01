#include "serial.h"

void init_serial() {
	// set baud rate
	UBRR0 = MYUBRR;
	// enable RX and TX
	UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
	// asynchronous, no parity, 1 stop bit, 8 data
	UCSR0C = (3 << UCSZ00); 
	// enable interrupts
	UCSR0B |= (1 << RXCIE0);

	startFlag = 0;
	validFlag = 0;
	count = 0;
	buffer[0] = '+';
	buffer[1] = '0';
	buffer[2] = '0';
	buffer[3] = '0';
	buffer[4] = '\n';

	sei();
}

char rx_char()
{
	// waits for receive complete flag to go high
	while ( !(UCSR0A & (1 << RXC0)) ) {}
	return UDR0;
}

void tx_char(char ch)
{
	// waits for transmitter data register empty
	while ((UCSR0A & (1<<UDRE0)) == 0) {}
	UDR0 = ch;
}

void transmitData(unsigned int tempdata) {
	tx_char('#');
	tx_char('+');
	tx_char(tempdata / 10 + '0');
	tx_char(tempdata % 10 + '0');
	tx_char('$');
}

// unsigned int* convertToNumbers(unsigned char* buff) {
// 	unsigned int* nums;
// 	sscanf(buff, "%d %d", &nums, &nums+1, &nums+2);
// }

ISR(USART_RX_vect) {
	char next = UDR0;

	// if data reads #, then restart
	if (next == '#') {
		startFlag = 1;
		count = 0;
		validFlag = 0;
	}
	// sequence has started
	else if (next == '$' && count > 0) {
		// indicates complete data in buffer
		validFlag = 1;
	}
	else if (startFlag && next == '+' || next == '-') {
		buffer[0] = next;
		count++;
	}
	// if sequence has started and next data byte is in range, store in buffer
	else if (startFlag && next >= '0' && next <= '9') {
		buffer[count] = next - '0';
		count++;
	}
	// if data is invalid after data read started 
	else if (startFlag && (next < '0' || next > '9' || next == '$' || count > 4)) {
		startFlag = 0;
		count = 0;
	}

}



