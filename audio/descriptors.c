/**
 * Copyright (C) 2009 Ubixum, Inc. 
 * Copyright (C) 2014 Tim 'mithro' Ansell
 * Copyright (C) 2017 Kyle Robbertze
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **/

#include "descriptors.h"
#include "date.h"

__code __at(DSCR_AREA) struct usb_descriptors code_descriptors = {
    .device = {
        .bLength            = USB_DT_DEVICE_SIZE,
        .bDescriptorType    = USB_DT_DEVICE,
        .bcdUSB             = USB_BCD_V20,
        .bDeviceClass       = 0, // Class and protocol defined per interface
        .bDeviceSubClass    = 0,
        .bDeviceProtocol    = 0,
        .bMaxPacketSize0    = 64, // kB
        .idVendor           = VID,
        .idProduct          = PID,
        .bcdDevice          = DID,
        .iManufacturer      = USB_STRING_INDEX(0),
        .iProduct           = USB_STRING_INDEX(1),
        .iSerialNumber      = USB_STRING_INDEX_NONE,
        .bNumConfigurations = 1,
    },
    .qualifier = {
        .bLength            = USB_DT_DEVICE_QUALIFIER_SIZE,
        .bDescriptorType    = USB_DT_DEVICE_QUALIFIER,
        .bcdUSB             = USB_BCD_V20,
        .bDeviceClass       = 0, // Class and protocol defined per interface 
        .bDeviceSubClass    = 0,
        .bDeviceProtocol    = 0,
        .bMaxPacketSize0    = 64,
        .bNumConfigurations = 1,
        .bRESERVED          = 0, // Must be zero
    },
    .highspeed = {
        .config = {
            .bLength                = USB_DT_CONFIG_SIZE,
            .bDescriptorType        = USB_DT_CONFIG,
            .wTotalLength           = sizeof(descriptors.highspeed),
            .bNumInterfaces         = 1,
            .bConfigurationValue    = 1,
            .iConfiguration         = 0,
            .bmAttributes           = USB_CONFIG_ATT_ONE,
            .bMaxPower              = 0x32, // * 2 mA
        },
        .interface = {
            .bLength            = USB_DT_INTERFACE_SIZE,
            .bDescriptorType    = USB_DT_INTERFACE,
            .bInterfaceNumber   = 0,
            .bAlternateSetting  = 0,
            .bNumEndpoints      = 1,
            .bInterfaceClass    = USB_CLASS_AUDIO,
            .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
            .bInterfaceProtocol = 0, // Must be zero
            .iInterface         = USB_STRING_INDEX(2),
        },
        .endpoints = {
            {
                .bLength            = USB_DT_ENDPOINT_AUDIO_SIZE,
                .bDescriptorType    = USB_DT_ENDPOINT,
                .bEndpointAddress   = USB_ENDPOINT_NUMBER(0x2) | USB_DIR_OUT,
                .bmAttributes       = 0x1, // Isynchronous endpoint
                .wMaxPacketSize     = 512,
                .bInterval          = 1,
                .bRefresh           = 0,
                .bSynchAddress      = 0,
            },
        },
    },
    .fullspeed = {
        .config = {
            .bLength                = USB_DT_CONFIG_SIZE,
            .bDescriptorType        = USB_DT_CONFIG,
            .wTotalLength           = sizeof(descriptors.fullspeed),
            .bNumInterfaces         = 1,
            .bConfigurationValue    = 1,
            .iConfiguration         = 0,
            .bmAttributes           = USB_CONFIG_ATT_ONE,
            .bMaxPower              = 0x32, // * 2 mA
        },
        .interface = {
            .bLength            = USB_DT_INTERFACE_SIZE,
            .bDescriptorType    = USB_DT_INTERFACE,
            .bInterfaceNumber   = 0,
            .bAlternateSetting  = 0,
            .bNumEndpoints      = 1,
            .bInterfaceClass    = USB_CLASS_AUDIO,
            .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
            .bInterfaceProtocol = 0, // Must be zero
            .iInterface         = USB_STRING_INDEX(2),
        },
        .endpoints = {
            {
                .bLength            = USB_DT_ENDPOINT_AUDIO_SIZE,
                .bDescriptorType    = USB_DT_ENDPOINT,
                .bEndpointAddress   = USB_ENDPOINT_NUMBER(0x2) | USB_DIR_OUT,
                .bmAttributes       = 0x1, // Isynchronous endpoint
                .wMaxPacketSize     = 512,
                .bInterval          = 1,
                .bRefresh           = 0,
                .bSynchAddress      = 0,
            },
        },
    },
    #include "descriptors_strings.inc"
};
