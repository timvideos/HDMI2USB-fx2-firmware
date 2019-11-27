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
#include <libfx2/firmware/library/include/fx2delay.h>
#include <eputils.h>
#include <fx2macros.h>
#include <fx2regs.h>
#include <setupdat.h>

#include "audiodata.h"
#include "fx2lights.h"

volatile __bit got_sud;
extern uint8_t alt_setting;

void main() {
  d1on();
  d2on();
  /* Not using advanced endpoint controls */
  REVCTL = 0;

  got_sud = 0;

  /* renumerate */
  //fx2lib
  RENUMERATE_UNCOND();

  //fx2lib
  SETCPUFREQ(CLK_48M);
  //fx2lib
  SETIF48MHZ();
  usart_init();

  //fx2lib
  USE_USB_INTS();
  //fx2lib
  ENABLE_SUDAV();
  //fx2lib
  ENABLE_USBRESET();
  //fx2lib
  ENABLE_HISPEED();
  d1off();

  /**
     * No valid endpoints by default, thus clear the valid bit and set the
     * rest to default.
     * http://www.cypress.com/file/126446/download#G11.1057794
     * Invalid
     * Type: Bulk
     * Direction: out
     * Buffer: Double
     */
  EP2CFG = ~_VALID;
  SYNCDELAY;
  EP4CFG = ~_VALID;
  SYNCDELAY;
  EP6CFG = ~_VALID;
  SYNCDELAY;
  EP8CFG = ~_VALID;

  /* Enable global interrupts */
  EA = 1;
  /* Disable serial interrupts */
  ES0 = 0;
  d2off();

  printf("Initialisation complete\n");

  while (1) {
    if (got_sud) {
      printf("Handle setup data\n");
      handle_setupdata();
      got_sud = 0;
    }
    /* ISO endpoint config type is 01 in the enpoint configuration buffer */
    if ((EP8CFG & (_TYPE0 | _TYPE1)) == _TYPE0) {
      while (!(EP2468STAT & _EP8F)) {
        d1on();
        /* Send max data. Larger than 0x30 causes an EOVERFLOW */
        EP8BCH = 0x00;
        SYNCDELAY; EP8BCL = 0x30;
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
  got_sud = 1;
  //fx2lib
  CLEAR_SUDAV();
}

/**
 * Interrupt called when a reset is requested.
 */
void usbreset_isr() __interrupt USBRESET_ISR {
  /* By default the USB is in full speed mode when reset */
  handle_hispeed(0);
  //fx2lib
  CLEAR_USBRESET();
}

/**
 * Interrupt called when hispeed mode is requested.
 */
void hispeed_isr() __interrupt HISPEED_ISR {
  handle_hispeed(1);
  //fx2lib
  CLEAR_HISPEED();
}
