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

#ifdef DEBUG
#include <stdio.h>

#include "debug.h"
#else
#define printf(...)
#endif

#include <libfx2/firmware/library/include/fx2delay.h>
#include <eputils.h>
#include <fx2regs.h>

#include "audiodata.h"

/**
 * Returns the configuration. We only have one configuration.
 */
uint8_t handle_get_configuration() {
  return 1;
}

/**
 * Sets the configuration. Successful if setting it to cfg 1, otherwise fail
 * as the descriptors only provide one configuration.
 */
uint8_t handle_set_configuration(uint8_t cfg) {
  return cfg == 1 ? 1: 0;
}

/* The current alternative setting */
uint8_t alt_setting = 0;
/**
 * Returns the interface currently in use.
 */
uint8_t handle_get_interface(uint8_t ifc, uint8_t* alt_ifc) {
  printf("Get Interface\n");
  if (ifc == 0 || ifc == 1) {
    *alt_ifc = alt_setting;
    return 1;
  }
  return 0;
}

/**
 * Sets the interface in use.
 * 0,0 - Default control
 * 1,0 - Streaming, no endpoint. This is used for when the device is not
 *       streaming
 * 1,1 - Streaming with endpoint 8.
 * See TRM section 2.3.7
 * http://www.cypress.com/file/126446/download#G5.1043536
 */
uint8_t handle_set_interface(uint8_t ifc, uint8_t alt_ifc) {
  printf("Set interface %d to alt: %d\n", ifc, alt_ifc);

  if (ifc == 0 && alt_ifc == 0) {
    alt_setting = 0;
    /*
         * Restore endpoints to default condition with valid bit cleared
         * (invalid, bulk, out, double)
         */
    EP8CFG = ~_VALID;
    SYNCDELAY;
    //fx2lib
    RESETFIFO(0x02);
    return 1;
  } else if (ifc == 1 && alt_ifc == 0) {
    alt_setting = 0;
    EP8CFG = ~_VALID;
    SYNCDELAY;
    //fx2lib
    RESETFIFO(0x02);
    /* reset toggles */
    SYNCDELAY;
    //fx2lib
    RESETTOGGLE(0x82);
    return 1;
  } else if (ifc == 1 && alt_ifc == 1) {
    alt_setting = 1;
    /* Reset audio streaming endpoint */
    EP8CFG = (bmVALID | bmDIR | bmTYPE0);
    SYNCDELAY;
    //fx2lib
    RESETFIFO(0x02);
    SYNCDELAY;
    //fx2lib
    RESETTOGGLE(0x82);
    return 1;
  }
  return 0;
}

/**
 * Descriptor requests are handled by fx2lib.
 */
uint8_t handle_get_descriptor() {
  printf("Get Descriptor\n");
  return 0;
}

/**
 * There are no vendor commands to handle
 */
uint8_t handle_vendorcommand(uint8_t cmd) {
  return 0;
}
