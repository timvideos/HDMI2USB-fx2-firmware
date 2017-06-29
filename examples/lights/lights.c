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
#include <delay.h>

void init_lights(void) {
    // Set pins to port mode instead of GPIF master/slave mode
    IFCONFIG &= ~(bmIFCONFGMASK);
    PORTACFG = 0x00;
}

// d1 is the LED labelled D1 on the FX2LP CY7C68013A mini-board
void d1_on(void) {
    // Set pin A1 I/O
    OEA |= 0x01;
    // Enable output pin A1
    IOA &= ~0x01;
}

// d2 is the LED labelled D2 on the FX2LP CY7C68013A mini-board
void d2_on(void) {
    // Set pin A2 I/O
    OEA |= 0x02;
    // Enable output pin A2
    IOA &= ~0x02;
}

void d1_off(void) {
    // Set pin A1 I/O
    OEA |= 0x01;
    // Disable output pin A1
    IOA |= 0x01;
}

void d2_off(void) {
    // Set pin A2 I/O
    OEA |= 0x02;
    // Disable output pin A2
    IOA |= 0x02;
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
