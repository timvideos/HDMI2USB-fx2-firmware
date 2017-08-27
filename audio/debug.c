// Copyright (C) 2009-2012 Chris McClelland
// Copyright (C) 2017 Kyle Robbertze <krobbertze@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/** \file debug.c
 * Implements writing to serial for debugging purposes
 */
#include <fx2regs.h>
#include "debug.h"

#define USART PD1
#define _USART _PD1
#define BAUD 32

/**
 * Initialises the usart interface. It supports output from the FX2 on pin D1
 */
void usart_init(void) {
    USART = 1;
    /* Enable output on pin D1 */
    OED |= bmBIT3;
}

/**
 * Bit banging serial output
 */
void usart_send_byte(BYTE c) {
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

/**
 * Send a byte encoded as hexadecimal
 */
void usart_send_byte_hex(BYTE byte) {
    __xdata BYTE ch;
    ch = (byte >> 4) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = byte & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
}

/**
 * Send a word width of data as hexadecimal
 */
void usart_send_word_hex(WORD word) {
    __xdata BYTE ch;
    ch = (word >> 12) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 8) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 4) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 0) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
}

/**
 * Send a long word as hexadecimal
 */
void usart_send_long_hex(DWORD word) {
    __xdata BYTE ch;
    ch = (word >> 28) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 24) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 20) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 16) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 12) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 8) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 4) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
    ch = (word >> 0) & 0x0F;
    ch += (ch < 10 ) ? '0' : 'A' - 10;
    usart_send_byte(ch);
}

/**
 * Send a string using bit banging output. Either \n or \r will send both
 * \n and \r for ease of programming
 */
void usart_send_string(const char *s) {
    while (*s) {
        switch (*s) {
            case '\r':
            case '\n':
                usart_send_newline();
                break;
            default:
                usart_send_byte(*s);
        }
        *s++;
    }
}

/**
 * Ease of use for sending a new line.
 */
void usart_send_newline(void) {
    usart_send_byte('\n');
    usart_send_byte('\r');
}

/**
 * The function that printf uses to send characters. This allows printf to
 * be used for debug messages.
 */
void putchar(char c) {
    switch (c) {
        case '\r':
        case '\n':
            usart_send_newline();
            break;
        default:    
            usart_send_byte(c);
    }
}

/**
 * Not implemented
 */
char getchar(void) {
    return '0';
}
