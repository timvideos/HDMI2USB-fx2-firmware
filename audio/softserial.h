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

/** \file softserial.h
 * Allows printf to print to serial
 */

#ifndef SOFT_SERIAL_H
#define SOFT_SERIAL_H

#include <fx2types.h>

void soft_sio0_init(WORD baud_rate);
void soft_putchar(char c);
char soft_getchar(void);

#endif
