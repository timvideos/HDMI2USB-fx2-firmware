// Copyright (C) 2017 Kyle Robbertze <krobbertze@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/** \file firmware.c
 * Initialises the USB audio firmware and handles interrupts
 */

#ifdef DEBUG
#include <stdio.h>
#include "debug.h"
#else
#define printf(...)
#endif

#include <fx2regs.h>
#include <fx2macros.h>
#include <delay.h>
#include <autovector.h>
#include <setupdat.h>
#include <eputils.h>

#include "fx2lights.h"
#include "audiodata.h"

#define SYNCDELAY SYNCDELAY4
#define REARMVAL 0x80
#define REARM() EP2BCL=REARMVAL

volatile __bit got_sud;

void main() {
    d1off();
    d2off();
    REVCTL=0; // not using advanced endpoint controls

    got_sud=FALSE;

    // renumerate
    RENUMERATE_UNCOND();

    SETCPUFREQ(CLK_48M);
    SETIF48MHZ();
    usart_init();

    USE_USB_INTS();
    ENABLE_SUDAV();
    ENABLE_HISPEED();
    ENABLE_USBRESET();
    d1on();

    /* only valid endpoint is 2 */
    EP2CFG = 0xA2; // 10100010
    SYNCDELAY;
    EP1OUTCFG &= ~bmVALID;
    SYNCDELAY;
    EP4CFG &= ~bmVALID;
    SYNCDELAY;

    /* arm ep2 */
    EP2BCL = 0x80; // write once
    SYNCDELAY;
    EP2BCL = 0x80; // do it again

    // make it so we enumerate
    EA=1; // global interrupt enable
    d2on();

    printf("Initialisation complete\n");

    while(TRUE) {
        if (got_sud) {
            printf("Handle setup data\n");
            handle_setupdata();
            got_sud=FALSE;
        }
    }
}

/**
 * There are no vendor commands to handle
 */
BOOL handle_vendorcommand(BYTE cmd) {
    return FALSE;
}

/**
 * Interrupt called when setup data is available.
 * Copied usb jt routines from usbjt.h
 */
void sudav_isr() __interrupt SUDAV_ISR {
    got_sud=TRUE;
    CLEAR_SUDAV();
}

void usbreset_isr() __interrupt USBRESET_ISR {
    handle_hispeed(FALSE);
    CLEAR_USBRESET();
}

void hispeed_isr() __interrupt HISPEED_ISR {
    handle_hispeed(TRUE);
    CLEAR_HISPEED();
}
