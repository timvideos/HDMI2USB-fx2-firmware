#include "debug.h"
#include "softserial.h"

void soft_sio0_init( WORD baud_rate ) {
	usartInit();
}

void putchar(char c) {
	switch (c) {
	case '\r':
	case '\n':
        	usartSendByte('\n');
        	usartSendByte('\r');
		break;
	default:	
	        usartSendByte(c);
	}
}

char getchar(void) {
        return '0';
}
