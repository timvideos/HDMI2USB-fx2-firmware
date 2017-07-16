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

/** \file softserial.c
 * Allows printf to print to serial
 */

#include "softserial.h"
#include "debug.h"

void soft_sio0_init( WORD baud_rate ) {
    usart_init();
}

void soft_putchar(char c) {
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
char soft_getchar(void) {
    return '0';
}
