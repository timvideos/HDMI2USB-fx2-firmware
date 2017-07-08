/**
 * Copyright (C) 2009 Ubixum, Inc. 
 * Copyright (C) 2016 Matthew Iselin <miselin@google.com>
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

#include <fx2regs.h>
#include <fx2types.h>
#include <delay.h>

void init_lights(void) {
    // Set pins to port mode instead of GPIF master/slave mode
    IFCONFIG &= ~(bmIFCFGMASK);
    // No alternate functions
#ifdef BOARD_atlys
    PORTDCFG = 0x00;
#ifdef BOARD_fx2
    PORTACFG = 0x00;
#else
#ifdef BOARD_opsis
    PORTECFG = 0x00;
#endif // BOARD_opsis
#endif // BOARD_fx2
#endif // BOARD_atlys
}

// d2 is the LED labelled D1 in the Atlys set to the input transfer colour
// d1 is the LED labelled D1 on the FX2LP CY7C68013A mini-board
void d1_on(void) {
#ifdef BOARD_atlys
    // Set pin D0 as output
    OED |= bmBIT0;
    // Set D0
    IOD &= ~bmBIT0;
#else
#ifdef BOARD_fx2
    // Set pin A0 as output
    OEA |= bmBIT0;
    // Set A0
    IOA &= ~bmBIT0;
#endif // Opsis D1 not connected to FX2
#endif // BOARD_atlys
}

// d2 is the LED labelled D1 in the Atlys set to the output transfer colour
// d2 is the LED labelled D2 on the FX2LP CY7C68013A mini-board
// d2 is the LED labelled D2 on the Numato Opsis
void d2_on(void) {
#ifdef BOARD_atlys
    // Set pin D1 as output
    OED |= bmBIT1;
    // Set D1
    IOD &= ~bmBIT1;
#else
#ifdef BOARD_fx2
    // Set pin A1 as output
    OEA |= bmBIT1;
    // Set A1
    IOA &= ~bmBIT1;
#else
#ifdef BOARD_opsis
    // Set pin E5 as output
    OEE |= bmBIT5;
    // Set E5
    IOE &= ~bmBIT5;
#endif // BOARD_opsis
#endif // BOARD_fx2
#endif // BOARD_atlys
}

void d1_off(void) {
#ifdef BOARD_atlys
    // Set pin D0 as output
    OED |= bmBIT0;
    // Clear D0
    IOD |= bmBIT0;
#else
#ifdef BOARD_fx2
    // Set pin A0 as output
    OEA |= bmBIT0;
    // Clear A0
    IOA |= bmBIT0;
#endif // Opsis D1 not connected to FX2
#endif // BOARD_atlys
}

void d2_off(void) {
#ifdef BOARD_atlys
    // Set pin D1 as output
    OED |= bmBIT1;
    // Clear D1
    IOD |= bmBIT1;
#else
#ifdef BOARD_fx2
    // Set pin A1 as output
    OEA |= bmBIT1;
    // Clear A1
    IOA |= bmBIT1;
#else
#ifdef BOARD_opsis
    // Set pin E5 as output
    OEE |= bmBIT5;
    // Clear E5
    IOE |= bmBIT5;
#endif // BOARD_opsis
#endif // BOARD_fx2
#endif // BOARD_atlys
}

void main(void) {
    BYTE n=0;

    init_lights();

    while(TRUE) {
       switch(n) {
       case 0:
           d1_on();
           break;
       case 1:
           d2_on();
           break;
       case 2:
           d1_off();
           break;
       default:
           d2_off();
           break;
       }
       delay(250);
       n = (n + 1) % 4;
    }
}
