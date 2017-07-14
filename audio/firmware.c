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

#include <fx2regs.h>
#include <fx2macros.h>
#include <delay.h>
#include <serial.h>
#include <autovector.h>
#include <setupdat.h>
#include <eputils.h>

#include "fx2lights.h"
#include "audiodata.h"

#ifdef DEBUG
#include "debug.c"
#else
#define usart_send_string(...)
#endif

#define SYNCDELAY SYNCDELAY4
#define REARMVAL 0x80
#define REARM() EP2BCL=REARMVAL

volatile WORD bytes;
volatile __bit gotbuf;
volatile BYTE icount;
volatile __bit got_sud;
DWORD lcount;
__bit on;

void main() {
    REVCTL=0; // not using advanced endpoint controls

    d1on();
    on=0;
    lcount=0;
    got_sud=FALSE;
    icount=0;
    gotbuf=FALSE;
    bytes=0;

    // renumerate
    RENUMERATE_UNCOND(); 

    SETCPUFREQ(CLK_48M);
    SETIF48MHZ();
    sio0_init(57600); // Required for sending descriptors
    usart_init();

    USE_USB_INTS(); 
    ENABLE_SUDAV();
    ENABLE_SOF();
    ENABLE_HISPEED();
    ENABLE_USBRESET();

    // only valid endpoints is 2
    EP2CFG = 0xA2; // 10100010
    SYNCDELAY;
    EP1OUTCFG &= ~bmVALID;
    SYNCDELAY;
    EP4CFG &= ~bmVALID;
    SYNCDELAY;

    // arm ep2
    EP2BCL = 0x80; // write once
    SYNCDELAY;
    EP2BCL = 0x80; // do it again

    // make it so we enumerate
    EA=1; // global interrupt enable 
    usart_send_string("Initialisation complete\n");
    usart_send_string("Initialisation complete\n");

    d2on();

    while(TRUE) {
        if (got_sud) {
            handle_setupdata(); 
            got_sud=FALSE;
            //usart_send_string("Handled setupdata\n");
        }
    }
}

// value (low byte) = ep
#define VC_EPSTAT 0xB1

BOOL handle_vendorcommand(BYTE cmd) {
    switch (cmd) {
    case VC_EPSTAT:
        {         
            __xdata BYTE* pep= ep_addr(SETUPDAT[2]);
            //usart_send_string("ep ");
            //usart_send_byte_hex(*pep);
            //usart_send_newline();
            if (pep) {
                EP0BUF[0] = *pep;
                EP0BCH=0;
                EP0BCL=1;
                return TRUE;
            } 
        }
    default:
        //usart_send_string("Need to implement vendor command: ");
        //usart_send_byte_hex(cmd);
        //usart_send_newline();
    }
    return FALSE;
}

// copied usb jt routines from usbjt.h
void sudav_isr() __interrupt SUDAV_ISR {
    got_sud=TRUE;
    CLEAR_SUDAV();
}

__bit on1;
__xdata WORD sofct=0;
void sof_isr () __interrupt SOF_ISR __using 1 {
    ++sofct;
    if(sofct==8000) { // about 8000 sof interrupts per second at high speed
        on1=!on1;
        if (on1) {d1on();} else {d1off();}
        sofct=0;
    }
    CLEAR_SOF();
}

void usbreset_isr() __interrupt USBRESET_ISR {
    handle_hispeed(FALSE);
    CLEAR_USBRESET();
}

void hispeed_isr() __interrupt HISPEED_ISR {
    handle_hispeed(TRUE);
    CLEAR_HISPEED();
}
