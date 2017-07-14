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

/** \file audiodata.c
 * Contains definitions for USB audio communication between the FX2 chip and
 * the host
 */

#include <stdio.h>
#include <fx2regs.h>
#include <delay.h>
#include <eputils.h>

#include "audiodata.h"

#define SYNCDELAY SYNCDELAY4

/**
 * Returns the configuration. We only support cfg 1
 */
BYTE handle_get_configuration() {
    return 1;
}

/**
 * Sets the configuration. Successful if setting it to cfg 1, otherwise fail
 * as that is the only one supported
 */
BOOL handle_set_configuration(BYTE cfg) {
    return cfg==1 ? TRUE : FALSE;
}

/**
 * Returns the interface currently in use. This firmware only supports 0,0
 */
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc) {
    printf ( "Get Interface\n" );
    if (ifc==0) {*alt_ifc=0; return TRUE;} else { return FALSE;}
}

/**
 * Sets the interface in use. Only 0,0 is supported.
 */
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc) {
    printf ( "Set interface %d to alt: %d\n" , ifc, alt_ifc );

    if (ifc==0&&alt_ifc==0) {
        // SEE TRM 2.3.7
        // reset toggles
        RESETTOGGLE(0x02);
        RESETTOGGLE(0x86);
        // restore endpoints to default condition
        RESETFIFO(0x02);
        EP2BCL=0x80;
        SYNCDELAY;
        EP2BCL=0X80;
        SYNCDELAY;
        RESETFIFO(0x86);
        return TRUE;
    } else
        return FALSE;
}

BOOL handle_get_descriptor() {
    printf ( "Get Descriptor\n" );
    return FALSE;
}
