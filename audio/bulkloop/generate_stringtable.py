#!/usr/bin/python3

"""
Generate a C file containing strings in the format needed by the USB descriptors.

Put the following in your Makefile
-------------------------------------------
include fx2-cdesc.mk
-------------------------------------------
"""

import sys

strings = [x.strip() for x in sys.stdin.readlines()]

if sys.argv[1] == "--header":
	print("""\
// This is an auto-generated file!
#include <ch9.h>
#include <ch9-extra.h>

#ifndef DESCRIPTORS_STRINGTABLE_H_
#define DESCRIPTORS_STRINGTABLE_H_

struct usb_descriptors_stringtable {
	struct usb_string_lang {
		__u8 bLength;
		__u8 bDescriptorType;
		__le16 wData[1];
	} language;""")
	for i, string in enumerate(strings):
		print("""\
	struct usb_string_%(i)i {
		__u8 bLength;
		__u8 bDescriptorType;
		__le16 wData[%(l)i];
	} string%(i)i;""" % {'l': len(string), 'i': i})
	print("""\
};

#endif // DESCRIPTORS_STRINGTABLE_H_
""")

if sys.argv[1] == "--cfile":
	print("""\
	.stringtable = {
		// English language header
		.language = {
			.bLength = sizeof(struct usb_string_lang),
			.bDescriptorType = USB_DT_STRING,
			.wData = { 0x0409 }, // 0x0409 is English
		},""")
	for i, string in enumerate(strings):
		d = ["((__le16)('%s'))" % s for s in string]

		print("""\
		// "%(s)s"
		.string%(i)i = {
			.bLength = sizeof(struct usb_string_%(i)i),
			.bDescriptorType = USB_DT_STRING,
			.wData = {%(d)s},
		},""" % {
		's': string,
		'i': i,
		'l': len(string),
		'd': ", ".join(d),
		})
	print("""\
	},
""")
