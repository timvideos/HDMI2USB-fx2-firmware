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
#include <audio-extra.h>

#ifndef DESCRIPTORS_H_
#define DESCRIPTORS_H_

DECLARE_UAC_AC_HEADER_DESCRIPTOR(1);
DECLARE_UAC_FORMAT_TYPE_I_DISCRETE_DESC(1);

struct uac_control_section {
    struct usb_interface_descriptor standard;
    struct uac1_ac_header_descriptor_1 classspec;
};

struct usb_hispeed_section {
    struct usb_config_descriptor config;
    struct uac_control_section control;
    struct uac_input_terminal_descriptor input;
    struct uac1_output_terminal_descriptor output;
    struct usb_interface_descriptor streaming0;
    struct usb_interface_descriptor streaming1;
    struct uac1_as_header_descriptor streamheader;
    struct uac_format_type_i_discrete_descriptor_1 format;
    struct usb_audio_endpoint_descriptor endpoints[1];
    struct uac_iso_endpoint_descriptor isoendpoint;
};

struct usb_fullspeed_section {
    struct usb_config_descriptor config;
    struct uac_control_section control;
    struct uac_input_terminal_descriptor input;
    struct uac1_output_terminal_descriptor output;
    struct usb_interface_descriptor streaming0;
    struct uac1_as_header_descriptor streamheader;
};

struct usb_descriptors {
    struct usb_device_descriptor device;
    struct usb_qualifier_descriptor qualifier;
    struct usb_hispeed_section highspeed;
    struct usb_fullspeed_section fullspeed;
    struct usb_descriptors_strings strings;
};

__xdata __at(DSCR_AREA) struct usb_descriptors descriptors;

__code __at(DSCR_AREA+offsetof(struct usb_descriptors, device))     WORD dev_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, qualifier))  WORD dev_qual_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, highspeed))  WORD highspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, fullspeed))  WORD fullspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, strings))    WORD dev_strings;
#endif // DESCRIPTORS_H_
