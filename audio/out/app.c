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
 * device and the host. Supports USB OUT to the endpoint 8 FIFO buffer.
 */

#include <fx2types.h>
#include <fx2macros.h>
#include <fx2regs.h>

#define SLWR PA3
#define FIFOADD0 PA4
#define FIFOADD1 PA5
#define PKTEND PA6
#define FULL_FLAG PA7
#define FDL IOB
#define FDH IOD

void TD_Init(void) {
    /* Use internal 48MHz clock */
    IFCONFIG = (bmIFCLKSRC | bm3048MHZ);
    /* Enable auto out from computer to FIFO */
    SYNCDELAY; REVCTL = (bmNOAUTOARM | bmSKIPCOMMIT);
    /* Enable outputs for FIFO */
    SYNCDELAY; OEA = (bmBIT3 | bmBIT4 | bmBIT5 | bmBIT6);
    SYNCDELAY; OEB = 0xFF;
    SYNCDELAY; OED = 0xFF;
    /* Automatically commit packets to the FIFO */
    SYNCDELAY; EP8FIFOCFG |= bmAUTOOUT;
    /* Clear all FIFO control signals */
    SYNCDELAY; SLWR = 1;
    SYNCDELAY; PKTEND = 1;
}

extern BYTE alt_setting;

/**
 * Sets the interface in use.
 * 0,0 - Default control
 * 1,0 - Streaming, no endpoint. this is used for when the device is not
 *       streaming
 * 1,1 - Streaming with endpoint 8.
 * See TRM Section 2.3.7
 * http://www.cypress.com/file/126446/download#g5.1043536
 */
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc) {
    printf("Set interface %d to alt: %d\n", ifc, alt_ifc);

    if (ifc == 0 && alt_ifc == 0) {
        alt_setting = 0;
        /* restore endpoints to default condition (invalid, bulk, out, double)*/
        EP2CFG = 0x7F;
        SYNCDELAY; EP4CFG = 0x7F;
        SYNCDELAY; EP6CFG = 0x7F;
        SYNCDELAY; EP8CFG = 0x7F;
        SYNCDELAY; RESETFIFO(0x08);
        SYNCDELAY; RESETTOGGLE(0x88);
        return TRUE;
    } else if (ifc == 1 && alt_ifc == 0) {
        alt_setting = 0;
        EP2CFG = 0x7F;
        SYNCDELAY; EP4CFG = 0x7F;
        SYNCDELAY; EP6CFG = 0x7F;
        SYNCDELAY; EP8CFG = 0x7F;
        SYNCDELAY; RESETFIFO(0x08);
        SYNCDELAY; RESETTOGGLE(0x88);
        return TRUE;
    } else if (ifc == 1 && alt_ifc == 1) {
        alt_setting = 1;
        /* Reset audio streaming endpoint */
        EP8CFG = (bmVALID | bmTYPE0);
        SYNCDELAY; EP2CFG = 0x7F;
        SYNCDELAY; EP4CFG = 0x7F;
        SYNCDELAY; EP6CFG = 0x7F;
        SYNCDELAY; RESETFIFO(0x08);
        SYNCDELAY; RESETTOGGLE(0x88);
        /**
         * Arm endpoint 8, as OUT endpoints do not come up armed. Arm by
         * writing OUTPKTEND with skip=1 N times, where N is the buffering
         * depth (2)
         */
        SYNCDELAY; OUTPKTEND = 0x88;
        SYNCDELAY; OUTPKTEND = 0x88;
        return TRUE;
    }
    return FALSE;
}

void TD_Poll() {
    int position = 0;
    /* ISO endpoint config type is 01 in the enpoint configuration buffer */
    if ((EP8CFG & bmTYPE) == bmTYPE0) {
        /* Write to Endpoint 8 FIFO */
        FIFOADD0 = 1;
        FIFOADD1 = 1;
        SYNCDELAY3;
        while (!(EP8CS & bmEPEMPTY)) {
            d1on();
            /* FLAGS are active low */
            if (FULL_FLAG) {
                SLWR = 1;
            } else {
                FDL = EP8FIFOBUF[position++];
                FDH = EP8FIFOBUF[position++];
                SLWR = 0;
                SLWR = 1;
                usart_send_byte_hex(FDH);
                usart_send_byte_hex(FDL);
            }
            if (position > 511) {
                /* Rearm EP8OUT */
                printf("\n");
                EP8BCL = 0x80;
                position = 0;
            }
        }
        /* Signal end of packet */
        PKTEND = 0;
        SYNCDELAY3;
        PKTEND = 1;
        d1off();
    }
}
