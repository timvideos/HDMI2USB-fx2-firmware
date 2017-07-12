/**
 * Copyright (C) 2009 Ubixum, Inc. 
 * Copyright (C) 2017 Kyle Robbertze <krobbertze@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **/
#ifndef FX2LIGHTS_H
#define FX2LIGHTS_H

#include <fx2types.h>
#include <fx2regs.h>

#ifdef BOARD_fx2miniboard
// FX2 Dev board lights
#define d1on() OEA |= bmBIT0; IOA &= ~bmBIT0;
#define d1off() OEA |= bmBIT0; IOA |= ~bmBIT0; 
#define d2on() OEA |= bmBIT1; IOA &= ~bmBIT1;
#define d2off() OEA |= bmBIT1; IOA |= ~bmBIT1; 
#else
#ifdef BOARD_opsis
// Opisis board
#define d1on()  // D1 not connected
#define d1off()
#define d2on() OEE |= bmBIT5; IOE &= ~bmBIT5;
#define d2off() OEE |= bmBIT5; IOE |= ~bmBIT5; 
#else
#include <lights.h> // Other boards
#endif // BOARD_opsis
#endif // BOARD_fx2miniboard
#endif // FX2LIGHTS_H
