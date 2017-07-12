// Copyright (C) 2017 Kyle Robbertze
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/** \file descriptors.h
 * Describes the USB audio device
 */
#include <stddef.h>

#include "descriptors_strings.h"

#include <ch9.h>
#include <ch9-extra.h>
#include <audio.h>

#ifndef DESCRIPTORS_H_
#define DESCRIPTORS_H_

struct usb_section {
    struct usb_config_descriptor config;
    struct usb_interface_descriptor interface;
    struct usb_audio_endpoint_descriptor endpoints[1];
};

struct usb_descriptors {
    struct usb_device_descriptor device;
    struct usb_qualifier_descriptor qualifier;
    struct usb_section highspeed;
    struct usb_section fullspeed;
    struct usb_descriptors_strings strings;
};

__xdata __at(DSCR_AREA) struct usb_descriptors descriptors;

__code __at(DSCR_AREA+offsetof(struct usb_descriptors, device))     WORD dev_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, qualifier))  WORD dev_qual_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, highspeed))  WORD highspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, fullspeed))  WORD fullspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, strings))    WORD dev_strings;
#endif // DESCRIPTORS_H_
