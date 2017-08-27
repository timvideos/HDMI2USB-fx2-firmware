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

#ifdef FIRMWARE_in
#include "in/app.c"
#endif
#ifdef FIRMWARE_out
#include "out/app.c"
#endif
#ifdef FIRMWARE_fifo
#include "fifo/app.c"
#endif

volatile __bit got_sud;
extern BYTE alt_setting;

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

    /* Set all FIFO buffers to 8 bit by default */
    EP2FIFOCFG &= ~bmWORDWIDE;
    EP4FIFOCFG &= ~bmWORDWIDE;
    EP6FIFOCFG &= ~bmWORDWIDE;
    EP8FIFOCFG &= ~bmWORDWIDE;

    USE_USB_INTS();
    ENABLE_SUDAV();
    ENABLE_USBRESET();
    ENABLE_HISPEED();

    /**
     * No valid endpoints by default, thus clear the valid bit and set the
     * rest to default.
     * http://www.cypress.com/file/126446/download#G11.1057794
     * Invalid
     * Type: Bulk
     * Direction: out
     * Buffer: Double
     */
    EP2CFG = 0x7F;
    SYNCDELAY; EP4CFG = 0x7F;
    SYNCDELAY; EP6CFG = 0x7F;
    SYNCDELAY; EP8CFG = 0x7F;

    /* Enable global interrupts */
    EA = 1;
    /* Disable serial interrupts */
    ES0 = 0;
    d1off();

    TD_Init();
    d2off();

    printf("Initialisation complete\n");

    while(TRUE) {
        if (got_sud) {
            printf("Handle setup data\n");
            handle_setupdata();
            got_sud = FALSE;
        }
        TD_Poll();
    }
}

/**
 * Interrupt called when setup data is available.
 * Copied usb jt routines from usbjt.h
 */
void sudav_isr() __interrupt SUDAV_ISR {
    got_sud=TRUE;
    CLEAR_SUDAV();
}

/**
 * Interrupt called when a reset is requested.
 */
void usbreset_isr() __interrupt USBRESET_ISR {
    /* By default the USB is in full speed mode when reset */
    handle_hispeed(FALSE);
    CLEAR_USBRESET();
}

/**
 * Interrupt called when hispeed mode is requested.
 */
void hispeed_isr() __interrupt HISPEED_ISR {
    handle_hispeed(TRUE);
    CLEAR_HISPEED();
}
