/*
 * Copyright (C) 2017 Kyle Robbertze <krobbertze@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <delay.h>
#include <fx2macros.h>
#include <fx2regs.h>
#include <fx2types.h>

#define PD3 0xB3
#define BAUD 32

__sbit __at PD3 USART; // USART slave send from port D3

void usart_init(void) {
    SETCPUFREQ(CLK_48M);
	USART = 1;
	OED |= 0xff;
}

void usart_send_byte(BYTE c) {
	(void)c; /* argument passed in DPL */
	__asm
		mov a, dpl
		mov r1, #9      // Move 9 into r1 for loop to execute 8 times (byte)
		clr c
	loop:
		mov _USART, c   // Move c into USART register
		rrc a           // Rotate accumulator 1 bit right
		mov r0, #BAUD   // Indicate ready to send
		djnz r0, .      // Wait for r0 to be cleared
		nop
		djnz r1, loop   // Reduce r1 and start loop again

		setb _USART     // Stop bit
		mov r0, #BAUD   // Indicate ready to send
		djnz r0, .      // Wait to be sent
	__endasm;
}

void usart_send_string(const char *s) {
	while ( *s ) {
        switch (*s) {
            case '\r':
            case '\n':
                usart_send_byte('\n');
                usart_send_byte('\r');
                break;
            default:	
                usart_send_byte(*s);
        }
        *s++;
	}
}

void main(void) {
    usart_init();
    while (1) {
        usart_send_string("This is the serial example for the HDMI2USB firmware\n");
        delay(2000);
    }
}
