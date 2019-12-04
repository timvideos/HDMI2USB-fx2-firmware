#include <fx2lib.h>
#include <fx2usb.h>
#include <usbcdc.h>
#include "usb_config.h"
#include "uvc_defs.h"
#include "uac_defs.h"

usb_ascii_string_c usb_strings[] = {
  [USB_STR_MANUFACTURER   - 1] = "TimVideos.us",
  [USB_STR_PRODUCT        - 1] = "HDMI2USB.tv - Numato Opsis Board",
  [USB_STR_SERIAL_NUMBER  - 1] = "0123456789abcdef", // must have length 16 to set it to FPGA DNA
  [USB_STR_CHANNEL_NAME_1 - 1] = "Left",
  [USB_STR_CHANNEL_NAME_2 - 1] = "Right",
};

usb_desc_device_c usb_device = {
  .bLength              = sizeof(struct usb_desc_device),
  .bDescriptorType      = USB_DESC_DEVICE,
  .bcdUSB               = 0x0200,
  .bDeviceClass         = USB_DEV_CLASS_MISCELLANEOUS,
  .bDeviceSubClass      = USB_DEV_SUBCLASS_COMMON,
  .bDeviceProtocol      = USB_DEV_PROTOCOL_INTERFACE_ASSOCIATION_DESCRIPTOR,
  .bMaxPacketSize0      = 64,
  .idVendor             = VID,  // VID, PID, DID must be defined as compiler flags
  .idProduct            = PID,
  .bcdDevice            = DID,
  .iManufacturer        = USB_STR_MANUFACTURER,
  .iProduct             = USB_STR_PRODUCT,
  .iSerialNumber        = USB_STR_SERIAL_NUMBER,
  .bNumConfigurations   = 1,
};

/*** CDC **********************************************************************/

usb_desc_if_assoc_c usb_cdc_if_assoc = {
  .bLength              = sizeof(struct usb_desc_if_assoc),
  .bDescriptorType      = USB_DESC_IF_ASSOC,
  .bFirstInterface      = USB_CFG_IF_CDC_COMMUNICATION,
  .bInterfaceCount      = 2,
  .bFunctionClass       = USB_IFACE_CLASS_CIC,
  .bFunctionSubClass    = USB_IFACE_SUBCLASS_CDC_CIC_ACM,
  .bFunctionProtocol    = USB_IFACE_PROTOCOL_CDC_CIC_NONE,
  .iFunction            = 1,
};

usb_desc_interface_c usb_iface_cic = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = USB_CFG_IF_CDC_COMMUNICATION,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_IFACE_CLASS_CIC,
  .bInterfaceSubClass   = USB_IFACE_SUBCLASS_CDC_CIC_ACM,
  .bInterfaceProtocol   = USB_IFACE_PROTOCOL_CDC_CIC_NONE,
  .iInterface           = 1,
};

usb_desc_endpoint_c usb_cdc_ep_comm = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = USB_CFG_EP_CDC_COMMUNICATION|USB_DIR_IN,
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
  .bInterfaceNumber     = USB_CFG_IF_CDC_DATA,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 2,
  .bInterfaceClass      = USB_IFACE_CLASS_DIC,
  .bInterfaceSubClass   = USB_IFACE_SUBCLASS_CDC_DIC,
  .bInterfaceProtocol   = USB_IFACE_PROTOCOL_CDC_DIC_NONE,
  .iInterface           = 1,
};

usb_desc_endpoint_c usb_cdc_ep_data_out = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = USB_CFG_EP_CDC_HOST2DEV,
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};

usb_desc_endpoint_c usb_cdc_ep_data_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = USB_CFG_EP_CDC_DEV2HOST|USB_DIR_IN,
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};

/*** UVC **********************************************************************/

/* Interface association descriptor */
usb_desc_if_assoc_c usb_uvc_if_assoc = {
  .bLength              = sizeof(struct usb_desc_if_assoc),
  .bDescriptorType      = USB_DESC_IF_ASSOC,
  .bFirstInterface      = USB_CFG_IF_UVC_VIDEO_CONTROL,
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
  .bInterfaceNumber     = USB_CFG_IF_UVC_VIDEO_CONTROL,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_UVC_CC_VIDEO,
  .bInterfaceSubClass   = USB_UVC_SUBCLASS_CC_VIDEOCONTROL,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

/*** UVC: Streaming interface 0 ***********************************************/

/* Input (camera) terminal descriptor */
#define LENGTH_usb_uvc_camera (sizeof(struct usb_desc_uvc_camera_terminal) + 3)
usb_desc_uvc_camera_terminal_c usb_uvc_camera = {
  .bLength                  = LENGTH_usb_uvc_camera,
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
#define LENGTH_usb_uvc_processing_unit (sizeof(struct usb_desc_uvc_processing_unit) + 3 + 1 + 1)
usb_desc_uvc_processing_unit_c usb_uvc_processing_unit = {
  .bLength            = LENGTH_usb_uvc_processing_unit,
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
#define LENGTH_usb_uvc_extension_unit (sizeof(struct usb_desc_uvc_extension_unit) + 1 + 1 + 3 + 1)
usb_desc_uvc_extension_unit_c usb_uvc_extension_unit = {
  .bLength               = LENGTH_usb_uvc_extension_unit,
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
#define LENGTH_usb_uvc_output_terminal (sizeof(struct usb_desc_uvc_output_terminal) + 0)
usb_desc_uvc_output_terminal_c usb_uvc_output_terminal = {
  .bLength            = LENGTH_usb_uvc_output_terminal,
  .bDescriptorType    = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType = USB_UVC_VC_OUTPUT_TERMINAL,
  .bTerminalID        = 4,
  .wTerminalType      = 0x0101,
  .bAssocTerminal     = 0,
  .bSourceID          = 3,
  .iTerminal          = 0,
  // ._tail              = {},
};

/* Class specific VC interface header descriptor */
#define LENGTH_usb_uvc_vc_if_header (sizeof(struct usb_desc_vc_if_header) + 1)
usb_desc_vc_if_header_c usb_uvc_vc_if_header = {
  .bLength              = LENGTH_usb_uvc_vc_if_header,
  .bDescriptorType      = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType   = USB_UVC_VC_HEADER,
  .bcdUVC               = 0x0100,
  .wTotalLength         = 
      // total size of all unit and terminal descriptors, flexible array members counted manually
      // macros as intializers must be constant
      LENGTH_usb_uvc_vc_if_header +
      LENGTH_usb_uvc_camera +
      LENGTH_usb_uvc_processing_unit +
      LENGTH_usb_uvc_extension_unit +
      LENGTH_usb_uvc_output_terminal,
  .dwClockFrequency     = 48000000,
  .bInCollection        = 1,
  .baInterfaceNr        = {1},
};

/* Standard video streaming interface descriptor (alternate setting 0) */
usb_desc_interface_c usb_uvc_std_streaming_iface_0 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = USB_CFG_IF_UVC_VIDEO_STREAMING,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_UVC_CC_VIDEO,
  .bInterfaceSubClass   = USB_UVC_SUBCLASS_CC_VIDEOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

/*** UVC: Streaming interface 1 ***********************************************/

/*** UVC: MJPEG ***/

/* Class specific VS format descriptor */
#define LENGTH_usb_uvc_mjpeg_vs_format (sizeof(struct usb_desc_uvc_vs_format_mjpeg))
usb_desc_uvc_vs_format_mjpeg_c usb_uvc_mjpeg_vs_format = {
  .bLength              = LENGTH_usb_uvc_mjpeg_vs_format,
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
#define LENGTH_usb_uvc_mjpeg_vs_frame_1 (sizeof(struct usb_desc_uvc_vs_frame) + sizeof(uint32_t) * 1)
usb_desc_uvc_vs_frame_c usb_uvc_mjpeg_vs_frame_1 = {
  .bLength                   = LENGTH_usb_uvc_mjpeg_vs_frame_1,
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
#define LENGTH_usb_uvc_mjpeg_vs_frame_2 (sizeof(struct usb_desc_uvc_vs_frame) + sizeof(uint32_t) * 1)
usb_desc_uvc_vs_frame_c usb_uvc_mjpeg_vs_frame_2 = {
  .bLength                   = LENGTH_usb_uvc_mjpeg_vs_frame_2,
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
#define LENGTH_usb_uvc_mjpeg_color_matching (sizeof(struct usb_desc_uvc_color_matching))
usb_desc_uvc_color_matching_c usb_uvc_mjpeg_color_matching = {
  .bLength                  = LENGTH_usb_uvc_mjpeg_color_matching,
  .bDescriptorType          = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype       = USB_UVC_VS_COLORFORMAT,
  .bColorPrimaries          = 1, // BT.709, sRGB
  .bTransferCharacteristics = 1, // BT.709
  .bMatrixCoefficients      = 4, // SMPTE 170M, BT.601
};

/*** UVC: YUY2 ***/

/* Class specific VS format descriptor */
#define LENGTH_usb_uvc_yuy2_vs_format (sizeof(struct usb_desc_uvc_vs_format_uncompressed))
usb_desc_uvc_vs_format_uncompressed_c usb_uvc_yuy2_vs_format = {
  .bLength              = LENGTH_usb_uvc_yuy2_vs_format,
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
#define LENGTH_usb_uvc_yuy2_vs_frame_1 (sizeof(struct usb_desc_uvc_vs_frame) + sizeof(uint32_t) * 1)
usb_desc_uvc_vs_frame_c usb_uvc_yuy2_vs_frame_1 = {
  .bLength                   = LENGTH_usb_uvc_yuy2_vs_frame_1,
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
#define LENGTH_usb_uvc_yuy2_vs_frame_2 (sizeof(struct usb_desc_uvc_vs_frame) + sizeof(uint32_t) * 1)
usb_desc_uvc_vs_frame_c usb_uvc_yuy2_vs_frame_2 = {
  .bLength                   = LENGTH_usb_uvc_yuy2_vs_frame_2,
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
#define LENGTH_usb_uvc_yuy2_color_matching (sizeof(struct usb_desc_uvc_color_matching))
usb_desc_uvc_color_matching_c usb_uvc_yuy2_color_matching = {
  .bLength                  = LENGTH_usb_uvc_yuy2_color_matching,
  .bDescriptorType          = USB_UVC_CS_INTERFACE,
  .bDescriptorSubtype       = USB_UVC_VS_COLORFORMAT,
  .bColorPrimaries          = 1, // BT.709, sRGB
  .bTransferCharacteristics = 1, // BT.709
  .bMatrixCoefficients      = 4, // SMPTE 170M, BT.601
};

/* Class-specific video streaming input header descriptor */
#define LENGTH_usb_uvc_vs_if_in_header (sizeof(struct usb_desc_vs_if_in_header) + 2)
usb_desc_vs_if_in_header_c usb_uvc_vs_if_in_header = {
  .bLength             = LENGTH_usb_uvc_vs_if_in_header,
  .bDescriptorType     = USB_UVC_CS_INTERFACE,
  .bDescriptorSubType  = USB_UVC_VS_INPUT_HEADER,
  .bNumFormats         = 2,
  .wTotalLength        =
      LENGTH_usb_uvc_vs_if_in_header +
      LENGTH_usb_uvc_mjpeg_vs_format +
      LENGTH_usb_uvc_mjpeg_vs_frame_1 +
      LENGTH_usb_uvc_mjpeg_vs_frame_2 +
      LENGTH_usb_uvc_mjpeg_color_matching +
      LENGTH_usb_uvc_yuy2_vs_format +
      LENGTH_usb_uvc_yuy2_vs_frame_1 +
      LENGTH_usb_uvc_yuy2_vs_frame_2 +
      LENGTH_usb_uvc_yuy2_color_matching,
  .bEndpointAddress    = USB_CFG_EP_UVC|USB_DIR_IN,
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
  .bInterfaceNumber     = USB_CFG_IF_UVC_VIDEO_STREAMING,
  .bAlternateSetting    = 1,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_UVC_CC_VIDEO,
  .bInterfaceSubClass   = USB_UVC_SUBCLASS_CC_VIDEOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

/*** UVC: endpoints ***********************************************************/

usb_desc_endpoint_c usb_endpoint_uvc_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = USB_CFG_EP_UVC|USB_DIR_IN,
  .bmAttributes         = USB_XFER_ISOCHRONOUS,
  .wMaxPacketSize       = 512,
  .bInterval            = 1,
};

/*** UAC **********************************************************************/

usb_desc_interface_c usb_uac_std_ac_interface = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = USB_CFG_IF_UAC_AUDIO_CONTROL,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOCONTROL,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

#define LENGTH_uac_input_terminal (sizeof(struct usb_desc_uac_input_terminal))
usb_desc_uac_input_terminal_c uac_input_terminal = {
  .bLength            = LENGTH_uac_input_terminal,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_INPUT_TERMINAL,
  .bTerminalID        = 1,
  .wTerminalType      = UAC_INPUT_TERMINAL_MICROPHONE,
  .bAssocTerminal     = 0,
  .bNrChannels        = 2, // stereo
  .wChannelConfig     = (UAC_CHANNEL_LEFT | UAC_CHANNEL_RIGHT),
  .iChannelNames      = USB_STR_CHANNEL_NAME_1, // first channel name, other channels must have consequetive indices
  .iTerminal          = 0,
};

#define LENGTH_uac_output_terminal (sizeof(struct usb_desc_uac1_output_terminal))
usb_desc_uac1_output_terminal_c uac_output_terminal = {
  .bLength            = LENGTH_uac_output_terminal,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_OUTPUT_TERMINAL,
  .bTerminalID        = 2,
  .wTerminalType      = UAC_OUTPUT_TERMINAL_STREAMING,
  .bAssocTerminal     = 0,
  .bSourceID          = 1, // connected to input terminal
  .iTerminal          = 0,
};

#define LENGTH_uac_ac_header (sizeof(struct usb_desc_uac1_ac_header) + 1)
usb_desc_uac1_ac_header_c uac_ac_header = {
  .bLength            = LENGTH_uac_ac_header,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_MS_HEADER,
  .bcdADC             = UAC_BCD_V10,
  .wTotalLength       = // this descriptor + all unit and terminal descriptors
      LENGTH_uac_ac_header +
      LENGTH_uac_input_terminal +
      LENGTH_uac_output_terminal,
  .bInCollection      = 1, // one interface in collection 
  .baInterfaceNr      = {5}, // streaing interface 0
};

// Setting 0 of streaming interface, no endpoints which means this is a zero-bandwidth
// setting to allow host to temporarily disable audio in case of bandiwdth problems
usb_desc_interface_c uac_std_streaming_interface_alt0 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = USB_CFG_IF_UAC_AUDIO_STREAMING,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

// Setting 1 of streaming interface, regular operation
usb_desc_interface_c uac_std_streaming_interface_alt1 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = USB_CFG_IF_UAC_AUDIO_STREAMING,
  .bAlternateSetting    = 1,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

usb_desc_uac1_as_header_c uac_as_header = {
  .bLength            = sizeof(struct usb_desc_uac1_as_header),
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_AS_GENERAL,
  .bTerminalLink      = 2, // connected to output terminal
  .bDelay             = 1,
  .wFormatTag         = UAC_FORMAT_TYPE_I_PCM,
};

usb_desc_uac_format_type_i_discrete_c uac_format = {
  .bLength            = sizeof(struct usb_desc_uac_format_type_i_discrete) + 3,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_FORMAT_TYPE,
  .bFormatType        = UAC_FORMAT_TYPE_I,
  .bNrChannels        = 2,
  .bSubframeSize      = 2,
  .bBitResolution     = 16,
  .bSamFreqType       = 1,
  .tSamFreq           = {{0x40, 0x1F, 0x00}}, // 8000Hz, little endian
};

/*** UAC: endpoints ***********************************************************/

usb_desc_audio_endpoint_c uac_audio_endpoint = {
  .bLength          = sizeof(struct usb_desc_audio_endpoint),
  .bDescriptorType  = USB_DESC_ENDPOINT,
  .bEndpointAddress = USB_CFG_EP_UAC|USB_DIR_IN,
  .bmAttributes     = USB_XFER_ISOCHRONOUS,
  .wMaxPacketSize   = 512,
  .bInterval        = 4,
  .bRefresh         = 0,
  .bSynchAddress    = 0,
};

usb_desc_uac_iso_endpoint_c uac_iso_endpoint = {
  .bLength            = sizeof(struct usb_desc_uac_iso_endpoint),
  .bDescriptorType    = USB_DT_CS_ENDPOINT,
  .bDescriptorSubtype = UAC_AS_GENERAL,
  .bmAttributes       = 0,
  .bLockDelayUnits    = 0,
  .wLockDelay         = 0,
};

/*** Configuration ************************************************************/

usb_configuration_c usb_config = {
  {
    .bLength              = sizeof(struct usb_desc_configuration),
    .bDescriptorType      = USB_DESC_CONFIGURATION,
    .bNumInterfaces       = USBConfigInterface_COUNT,
    .bConfigurationValue  = 1,
    .iConfiguration       = 0,
    .bmAttributes         = USB_ATTR_RESERVED_1,
    .bMaxPower            = 250, // 500mA
  },
  {
    // UVC
    { .generic   = (struct usb_desc_generic *) &usb_uvc_if_assoc                 },
    { .interface =                             &usb_uvc_std_ctrl_iface           },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_vc_if_header             },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_camera                   },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_processing_unit          },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_extension_unit           },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_output_terminal          },
    { .interface =                             &usb_uvc_std_streaming_iface_0    },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_vs_if_in_header          },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_format          },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_frame_1         },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_frame_2         },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_color_matching     },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_format           },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_frame_1          },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_frame_2          },
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_color_matching      },
    { .interface =                             &usb_uvc_std_streaming_iface_1    },
    { .endpoint  =                             &usb_endpoint_uvc_in              },
    // CDC
    { .generic   = (struct usb_desc_generic *) &usb_cdc_if_assoc                 },
    { .interface =                             &usb_iface_cic                    },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_header              },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_acm                 },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_union               },
    { .endpoint  =                             &usb_cdc_ep_comm                  },
    { .interface =                             &usb_iface_dic                    },
    { .endpoint  =                             &usb_cdc_ep_data_out              },
    { .endpoint  =                             &usb_cdc_ep_data_in               },
    // UAC
    // TODO: { .generic   = (struct usb_desc_generic *) &usb_uvc_if_assoc              },
    { .interface =                             &usb_uac_std_ac_interface         },
    { .generic   = (struct usb_desc_generic *) &uac_ac_header                    },
    { .generic   = (struct usb_desc_generic *) &uac_input_terminal               },
    { .generic   = (struct usb_desc_generic *) &uac_output_terminal              },
    { .interface =                             &uac_std_streaming_interface_alt0 },
    { .interface =                             &uac_std_streaming_interface_alt1 },
    { .generic   = (struct usb_desc_generic *) &uac_as_header                    },
    { .generic   = (struct usb_desc_generic *) &uac_format                       },
    { .generic   = (struct usb_desc_generic *) &uac_audio_endpoint               },
    { .generic   = (struct usb_desc_generic *) &uac_iso_endpoint                 },
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

