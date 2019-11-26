#include <fx2lib.h>
#include <fx2usb.h>
#include <usbcdc.h>
#include "usbuvc.h"

usb_ascii_string_c usb_strings[] = {
  [0] = "TimVideos.us",  // manufacturer
  [1] = "HDMI2USB.tv - Numato Opsis Board",  // product
  [2] = "0123456789abcdef",  // serial number
};

usb_desc_device_c usb_device = {
  .bLength              = sizeof(struct usb_desc_device),
  .bDescriptorType      = USB_DESC_DEVICE,
  .bcdUSB               = 0x0200,
  // It would make more sense for this to be USB_DEV_CLASS_PER_INTERFACE, such that the device
  // could be a composite device and include non-CDC interfaces. However, this does not work under
  // Windows; it enumerates a broken unknown device and a broken serial port instead. It is likely
  // that the following Microsoft document describes a way to make it work, but I have not verified
  // it: https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-common-class-generic-parent-driver
  .bDeviceClass         = USB_DEV_CLASS_PER_INTERFACE,
  .bDeviceSubClass      = USB_DEV_SUBCLASS_PER_INTERFACE,
  .bDeviceProtocol      = USB_DEV_PROTOCOL_PER_INTERFACE,
  .bMaxPacketSize0      = 64,
  .idVendor             = VID,  // VID, PID, DID must be defined as compiler flags
  .idProduct            = PID,
  .bcdDevice            = DID,
  .iManufacturer        = 1,
  .iProduct             = 2,
  .iSerialNumber        = 3,
  .bNumConfigurations   = 1,
};

/*** CDC **********************************************************************/

usb_desc_interface_c usb_iface_cic = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 0,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_IFACE_CLASS_CIC,
  .bInterfaceSubClass   = USB_IFACE_SUBCLASS_CDC_CIC_ACM,
  .bInterfaceProtocol   = USB_IFACE_PROTOCOL_CDC_CIC_NONE,
  .iInterface           = 0,
};

usb_desc_endpoint_c usb_endpoint_ep1_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 1|USB_DIR_IN,
  .bmAttributes         = USB_XFER_INTERRUPT,
  .wMaxPacketSize       = 8,
  .bInterval            = 10,
};

usb_cdc_desc_functional_header_c usb_func_cic_header = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_header),
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_HEADER,
  .bcdCDC               = 0x0120,
};

usb_cdc_desc_functional_acm_c usb_func_cic_acm = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_acm),
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_ACM,
  .bmCapabilities       = 0,
};

usb_cdc_desc_functional_union_c usb_func_cic_union = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_union) +
                          sizeof(uint8_t) * 1,
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_UNION,
  .bControlInterface    = 0,
  .bSubordinateInterface = { 1 },
};

usb_desc_interface_c usb_iface_dic = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 1,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 2,
  .bInterfaceClass      = USB_IFACE_CLASS_DIC,
  .bInterfaceSubClass   = USB_IFACE_SUBCLASS_CDC_DIC,
  .bInterfaceProtocol   = USB_IFACE_PROTOCOL_CDC_DIC_NONE,
  .iInterface           = 0,
};

usb_desc_endpoint_c usb_endpoint_ep2_out = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 2,
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};

usb_desc_endpoint_c usb_endpoint_ep4_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 4|USB_DIR_IN,
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};

/*** UVC **********************************************************************/

usb_desc_if_assoc_c usb_uvc_if_assoc = {
  .bLength              = sizeof(struct usb_desc_if_assoc),
  .bDescriptorType      = USB_DESC_IF_ASSOC,
  .bFirstInterface      = 0,
  .bInterfaceCount      = 2,
  .bFunctionClass       = USB_UVC_CC_VIDEO,
  .bFunctionSubClass    = USB_UVC_SC_VIDEO_INTERFACE_COLLECTION,
  .bFunctionProtocol    = 0,
  .iFunction            = 1,
};

/* Standard video control interface descriptor */
usb_desc_interface_c usb_uvc_std_ctrl_iface = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 2,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_UVC_CC_VIDEO,
  .bInterfaceSubClass   = USB_UVC_SUBCLASS_CC_VIDEOCONTROL,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

/* Input (camera) terminal descriptor */
usb_desc_uvc_camera_terminal_c usb_uvc_camera = {
  .bLength                  = sizeof(struct usb_desc_uvc_camera_terminal) + 3,
  .bDescriptorType          = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType       = USB_UVC_VC_INPUT_TERMINAL,
  .bTerminalID              = 1,
  .wTerminalType            = 0x0201,
  .bAssocTerminal           = 0,
  .iTerminal                = 0,
  .wObjectiveFocalLengthMin = 0x0000, // no optical zoom supported
  .wObjectiveFocalLengthMax = 0x0000, // no optical zoom supported
  .wOcularFocalLength       = 0x0000, // no optical zoom supported
  .bControlSize             = 3,
  .bmControls               = {0x00, 0x00, 0x00}, // no controls supported
};

/* Processing unit descriptor */
usb_desc_uvc_processing_unit_c usb_uvc_processing_unit = {
  .bLength            = sizeof(struct usb_desc_uvc_processing_unit) + 3 + 1 + 1,
  .bDescriptorType    = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType = USB_UVC_VC_PROCESSING_UNIT,
  .bUnitID            = 2,
  .bSourceID          = 1,
  .wMaxMultiplier     = 0,
  .bControlSize       = 3,
  ._tail              = {
    0x00, 0x00, 0x00, // bmControls[]
    0x00, // iProcessing
    0x00, // bmVideoStandards
  },
};

/* Extension unit descriptor */
usb_desc_uvc_extension_unit_c usb_uvc_extension_unit = {
  .bLength               = sizeof(struct usb_desc_uvc_extension_unit) + 1 + 1 + 3 + 1,
  .bDescriptorType       = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType    = USB_UVC_VC_EXTENSION_UNIT,
  .bUnitID               = 3,
  .guidExtensionCode     = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  },
  .bNumControls          = 0,
  .bNrInPins             = 1,
  ._tail                 = {
    2, // baSourceID[]
    3, // bControlSize
    0x00, 0x00, 0x00, // bmControls[]
    0, // iExtension
  },
};

/* Output terminal descriptor */
usb_desc_uvc_output_terminal_c usb_uvc_output_terminal = {
  .bLength            = sizeof(struct usb_desc_uvc_output_terminal) + 0,
  .bDescriptorType    = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType = USB_UVC_VC_OUTPUT_TERMINAL,
  .bTerminalID        = 4,
  .wTerminalType      = 0x0101,
  .bAssocTerminal     = 0,
  .bSourceID          = 3,
  .iTerminal          = 0,
  // ._tail              = {},
};

usb_desc_vc_if_header_c usb_uvc_vc_if_header = {
  .bLength              = sizeof(struct usb_desc_vc_if_header) + 1,
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType   = USB_UVC_VC_HEADER,
  .bcdUVC               = 0x0100,
  .wTotalLength         = // total size of all unit and terminal descriptors, flexible array members counted manually
      sizeof(usb_uvc_vc_if_header) + 1 +
      sizeof(usb_uvc_camera) + 1 +
      sizeof(usb_uvc_processing_unit) + 5 +
      sizeof(usb_uvc_extension_unit) + 6 +
      sizeof(usb_uvc_output_terminal) + 0, // TODO: check size, should be 0x50+1 (as in previous code + 1 byte added to processing unit)
  .dwClockFrequency     = 48000000,
  .bInCollection        = 1,
  .baInterfaceNr        = {1},
};

/* Standard video streaming interface descriptor (alternate setting 0) */
usb_desc_interface_c usb_uvc_std_streaming_iface_0 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 1,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_UVC_CC_VIDEO,
  .bInterfaceSubClass   = USB_UVC_SUBCLASS_CC_VIDEOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

/* Class-specific video streaming input header descriptor */
usb_desc_vs_if_in_header_c usb_uvc_vs_if_in_header = {
  .bLength             = sizeof(struct usb_desc_vs_if_in_header) + 2,
  .bDescriptorType     = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType  = USB_UVC_VS_INPUT_HEADER,
  .bNumFormats         = 2,
  .wTotalLength        =
      sizeof(usb_uvc_vs_if_in_header) + 0, // TODO
  .bEndpointAddress    = 6|USB_DIR_IN, // TODO: change CDC endpoint 6
  .bmInfo              = 0,
  .bTerminalLink       = 4,
  .bStillCaptureMethod = 1,
  .bTriggerSupport     = 1,
  .bTriggerUsage       = 0,
  .bControlSize        = 1,
  .bmaControls         = {0x00, 0x00},
};

/* Standard video streaming interface descriptor (alternate setting 1) */
usb_desc_interface_c usb_uvc_std_streaming_iface_1 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 1,
  .bAlternateSetting    = 1,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_UVC_CC_VIDEO,
  .bInterfaceSubClass   = USB_UVC_SUBCLASS_CC_VIDEOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

/*** UVC: MJPEG ***************************************************************/

/* Class specific VS format descriptor */
usb_desc_uvc_vs_format_mjpeg_c usb_uvc_mjpeg_vs_format = {
  .bLength              = sizeof(struct usb_desc_uvc_vs_format_mjpeg),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = USB_UVC_VS_FORMAT_MJPEG,
  .bFormatIndex         = 1,
  .bNumFrameDescriptors = 2,
  .bmFlags              = 1, // fixed sample size
  .bDefaultFrameIndex   = 1,
  .bAspectRatioX        = 0,
  .bAspectRatioY        = 0,
  .bmInterlaceFlags     = 0,
  .bCopyProtect         = 0, // duplication unrestricted
};

/* Class specific VS frame descriptor */
usb_desc_uvc_vs_frame_c usb_uvc_mjpeg_vs_frame_1 = {
  .bLength                   = sizeof(struct usb_desc_uvc_vs_frame) + sizeof(uint32_t) * 1,
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 1,
  .bmCapabilities            = 0x02,
  .wWidth                    = 1024,
  .wHeight                   = 768,
  .dwMinBitRate              = 0x0e000000,
  .dwMaxBitRate              = 0x0e000000,
  .dwMaxVideoFrameBufferSize = 2ul * 1024 * 768,
  .dwDefaultFrameInterval    = 666666,
  .bFrameIntervalType        = 1,
  .frameIntervals            = { { .dwFrameInterval = 666666, }, }
};

/* Class specific VS frame descriptor */
usb_desc_uvc_vs_frame_c usb_uvc_mjpeg_vs_frame_2 = {
  .bLength                   = sizeof(struct usb_desc_uvc_vs_frame) + sizeof(uint32_t) * 1,
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_MJPEG,
  .bFrameIndex               = 2,
  .bmCapabilities            = 0x02,
  .wWidth                    = 1280,
  .wHeight                   = 720,
  .dwMinBitRate              = 0x0e000000,
  .dwMaxBitRate              = 0x0e000000,
  .dwMaxVideoFrameBufferSize = 2ul * 1280 * 720,
  .dwDefaultFrameInterval    = 666666,
  .bFrameIntervalType        = 1,
  .frameIntervals            = { { .dwFrameInterval = 666666, }, }
};

/* VS Color Matching Descriptor Descriptor */
usb_desc_uvc_color_matching_c usb_uvc_mjpeg_color_matching = {
  .bLength                  = sizeof(struct usb_desc_uvc_color_matching),
  .bDescriptorType          = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype       = USB_UVC_VS_COLORFORMAT,
  .bColorPrimaries          = 1, // BT.709, sRGB
  .bTransferCharacteristics = 1, // BT.709
  .bMatrixCoefficients      = 4, // SMPTE 170M, BT.601
};

/*** UVC: YUY2 ****************************************************************/

/* Class specific VS format descriptor */
usb_desc_uvc_vs_format_uncompressed_c usb_uvc_yuy2_vs_format = {
  .bLength              = sizeof(struct usb_desc_uvc_vs_format_uncompressed),
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype   = USB_UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 2,
  .bNumFrameDescriptors = 2,
  .guidFormat           = {
        0x59, 0x55, 0x59, 0x32, 0x00, 0x00, 0x10, 0x00,
        0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71,
  },
  .bBitsPerPixel        = 0x10,
  .bDefaultFrameIndex   = 1,
  .bAspectRatioX        = 0, // non-interlaced in progressive scan
  .bAspectRatioY        = 0, // non-interlaced in progressive scan
  .bmInterlaceFlags     = 0, // non-interlaced
  .bCopyProtect         = 0, // no restrictions
};

/* Frame descriptors 1 */
usb_desc_uvc_vs_frame_c usb_uvc_yuy2_vs_frame_1 = {
  .bLength                   = sizeof(struct usb_desc_uvc_vs_frame) + sizeof(uint32_t) * 1,
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 1,
  .bmCapabilities            = 0x02,
  .wWidth                    = 1024,
  .wHeight                   = 768,
  .dwMinBitRate              = 0x0e000000,
  .dwMaxBitRate              = 0x0e000000,
  .dwMaxVideoFrameBufferSize = 2ul * 1024 * 768,
  .dwDefaultFrameInterval    = 1333332,
  .bFrameIntervalType        = 1,
  .frameIntervals            = { { .dwFrameInterval = 1333332, }, }
};

/* Frame descriptors 2 */
usb_desc_uvc_vs_frame_c usb_uvc_yuy2_vs_frame_2 = {
  .bLength                   = sizeof(struct usb_desc_uvc_vs_frame) + sizeof(uint32_t) * 1,
  .bDescriptorType           = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype        = USB_UVC_VS_FRAME_UNCOMPRESSED,
  .bFrameIndex               = 2,
  .bmCapabilities            = 0x02,
  .wWidth                    = 1280,
  .wHeight                   = 720,
  .dwMinBitRate              = 0x0e000000,
  .dwMaxBitRate              = 0x0e000000,
  .dwMaxVideoFrameBufferSize = 2ul * 1280 * 720,
  .dwDefaultFrameInterval    = 1333332,
  .bFrameIntervalType        = 1,
  .frameIntervals            = { { .dwFrameInterval = 1333332, }, }
};

/* VS Color Matching Descriptor Descriptor */
usb_desc_uvc_color_matching_c usb_uvc_yuy2_color_matching = {
  .bLength                  = sizeof(struct usb_desc_uvc_color_matching),
  .bDescriptorType          = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype       = USB_UVC_VS_COLORFORMAT,
  .bColorPrimaries          = 1, // BT.709, sRGB
  .bTransferCharacteristics = 1, // BT.709
  .bMatrixCoefficients      = 4, // SMPTE 170M, BT.601
};

/*** UVC: endpoints ***********************************************************/

usb_desc_endpoint_c usb_uvc_endpoint_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 6|USB_DIR_IN,
  .bmAttributes         = USB_XFER_ISOCHRONOUS,
  .wMaxPacketSize       = 1024,
  .bInterval            = 1,
};

/*** Configuration ************************************************************/

usb_configuration_c usb_config = {
  {
    .bLength              = sizeof(struct usb_desc_configuration),
    .bDescriptorType      = USB_DESC_CONFIGURATION,
    .bNumInterfaces       = 4,  // 2 for uvc (as we have 2 alt settings)
    .bConfigurationValue  = 1,
    .iConfiguration       = 0,
    .bmAttributes         = USB_ATTR_RESERVED_1,
    .bMaxPower            = 250, // 500mA
  },
  {
    // UVC
    { .generic   = (struct usb_desc_generic *) &usb_uvc_if_assoc              },
    { .interface =                             &usb_uvc_std_ctrl_iface        },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_camera                },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_processing_unit       },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_extension_unit        },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_output_terminal       },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_vc_if_header          },
    { .interface =                             &usb_uvc_std_streaming_iface_0 },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_format       },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_frame_1      },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_frame_2      },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_color_matching  },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_format        },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_frame_1       },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_frame_2       },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_color_matching   },
    { .interface =                             &usb_uvc_std_streaming_iface_1 },
    { .endpoint  =                             &usb_uvc_endpoint_in           },
    // CDC
    { .interface =                             &usb_iface_cic                 },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_header           },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_acm              },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_union            },
    { .endpoint  =                             &usb_endpoint_ep1_in           },
    { .interface =                             &usb_iface_dic                 },
    { .endpoint  =                             &usb_endpoint_ep2_out          },
    { .endpoint  =                             &usb_endpoint_ep4_in           },
    { 0 }
  }
};

usb_configuration_set_c usb_configs[] = {
  &usb_config,
};

usb_descriptor_set_c usb_descriptor_set = {
  .device           = &usb_device,
  .config_count     = ARRAYSIZE(usb_configs),
  .configs          = usb_configs,
  .string_count     = ARRAYSIZE(usb_strings),
  .strings          = usb_strings,
};

