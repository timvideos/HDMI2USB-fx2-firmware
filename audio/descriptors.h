/*
 * Copyright (C) 2017 Kyle Robbertze
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>

#include "date.h"
#include "descriptors_strings.h"

#include <ch9.h>
#include <ch9-extra.h>

#include <audio.h>

#ifndef DESCRIPTORS_H_
#define DESCRIPTORS_H_

struct usb_section {
	struct usb_config_descriptor config;
	/* ;;;;;;;;;;;;;;;;;;;;;;;;;; AUDIO ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; */
	struct usb_audio {
		struct usb_interface_assoc_descriptor assoc_interface;

		struct usb_audio_control {
            /* Standard audio control interface descriptor (alternate setting 0) */
            struct usb_interface_descriptor interface;
			/* Header functional descriptor */
            DECLARE_UAC_AC_HEADER_DESCRIPTOR(1) header;
            /* Input terminal descriptor */
            struct uac_input_terminal_descriptor input;
            /* Control feature unit descriptor (mute + volume) */
            DECLARE_UAC_FEATURE_UNIT_DESCRIPTOR(2) feature;
            /* Ouput terminal descriptor */
            struct uac1_output_terminal_descriptor output;
		} audiocontrol;

		struct usb_audio_stream1 {
            /* Standard audio control interface descriptor (alternate setting 1) */
            struct usb_interface_descriptor interface;
		} audiostream1;
		struct usb_audio_stream2 {
            /* Standard audio control interface descriptor (alternate setting 2) */
            struct usb_interface_descriptor interface;
			/* Header functional descriptor */
            struct uac1_as_header_descriptor header;
            /* Audio Streaming discrete interface */
            DECLARE_UAC_FORMAT_TYPE_I_DISCRETE_DESC(8) asinterface;
            /* Standard audio endpoint for streaming data */
            struct usb_audio_endpoint_descriptor endpoints[1];
            /* Audio specific endpoint for changing sample rate */
            struct uac_iso_endpoint_descriptor acendpoints[1];
		} audiostream2;
        /* Streaming with lower bandwidth 
         * 1x 50 bytes packet size
         * Subframe size 1
         * 8 bits per sample */
		struct usb_audio_stream2 audiostream3;
	} audio;
};

struct usb_descriptors {
	struct usb_device_descriptor device;
	struct usb_section highspeed;
	WORD fullspeed;
	struct usb_descriptors_strings strings;
};

__xdata __at(DSCR_AREA) struct usb_descriptors descriptors;

__code __at(DSCR_AREA+offsetof(struct usb_descriptors, device)) WORD dev_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, highspeed)) WORD highspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, fullspeed)) WORD fullspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, strings)) WORD dev_strings;

#endif // DESCRIPTORS_H_
