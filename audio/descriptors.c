
#include "descriptors.h"

__code __at(DSCR_AREA) struct usb_descriptors code_descriptors = {
    .device = {
        .bLength            = USB_DT_DEVICE_SIZE,
        .bDescriptorType    = USB_DT_DEVICE,
        .bcdUSB             = USB_BCD_V20,
        // FIXME:
        // This may be better as a miscellaneous device with an interface
        // association so that the same driver instance handles video and
        // audio capture
        // http://www.usb.org/developers/docs/whitepapers/iadclasscode_r10.pdf
        // http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf
        .bDeviceClass       = 0x0, // Audio only, info found at interface level
        .bDeviceSubClass    = 0x0,
        .bDeviceProtocol    = 0x0,
        .bMaxPacketSize0    = 64,
        .idVendor           = VID,
        .idProduct          = PID,
        .bcdDevice          = DID,
        .iManufacturer      = USB_STRING_INDEX(0),
        .iProduct           = USB_STRING_INDEX(1),
        .iSerialNumber      = USB_STRING_INDEX(2),
        .bNumConfigurations = 1
    },
    .highspeed = {
        .config = {
            .bLength            = USB_DT_CONFIG_SIZE,
            .bDescriptorType    = USB_DT_CONFIG,
            .wTotalLength       = sizeof(descriptors.highspeed),
            .bNumInterfaces     = 4,
            .bConfigurationValue    = 1,
            .iConfiguration     = USB_STRING_INDEX_NONE,
            .bmAttributes       = USB_CONFIG_ATT_ONE,
            .bMaxPower          = 500, // mA
        },
        .audio = {
            /* Interface association descriptor */
            .assoc_interface = {
                .bLength        = sizeof(struct usb_interface_assoc_descriptor),
                .bDescriptorType    = USB_DT_INTERFACE_ASSOCIATION,
                .bFirstInterface    = 2,
                .bInterfaceCount    = 2,
                .bFunctionClass     = USB_CLASS_AUDIO,
                .bFunctionSubClass  = 0x0,
                .bFunctionProtocol  = 0x0,
                .iFunction          = USB_STRING_INDEX(4),
            },
            .audiocontrol = {
                .interface = {
                    .bLength            = USB_DT_INTERFACE_SIZE,
                    .bDescriptorType    = USB_DT_INTERFACE,
                    .bInterfaceNumber   = 2,
                    .bAlternateSetting  = 0,
                    .bNumEndpoints      = 0,
                    .bInterfaceClass    = USB_CLASS_AUDIO,
                    .bInterfaceSubClass = USB_SUBCLASS_AUDIOCONTROL,
                    .bInterfaceProtocol = 0x0,
                    .iInterface         = USB_STRING_INDEX(4),
                },
                .header = {
                    .bLength            = UAC_DT_AC_HEADER_SIZE(1),
                    .bDescriptorType    = USB_DT_CS_INTERFACE,
                    .bDescriptorSubtype = UAC_MS_HEADER,
                    .bcdADC             = 0x0100,
                    .wTotalLength       = UAC_DT_AC_HEADER_SIZE(1) +
                                            UAC_DT_INPUT_TERMINAL_SIZE +
                                            UAC_DT_FEATURE_UNIT_SIZE(2) +
                                            UAC_DT_OUTPUT_TERMINAL_SIZE,
                    .bInCollection      = 0x1,
                    .baInterfaceNr[0]   = 0x3, // FIXME: Check this
                },
                .input = {
                    .bLength            = UAC_DT_INPUT_TERMINAL_SIZE,
                    .bDescriptorType    = USB_DT_CS_INTERFACE,
                    .bDescriptorSubtype = UAC_INPUT_TERMINAL,
                    .bTerminalID        = 0x1,
                    .wTerminalType      = UAC_INPUT_TERMINAL_MICROPHONE,
                    .bAssocTerminal     = 0x0,
                    .bNrChannels        = 0x2, // Stereo
                    .wChannelConfig     = 0x0000,
                    .iChannelNames      = 0x0,                      // Unused
                    .iTerminal          = USB_STRING_INDEX_NONE,    // Unused
                },
                .feature = {
                    .bLength            = UAC_DT_FEATURE_UNIT_SIZE(2),
                    .bDescriptorType    = USB_DT_CS_INTERFACE,
                    .bDescriptorSubtype = UAC_FEATURE_UNIT,
                    .bUnitID            = 0x2,
                    .bSourceID          = 0x1, // Associated with input mic
                    .bControlSize       = 0x2,
                    .bmaControls[0]     = (UAC_FU_MUTE | UAC_FU_VOLUME),
                    .bmaControls[1]     = 0x00, // link stereo channels
                    .bmaControls[2]     = 0x00,
                    .iFeature           = USB_STRING_INDEX_NONE,
                },
                .output = {
                    .bLength            = UAC_DT_OUTPUT_TERMINAL_SIZE,
                    .bDescriptorType    = USB_DT_CS_INTERFACE,
                    .bDescriptorSubtype = UAC_OUTPUT_TERMINAL,
                    .bTerminalID        = 0x3,
                    .wTerminalType      = UAC_TERMINAL_STREAMING,
                    .bAssocTerminal     = 0x0,
                    .bSourceID          = 0x2, // Connected to feature unit
                    .iTerminal          = USB_STRING_INDEX(1),
                },
            },
            .audiostream1 = {
                .interface = {
                    .bLength            = USB_DT_INTERFACE_SIZE,
                    .bDescriptorType    = USB_DT_INTERFACE,
                    .bInterfaceNumber   = 0x3,
                    .bAlternateSetting  = 0x0,
                    .bNumEndpoints      = 0x0,
                    .bInterfaceClass    = USB_CLASS_AUDIO,
                    .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
                    .bInterfaceProtocol = 0x0,
                    .iInterface         = USB_STRING_INDEX(4),
                },
            },
            .audiostream2 = {
                .interface = {
                    .bLength            = USB_DT_INTERFACE_SIZE,
                    .bDescriptorType    = USB_DT_INTERFACE,
                    .bInterfaceNumber   = 0x3,
                    .bAlternateSetting  = 0x2,
                    .bNumEndpoints      = 0x1,
                    .bInterfaceClass    = USB_CLASS_AUDIO,
                    .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
                    .bInterfaceProtocol = 0x0,
                    .iInterface         = USB_STRING_INDEX(4),
                },
                .header = {
                    .bLength            = UAC_DT_AS_HEADER_SIZE,
                    .bDescriptorType    = USB_DT_CS_INTERFACE,
                    .bDescriptorSubtype = UAC_AS_GENERAL,
                    .bTerminalLink      = 0x3,
                    .bDelay             = 0x1, // frame
                    .wFormatTag         = UAC_FORMAT_TYPE_I_PCM,
                },
                .asinterface = {
                    .bLength = UAC_FORMAT_TYPE_I_DISCRETE_DESC_SIZE(8),
                    .bDescriptorType    = USB_DT_CS_INTERFACE,
                    .bDescriptorSubtype = UAC_FORMAT_TYPE,
                    .bFormatType        = UAC_FORMAT_TYPE_I,
                    .bNrChannels        = 0x2,  // Stereo
                    .bSubframeSize      = 0x2,  // bytes per subframe
                    .bBitResolution     = 0x10, // 16 bits per sample
                    .bSamFreqType       = 0x8,  // frequencies supported
                    .tSamFreq           = { {0x00, 0xBB, 0x80}, // 48000 Hz
                                            {0x00, 0xAC, 0x44}, // 44100 Hz
                                            {0x00, 0x5D, 0xC0}, // 24000 Hz 
                                            {0x00, 0x56, 0x22}, // 22050 Hz 
                                            {0x00, 0x3E, 0x80}, // 16000 Hz 
                                            {0x00, 0x2E, 0xE0}, // 12000 Hz
                                            {0x00, 0x2B, 0x11}, // 11025 Hz
                                            {0x00, 0x1F, 0x40}}, // 8000 Hz
                },
                .endpoints = {
                    {
                        .bLength            = USB_DT_ENDPOINT_AUDIO_SIZE,
                        .bDescriptorType    = USB_DT_CS_ENDPOINT,
                        .bEndpointAddress   = USB_ENDPOINT_NUMBER(0x3) |
                                                USB_DIR_IN,
                        .bmAttributes       = (USB_ENDPOINT_XFER_ISOC |
                                                USB_ENDPOINT_SYNC_ASYNC),
                        .wMaxPacketSize     = 0x0064,
                        .bInterval          = 0x4, // ms
                        .bRefresh           = 0x0,
                        .bSynchAddress      = 0x0,
                    },
                },
                .acendpoints = {
                    {
                        .bLength            = UAC_ISO_ENDPOINT_DESC_SIZE,
                        .bDescriptorType    = USB_DT_CS_ENDPOINT,
                        .bDescriptorSubtype = UAC_EP_GENERAL,
                        .bmAttributes       = UAC_EP_CS_ATTR_SAMPLE_RATE,
                        .bLockDelayUnits    = 0x1, // ms
                        .wLockDelay         = 0x0, // ms
                    }
                },
            },
            .audiostream3 = {
                .interface = {
                    .bLength            = USB_DT_INTERFACE_SIZE,
                    .bDescriptorType    = USB_DT_INTERFACE,
                    .bInterfaceNumber   = 0x3,
                    .bAlternateSetting  = 0x2,
                    .bNumEndpoints      = 0x1,
                    .bInterfaceClass    = USB_CLASS_AUDIO,
                    .bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING,
                    .bInterfaceProtocol = 0x0,
                    .iInterface         = USB_STRING_INDEX(4),
                },
                .header = {
                    .bLength            = UAC_DT_AS_HEADER_SIZE,
                    .bDescriptorType    = USB_DT_CS_INTERFACE,
                    .bDescriptorSubtype = UAC_AS_GENERAL,
                    .bTerminalLink      = 0x3,
                    .bDelay             = 0x1, // frame
                    .wFormatTag         = UAC_FORMAT_TYPE_I_PCM,
                },
                .asinterface = {
                    .bLength = UAC_FORMAT_TYPE_I_DISCRETE_DESC_SIZE(8),
                    .bDescriptorType    = USB_DT_CS_INTERFACE,
                    .bDescriptorSubtype = UAC_FORMAT_TYPE,
                    .bFormatType        = UAC_FORMAT_TYPE_I,
                    .bNrChannels        = 0x2,   // Stereo
                    .bSubframeSize      = 0x1,   // bytes per subframe
                    .bBitResolution     = 0x8,   // 8 bits per sample
                    .bSamFreqType       = 0x8,   // frequencies supported
                    .tSamFreq           = { {0x00, 0xBB, 0x80}, // 48000 Hz
                                            {0x00, 0xAC, 0x44}, // 44100 Hz
                                            {0x00, 0x5D, 0xC0}, // 24000 Hz 
                                            {0x00, 0x56, 0x22}, // 22050 Hz 
                                            {0x00, 0x3E, 0x80}, // 16000 Hz 
                                            {0x00, 0x2E, 0xE0}, // 12000 Hz
                                            {0x00, 0x2B, 0x11}, // 11025 Hz
                                            {0x00, 0x1F, 0x40}}, // 8000 Hz
                },
                .endpoints = {
                    {
                        .bLength            = USB_DT_ENDPOINT_AUDIO_SIZE,
                        .bDescriptorType    = USB_DT_CS_ENDPOINT,
                        .bEndpointAddress   = USB_ENDPOINT_NUMBER(0x3) |
                                                USB_DIR_IN,
                        .bmAttributes       = (USB_ENDPOINT_XFER_ISOC |
                                                USB_ENDPOINT_SYNC_ASYNC),
                        .wMaxPacketSize     = 0x0032,
                        .bInterval          = 0x4, // ms
                        .bRefresh           = 0x0,
                        .bSynchAddress      = 0x0,
                    },
                },
                .acendpoints = {
                    {
                        .bLength            = UAC_ISO_ENDPOINT_DESC_SIZE,
                        .bDescriptorType    = USB_DT_CS_ENDPOINT,
                        .bDescriptorSubtype = UAC_EP_GENERAL,
                        .bmAttributes       = UAC_EP_CS_ATTR_SAMPLE_RATE,
                        .bLockDelayUnits    = 0x1, // ms
                        .wLockDelay         = 0x0, // ms
                    }
                },
            },
        },
    },
    .fullspeed = 0x0,
#include "descriptors_strings.inc"
};
