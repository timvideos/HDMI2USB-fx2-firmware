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
#include <fx2macros.h>
#include <fx2regs.h>
#include "makestuff.h"

#define P_D3 0xB3
#define BAUD 32

__sbit __at P_D3 USART; // USART slave send from port D3

void usartInit(void) {
    SETCPUFREQ(CLK_48M);
	USART = 1;
	OED |= 0xff;
}

void usartSendByte(uint8 c) {
	(void)c; /* argument passed in DPL */
	__asm
		mov a, dpl
		mov r1, #9
		clr c
	loop:
		mov _USART, c
		rrc a
		mov r0, #BAUD
		djnz r0, .
		nop
		djnz r1, loop

		;; Stop bit
		setb _USART
		mov r0, #BAUD
		djnz r0, .
	__endasm;
}

void usartSendString(const char *s) {
	while ( *s ) {
        switch (*s) {
            case '\r':
            case '\n':
                usartSendByte('\n');
                usartSendByte('\r');
                break;
            default:	
                usartSendByte(*s);
        }
        *s++;
	}
}

void main(void) {
    int i, j;
    usartInit();
    while (1) {
        usartSendString("This is the serial example for the HDMI2USB firmware\n");
        for (i = 0; i < 255; i++) // Delay to display message every two seconds
            for (j = 0; j < 5000; j++);
    }
}
