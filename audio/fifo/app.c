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
 * device and the host. Supports USB IN from the endpoint 2 FIFO buffer.
 */

#include "fx2regs.h"

/**
 * Initialises the FIFO slave interface
 */
void TD_Init(void) {
    /* Use external clock for slave interface */
    IFCONFIG &= ~(bmIFCLKSRC | bmIFCLKOE);
    /* Enable slave FIFO mode */
    SYNCDELAY; IFCONFIG |= (bmIFCFG1 | bmIFCFG0);
    /* Use word wide data transfer */
    SYNCDELAY; EP2FIFOCFG |= bmWORDWIDE;
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
        EP1INCFG = EP1OUTCFG = EP2CFG = EP4CFG = EP6CFG = EP8CFG = 0;
        SYNCDELAY; RESETFIFO(0x02);
        return TRUE;
    } else if (ifc == 1 && alt_ifc == 0) {
        alt_setting = 0;
        EP1INCFG = EP1OUTCFG = EP2CFG = EP4CFG = EP6CFG = EP8CFG = 0;
        SYNCDELAY; RESETFIFO(0x02);
        /* reset toggles */
        SYNCDELAY; RESETTOGGLE(0x82);
        return TRUE;
    } else if (ifc == 1 && alt_ifc == 1) {
        alt_setting = 1;
        /* Reset audio streaming endpoint */
        EP2CFG |= (bmVALID | bmDIR | bmTYPE0);
        SYNCDELAY; EP1INCFG = EP1OUTCFG = EP4CFG = EP6CFG = EP8CFG = 0;
        SYNCDELAY; RESETFIFO(0x02);
        SYNCDELAY; RESETTOGGLE(0x82);
        return TRUE;
    }
    return FALSE;
}

void TD_Poll(void) {
    /* ISO endpoint config type is 01 in the enpoint configuration buffer */
    if ((EP2CFG & bmTYPE) == bmTYPE0) {
        while (!(EP2468STAT & bmEP2FULL)) {
            d1on();
            /* Send max data. Larger than 0x30 causes an EOVERFLOW */
            EP2BCH = 0x00;
            SYNCDELAY;
            EP2BCL = 0x30;
        }
        d1off();
    }
}
