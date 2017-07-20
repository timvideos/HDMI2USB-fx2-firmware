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
#define usart_init()
#endif

#include <autovector.h>
#include <delay.h>
#include <eputils.h>
#include <fx2macros.h>
#include <fx2regs.h>
#include <fx2types.h>
#include <setupdat.h>

#include "fx2lights.h"
#include "audiodata.h"

#define SYNCDELAY SYNCDELAY4

volatile __bit got_sud;

void main() {
    d1on();
    d2on();
    /* Not using advanced endpoint controls */
    REVCTL=0;

    got_sud = FALSE;

    /* renumerate */
    RENUMERATE_UNCOND();

    SETCPUFREQ(CLK_48M);
    SETIF48MHZ();
    usart_init();

    USE_USB_INTS();
    ENABLE_SUDAV();
    ENABLE_USBRESET();
    //ENABLE_HISPEED();
    d1off();

    /* No valid endpoints by default */
    EP1INCFG = EP1OUTCFG = EP2CFG = EP4CFG = EP6CFG = EP8CFG = 0;

    /* Enable global interrupts */
    EA=1;
    d2off();

    printf("Initialisation complete\n");

    while(TRUE) {
        if (got_sud) {
            printf("Handle setup data\n");
            handle_setupdata();
            got_sud = FALSE;
        }
        /* ISO endpoint config type is 01 */
        if ((EP2CFG & bmTYPE) == bmTYPE0) {
            while (!(EP2468STAT & bmEP2FULL)) {
                d1on();
                /* Send known pattern 0x0400 */
                EP2BCH = 0x04;
                EP2BCL = 0x00;
            }
            d1off();
        }
    }
}

/**
 * Interrupt called when setup data is available.
 * Copied usb jt routines from usbjt.h
 */
void sudav_isr() __interrupt SUDAV_ISR {
    printf("sudav interrupt\n");
    got_sud=TRUE;
    CLEAR_SUDAV();
}

void usbreset_isr() __interrupt USBRESET_ISR {
    printf("reset interrupt\n");
    /* By default the USB is in full speed mode when reset */
    handle_hispeed(FALSE);
    CLEAR_USBRESET();
}

void hispeed_isr() __interrupt HISPEED_ISR {
    printf("hispeed interrupt\n");
    handle_hispeed(TRUE);
    CLEAR_HISPEED();
}
