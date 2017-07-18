// Copyright (C) 2009 Ubixum, Inc. 
// Copyright (C) 2017 Kyle Robbertze <krobbertze@gmail.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

/** \file fx2lights.h
 * macros for turning lights on the EZ-USB development board, Atlys board
 * and Opsis board on and off.
 **/

/**
 *  Easy to make lights blink with these macros:
 *  \code
 *      WORD ct=0;
 *      BOOL on=FALSE;
 *      while (TRUE) {
 *          if (!ct) {
 *              on=!on;
 *              if (on) d2on(); else d2off();
 *          }
 *          ++ct;
 *      }
 *  \endcode
 **/

#ifndef FX2_LIGHTS_H
#define FX2_LIGHTS_H

#include "fx2types.h"
#include "fx2regs.h"

// Digilent Atlys - https://www.digilentinc.com/atlys
#ifdef BOARD_atlys
#define d1on() OED |= bmBIT7; IOD |= bmBIT7;
#define d1off() OED |= bmBIT7; IOD &= ~bmBIT7; 
#define d2on()  // D2 is not connected
#define d2off() 
#endif // BOARD_atlys

// FX2 "miniboard" board
#ifdef BOARD_fx2miniboard
#define d1on() OEA |= bmBIT0; IOA &= ~bmBIT0;
#define d1off() OEA |= bmBIT0; IOA |= bmBIT0; 
#define d2on() OEA |= bmBIT1; IOA &= ~bmBIT1;
#define d2off() OEA |= bmBIT1; IOA |= bmBIT1; 
#endif // BOARD_fx2miniboard

// Numato Opsis board - https://opsis.hdmi2usb.tv
#ifdef BOARD_opsis
#define d1on()  // D1 not connected
#define d1off()
#define d2on() OEE |= bmBIT5; IOE &= ~bmBIT5;
#define d2off() OEE |= bmBIT5; IOE |= bmBIT5; 
#endif // BOARD_opsis

#endif // FX2_LIGHTS_H
