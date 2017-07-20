 //Copyright (C) 2009 Ubixum, Inc. 
 //Copyright (C) 2014 Tim 'mithro' Ansell
 //Copyright (C) 2017 Kyle Robbertze
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

/** \file descriptors.c
 * Describes the device according to Section 4 of the USB Audio Spec
 */

#include "descriptors.h"
#include "version_data.h"

__code __at(DSCR_AREA) struct usb_descriptors code_descriptors = {
    .device = {
        .bLength            = USB_DT_DEVICE_SIZE,
        .bDescriptorType    = USB_DT_DEVICE,
        .bcdUSB             = USB_BCD_V20,
        /* Class defined per interface */
        .bDeviceClass       = 0, 
        .bDeviceSubClass    = 0,
        /* Protocol defined per interface */
        .bDeviceProtocol    = 0,
        /* packet size is in bytes */
        .bMaxPacketSize0    = 8,
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
        /* Class defined per interface */
        .bDeviceClass       = 0,
        .bDeviceSubClass    = 0,
        /* Protocol defined per interface */
        .bDeviceProtocol    = 0,
        /* packet size is in kB */
        .bMaxPacketSize0    = 8,
        .bNumConfigurations = 1,
        /* Must be zero */
        .bRESERVED          = 0,
    },
    .highspeed = {
        .config = {
            .bLength                = USB_DT_CONFIG_SIZE,
            .bDescriptorType        = USB_DT_CONFIG,
            .wTotalLength           = sizeof(descriptors.highspeed),
            .bNumInterfaces         = 2,
            .bConfigurationValue    = 1,
            .iConfiguration         = 0,
            .bmAttributes           = USB_CONFIG_ATT_ONE,
            /* bMaxPower has a resolution of 2mA */
            .bMaxPower              = 0x32,
        },
        .control = {
            .standard = {
                .bLength            = USB_DT_INTERFACE_SIZE,
                .bDescriptorType    = USB_DT_INTERFACE,
                .bInterfaceNumber   = 0,
                .bAlternateSetting  = 0,
                .bNumEndpoints      = 0,
                .bInterfaceClass    = USB_CLASS_AUDIO,
                .bInterfaceSubClass = USB_SUBCLASS_AUDIOCONTROL,
                /* Must be zero */
                .bInterfaceProtocol = 0,
                /* Unused */
                .iInterface         = 0,
            },
            .classspec = {
                .bLength            = UAC_DT_AC_HEADER_SIZE(1),
                .bDescriptorType    = USB_DT_CS_INTERFACE,
                .bDescriptorSubtype = UAC_MS_HEADER,
                .bcdADC             = UAC_BCD_V10,
                .wTotalLength       =
                    sizeof(descriptors.highspeed.control.classspec) +
                    sizeof(descriptors.highspeed.input) +
                    sizeof(descriptors.highspeed.output),
                /* Number of streaming interfaces */
                .bInCollection      = 1,
                /* The first streaming interface belongs to this control */
                .baInterfaceNr[0]   = 1,
            },
        },
        .input = {
            .bLength            = UAC_DT_INPUT_TERMINAL_SIZE,
            .bDescriptorType    = USB_DT_CS_INTERFACE,
            .bDescriptorSubtype = UAC_INPUT_TERMINAL,
            .bTerminalID        = 1,
            .wTerminalType      = UAC_INPUT_TERMINAL_MICROPHONE,
            /* No associated terminals */
            .bAssocTerminal     = 0,
            /* Stereo channels */
            .bNrChannels        = 2,
            .wChannelConfig     = (UAC_CHANNEL_LEFT | UAC_CHANNEL_RIGHT),
            .iChannelNames      = USB_STRING_INDEX(2),
            /* Unused */
            .iTerminal          = 0,
        },
        .output = {
            .bLength            = UAC_DT_OUTPUT_TERMINAL_SIZE,
            .bDescriptorType    = USB_DT_CS_INTERFACE,
            .bDescriptorSubtype = UAC_OUTPUT_TERMINAL,
            .bTerminalID        = 2,
            .wTerminalType      = UAC_OUTPUT_TERMINAL_STREAMING,
            .bAssocTerminal     = 0,
            /* Connected to the input terminal */
            .bSourceID          = 1,
            .iTerminal          = 0,
        },
        .streaming0 = {
            .bLength            = USB_DT_INTERFACE_SIZE,
            .bDescriptorType    = USB_DT_INTERFACE,
            .bInterfaceNumber   = 1,
            .bAlternateSetting  = 0,
            .bNumEndpoints      = 0,
            .bInterfaceClass    = USB_CLASS_AUDIO,
            .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
            .bInterfaceProtocol = 0,
            .iInterface         = 0,
        },
        .streaming1 = {
            .bLength            = USB_DT_INTERFACE_SIZE,
            .bDescriptorType    = USB_DT_INTERFACE,
            .bInterfaceNumber   = 1,
            .bAlternateSetting  = 1,
            .bNumEndpoints      = 1,
            .bInterfaceClass    = USB_CLASS_AUDIO,
            .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
            .bInterfaceProtocol = 0,
            .iInterface         = 0,
        },
        .streamheader = {
            .bLength            = UAC_DT_AS_HEADER_SIZE,
            .bDescriptorType    = USB_DT_CS_INTERFACE,
            .bDescriptorSubtype = UAC_AS_GENERAL,
            .bTerminalLink      = 2,
            .bDelay             = 1,
            .wFormatTag         = UAC_FORMAT_TYPE_I_PCM,
        },
        .format = {
            .bLength            = UAC_FORMAT_TYPE_I_DISCRETE_DESC_SIZE(1),
            .bDescriptorType    = USB_DT_CS_INTERFACE,
            .bDescriptorSubtype = UAC_FORMAT_TYPE,
            .bFormatType        = UAC_FORMAT_TYPE_I,
            .bNrChannels        = 2,
            /* Bytes per audio subframe */
            .bSubframeSize      = 2,
            .bBitResolution     = 16,
            /* Frequencies supported */
            .bSamFreqType       = 1,
            /* 8000Hz */
            .tSamFreq[0]        = { 0x40, 0x1F, 0x00 },
        },
        .endpoints = {
            {
                .bLength            = USB_DT_ENDPOINT_AUDIO_SIZE,
                .bDescriptorType    = USB_DT_ENDPOINT,
                .bEndpointAddress   = USB_ENDPOINT_NUMBER(2) | USB_DIR_IN,
                /* Isynchronous endpoint, not shared */
                .bmAttributes       = 0x1,
                .wMaxPacketSize     = 32,
                .bInterval          = 1,
                .bRefresh           = 0,
                .bSynchAddress      = 0,
            },
        },
        .isoendpoint = {
            .bLength            = UAC_ISO_ENDPOINT_DESC_SIZE,
            .bDescriptorType    = USB_DT_CS_ENDPOINT,
            .bDescriptorSubtype = UAC_AS_GENERAL,
            .bmAttributes       = 0,
            /* Unused */
            .bLockDelayUnits    = 0,
            .wLockDelay         = 0,
        },
    },
    .fullspeed = {
        .config = {
            .bLength                = USB_DT_CONFIG_SIZE,
            .bDescriptorType        = USB_DT_CONFIG,
            .wTotalLength           = sizeof(descriptors.fullspeed),
            .bNumInterfaces         = 2,
            .bConfigurationValue    = 1,
            .iConfiguration         = 0,
            .bmAttributes           = USB_CONFIG_ATT_ONE,
            /* bMaxPower has a resolution of 2mA */
            .bMaxPower              = 0x32,
        },
        .control = {
            .standard = {
                .bLength            = USB_DT_INTERFACE_SIZE,
                .bDescriptorType    = USB_DT_INTERFACE,
                .bInterfaceNumber   = 0,
                .bAlternateSetting  = 0,
                .bNumEndpoints      = 0,
                .bInterfaceClass    = USB_CLASS_AUDIO,
                .bInterfaceSubClass = USB_SUBCLASS_AUDIOCONTROL,
                /* Must be zero */
                .bInterfaceProtocol = 0,
                /* Unused */
                .iInterface         = 0,
            },
            .classspec = {
                .bLength            = UAC_DT_AC_HEADER_SIZE(1),
                .bDescriptorType    = USB_DT_CS_INTERFACE,
                .bDescriptorSubtype = UAC_MS_HEADER,
                .bcdADC             = UAC_BCD_V10,
                .wTotalLength       =
                    sizeof(descriptors.fullspeed.control.classspec) +
                    sizeof(descriptors.fullspeed.input) + 
                    sizeof(descriptors.fullspeed.output),
                /* Number of streaming interfaces */
                .bInCollection      = 1,
                /* The first streaming interface belongs to this control */
                .baInterfaceNr[0]   = 1,
            },
        },
        .input = {
            .bLength            = UAC_DT_INPUT_TERMINAL_SIZE,
            .bDescriptorType    = USB_DT_CS_INTERFACE,
            .bDescriptorSubtype = UAC_INPUT_TERMINAL,
            .bTerminalID        = 1,
            .wTerminalType      = UAC_INPUT_TERMINAL_MICROPHONE,
            /* No associated terminals */
            .bAssocTerminal     = 0,
            /* Stereo channels */
            .bNrChannels        = 2,
            .wChannelConfig     = (UAC_CHANNEL_LEFT | UAC_CHANNEL_RIGHT),
            .iChannelNames      = USB_STRING_INDEX(2),
            /* Unused */
            .iTerminal          = 0,
        },
        .output = {
            .bLength            = UAC_DT_OUTPUT_TERMINAL_SIZE,
            .bDescriptorType    = USB_DT_CS_INTERFACE,
            .bDescriptorSubtype = UAC_OUTPUT_TERMINAL,
            .bTerminalID        = 2,
            .wTerminalType      = UAC_OUTPUT_TERMINAL_STREAMING,
            .bAssocTerminal     = 0,
            /* Connected to the input terminal */
            .bSourceID          = 1,
            .iTerminal          = 0,
        },
        .streaming0 = {
            .bLength            = USB_DT_INTERFACE_SIZE,
            .bDescriptorType    = USB_DT_INTERFACE,
            .bInterfaceNumber   = 1,
            .bAlternateSetting  = 0,
            .bNumEndpoints      = 0,
            .bInterfaceClass    = USB_CLASS_AUDIO,
            .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
            .bInterfaceProtocol = 0,
            .iInterface         = 0,
        },
        .streaming1 = {
            .bLength            = USB_DT_INTERFACE_SIZE,
            .bDescriptorType    = USB_DT_INTERFACE,
            .bInterfaceNumber   = 1,
            .bAlternateSetting  = 1,
            .bNumEndpoints      = 1,
            .bInterfaceClass    = USB_CLASS_AUDIO,
            .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
            .bInterfaceProtocol = 0,
            .iInterface         = 0,
        },
        .streamheader = {
            .bLength            = UAC_DT_AS_HEADER_SIZE,
            .bDescriptorType    = USB_DT_CS_INTERFACE,
            .bDescriptorSubtype = UAC_AS_GENERAL,
            .bTerminalLink      = 2,
            .bDelay             = 1,
            .wFormatTag         = UAC_FORMAT_TYPE_I_PCM,
        },
        .format = {
            .bLength            = UAC_FORMAT_TYPE_I_DISCRETE_DESC_SIZE(1),
            .bDescriptorType    = USB_DT_CS_INTERFACE,
            .bDescriptorSubtype = UAC_FORMAT_TYPE,
            .bFormatType        = UAC_FORMAT_TYPE_I,
            .bNrChannels        = 2,
            /* Bytes per audio subframe */
            .bSubframeSize      = 2,
            .bBitResolution     = 16,
            /* Frequencies supported */
            .bSamFreqType       = 1,
            /* 8000Hz */
            .tSamFreq[0]        = { 0x00, 0x1F, 0x40 },
        },
        .endpoints = {
            {
                .bLength            = USB_DT_ENDPOINT_AUDIO_SIZE,
                .bDescriptorType    = USB_DT_ENDPOINT,
                .bEndpointAddress   = USB_ENDPOINT_NUMBER(2) | USB_DIR_IN,
                /* Isynchronous endpoint, not shared */
                .bmAttributes       = 0x1,
                .wMaxPacketSize     = 16,
                .bInterval          = 1,
                .bRefresh           = 0,
                .bSynchAddress      = 0,
            },
        },
        .isoendpoint = {
            .bLength            = UAC_ISO_ENDPOINT_DESC_SIZE,
            .bDescriptorType    = USB_DT_CS_ENDPOINT,
            .bDescriptorSubtype = UAC_AS_GENERAL,
            .bmAttributes       = 0,
            /* Unused */
            .bLockDelayUnits    = 0,
            .wLockDelay         = 0,
        },
    },
    #include "descriptors_strings.inc"
};
