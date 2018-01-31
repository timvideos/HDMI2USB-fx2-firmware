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

/** \file app.c
 * Contains definitions for firmware specific USB traffic between the
 * device and the host. Supports USB IN from the endpoint 8 FIFO buffer.
 */

#include "fx2regs.h"

/**
 * Initialises the FIFO slave interface
 */
void TD_Init(void) {
    /* Return FIFO settings back to default */
    SYNCDELAY; PINFLAGSAB = 0x00;
    SYNCDELAY; PINFLAGSCD = 0x00;
    /* Make Full Flag active high */
    SYNCDELAY; FIFOPINPOLAR = bmBIT0;
    /* Use external clock and enable slave FIFO */
    IFCONFIG &= ~bmIFCLKSRC;
    SYNCDELAY; IFCONFIG = (bmIFCFG1 | bmIFCFG0);
    SYNCDELAY; REVCTL = (bmNOAUTOARM | bmSKIPCOMMIT);
    /* Reset auto out if set by other firmware */
    SYNCDELAY; EP8FIFOCFG &= ~bmAUTOOUT;
    /* Automatically commit packets to the FIFO */
    SYNCDELAY; EP8FIFOCFG |= bmAUTOIN;
    /* Use 8 bit wide FIFO */
    SYNCDELAY; EP8FIFOCFG &= ~bmWORDWIDE;
}

extern BYTE alt_setting;

/**
 * Sets the interface in use.
 * 0,0 - Default control
 * 1,0 - Streaming, no endpoint. This is used for when the device is not
 *       streaming
 * 1,1 - Streaming with endpoint 2.
 * SEE TRM 2.3.7
 */
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc) {
    printf("Set interface %d to alt: %d\n", ifc, alt_ifc);

    if (ifc == 0 && alt_ifc == 0) {
        alt_setting = 0;
        /* restore endpoints to default condition */
        EP2CFG = 0x7F;
        SYNCDELAY; EP4CFG = 0x7F;
        SYNCDELAY; EP6CFG = 0x7F;
        SYNCDELAY; EP8CFG = 0x7F;
        SYNCDELAY; RESETFIFO(0x08);
        return TRUE;
    } else if (ifc == 1 && alt_ifc == 0) {
        alt_setting = 0;
        EP2CFG = 0x7F;
        SYNCDELAY; EP4CFG = 0x7F;
        SYNCDELAY; EP6CFG = 0x7F;
        SYNCDELAY; EP8CFG = 0x7F;
        SYNCDELAY; RESETFIFO(0x08);
        /* reset toggles */
        SYNCDELAY; RESETTOGGLE(0x88);
        return TRUE;
    } else if (ifc == 1 && alt_ifc == 1) {
        alt_setting = 1;
        /* Reset audio streaming endpoint to IN, ISOC, x2 buffer */
        EP8CFG = (bmVALID | bmDIR | bmTYPE0);
        SYNCDELAY; EP2CFG = 0x7F;
        SYNCDELAY; EP4CFG = 0x7F;
        SYNCDELAY; EP6CFG = 0x7F;
        SYNCDELAY; RESETFIFO(0x08);
        SYNCDELAY; RESETTOGGLE(0x88);
        SYNCDELAY; EP8AUTOINLENH = 0x20;
        SYNCDELAY; EP8AUTOINLENL = 0x00;
        TD_Init();
        return TRUE;
    }
    return FALSE;
}

void TD_Poll(void) {
    int i = 0;
    while (i < 512)
        usart_send_byte_hex(EP8FIFOBUF[i++]);
    printf("\n");
    RESETFIFO(0x08);

/* Data sent automatically when FIFO is full*/
}
