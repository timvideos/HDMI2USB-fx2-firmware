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

/** \file debug.h
 * Details writing to serial for debugging purposes
 */

#ifndef DEBUG_H
#define DEBUG_H

void usart_init(void);
void usart_send_byte(uint8_t c);
void usart_send_byte_hex(uint8_t byte);
void usart_send_word_hex(uint16_t word);
void usart_send_long_hex(uint32_t word);
void usart_send_string(const char *s);
void usart_send_newline(void);
void putchar(char c);
char getchar(void);

#endif
