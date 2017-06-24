#include <stddef.h>

#include "build/descriptors_stringtable.h"

#include <ch9.h>
#include <ch9-extra.h>

#ifndef DESCRIPTORS_H_
#define DESCRIPTORS_H_

struct usb_section {
	struct usb_config_descriptor config;
	struct usb_interface_descriptor interface;
	struct usb_endpoint_descriptor endpoints[2];
};

struct usb_descriptors {
	struct usb_device_descriptor device;
	struct usb_qualifier_descriptor qualifier;
	struct usb_section highspeed;
	struct usb_section fullspeed;
	struct usb_descriptors_stringtable stringtable;
};

__xdata __at(DSCR_AREA) struct usb_descriptors descriptors;

__code __at(DSCR_AREA+offsetof(struct usb_descriptors, device))		WORD dev_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, qualifier))	WORD dev_qual_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, highspeed))	WORD highspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, fullspeed))	WORD fullspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, stringtable))	WORD dev_strings;

#endif // DESCRIPTORS_H_
