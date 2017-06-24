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
#include "descriptors.h"

__code __at(DSCR_AREA) struct usb_descriptors code_descriptors = {
	.device = {
		.bLength		= USB_DT_DEVICE_SIZE,
		.bDescriptorType	= USB_DT_DEVICE,
		.bcdUSB			= USB_BCD_V20,
		.bDeviceClass 		= USB_CLASS_VENDOR_SPEC,
		.bDeviceSubClass	= USB_SUBCLASS_VENDOR_SPEC,
		.bDeviceProtocol	= 0xff,
		.bMaxPacketSize0	= 64,
		.idVendor		= VID,
		.idProduct		= PID,
		.bcdDevice		= 0x0001,
		.iManufacturer		= USB_STRING_INDEX(0),
		.iProduct		= USB_STRING_INDEX(1),
		.iSerialNumber		= USB_STRING_INDEX_NONE,
		.bNumConfigurations	= 1,
	},
	.qualifier = {
		.bLength		= USB_DT_DEVICE_QUALIFIER_SIZE,
		.bDescriptorType	= USB_DT_DEVICE_QUALIFIER,
		.bcdUSB			= USB_BCD_V20,
		.bDeviceClass		= USB_CLASS_VENDOR_SPEC,
		.bDeviceSubClass	= USB_SUBCLASS_VENDOR_SPEC,
		.bDeviceProtocol	= 0xff,
		.bMaxPacketSize0	= 64,
		.bNumConfigurations	= 1,
		.bRESERVED		= 0,
	},
	.highspeed = {
		.config = {
			.bLength		= USB_DT_CONFIG_SIZE,
			.bDescriptorType	= USB_DT_CONFIG,
			.wTotalLength		= sizeof(descriptors.highspeed),
			.bNumInterfaces		= 1,
			.bConfigurationValue	= 1,
			.iConfiguration		= 0,
			.bmAttributes		= USB_CONFIG_ATT_ONE,
			.bMaxPower		= 0x32, // FIXME: ???
		},
		.interface = {
			.bLength		= USB_DT_INTERFACE_SIZE,
			.bDescriptorType	= USB_DT_INTERFACE,
			.bInterfaceNumber	= 0,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 2,
			.bInterfaceClass	= USB_CLASS_VENDOR_SPEC,
			.bInterfaceSubClass	= USB_SUBCLASS_VENDOR_SPEC,
			.bInterfaceProtocol	= USB_PROTOCOL_VENDOR_SPEC,
			.iInterface		= USB_STRING_INDEX(2),
		},
		.endpoints = {
			{
				.bLength		= USB_DT_ENDPOINT_SIZE,
				.bDescriptorType 	= USB_DT_ENDPOINT,
				.bEndpointAddress	= USB_ENDPOINT_NUMBER(0x2) | USB_DIR_OUT,
				.bmAttributes		= USB_ENDPOINT_XFER_BULK,
				.wMaxPacketSize		= 512,
				.bInterval		= 0,
			},
			{
				.bLength		= USB_DT_ENDPOINT_SIZE,
				.bDescriptorType 	= USB_DT_ENDPOINT,
				.bEndpointAddress	= USB_ENDPOINT_NUMBER(0x6) | USB_DIR_IN,
				.bmAttributes		= USB_ENDPOINT_XFER_BULK,
				.wMaxPacketSize		= 512,
				.bInterval		= 0,
			},
		},
	},
	.fullspeed = {
		.config = {
			.bLength		= USB_DT_CONFIG_SIZE,
			.bDescriptorType	= USB_DT_CONFIG,
			.wTotalLength		= sizeof(descriptors.fullspeed),
			.bNumInterfaces		= 1,
			.bConfigurationValue	= 1,
			.iConfiguration		= 0,
			.bmAttributes		= USB_CONFIG_ATT_ONE,
			.bMaxPower		= 0x32, // * 2 mA
		},
		.interface = {
			.bLength		= USB_DT_INTERFACE_SIZE,
			.bDescriptorType	= USB_DT_INTERFACE,
			.bInterfaceNumber	= 0,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 2,
			.bInterfaceClass	= USB_CLASS_VENDOR_SPEC,
			.bInterfaceSubClass	= USB_SUBCLASS_VENDOR_SPEC,
			.bInterfaceProtocol	= 0xff,
			.iInterface		= 3,
		},
		.endpoints = {
			{
				.bLength		= USB_DT_ENDPOINT_SIZE,
				.bDescriptorType 	= USB_DT_ENDPOINT,
				.bEndpointAddress	= USB_ENDPOINT_NUMBER(0x2) | USB_DIR_OUT,
				.bmAttributes		= USB_ENDPOINT_XFER_BULK,
				.wMaxPacketSize		= 64,
				.bInterval		= 0,
			},
			{
				.bLength		= USB_DT_ENDPOINT_SIZE,
				.bDescriptorType 	= USB_DT_ENDPOINT,
				.bEndpointAddress	= USB_ENDPOINT_NUMBER(0x6) | USB_DIR_IN,
				.bmAttributes		= USB_ENDPOINT_XFER_BULK,
				.wMaxPacketSize		= 64,
				.bInterval		= 0,
			},
		},
	},
	#include "descriptors_strings.inc"
};
