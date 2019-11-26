#include <fx2lib.h>
#include <fx2usb.h>
#include <usbcdc.h>

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

// CDC

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

usb_desc_endpoint_c usb_endpoint_ep6_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 6|USB_DIR_IN,
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};

// UVC

struct usb_desc_if_assoc {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bFirstInterface;
  uint8_t bInterfaceCount;
  uint8_t bFunctionClass;
  uint8_t bFunctionSubClass;
  uint8_t bFunctionProtocol;
  uint8_t iFunction;
};

typedef __code const struct usb_desc_if_assoc
  usb_desc_if_assoc_c;

struct usb_desc_vc_if_header {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint16_t bcdUVC;
  uint16_t wTotalLength;
  uint32_t dwClockFrequency;
  uint8_t bInCollection;
  uint8_t baInterfaceNr[];
};

typedef __code const struct usb_desc_vc_if_header
  usb_desc_vc_if_header_c;

struct usb_desc_uvc_input_terminal {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  uint8_t iTerminal;
  uint8_t _tail[]; // additional fields depends on Terminal type
};

typedef __code const struct usb_desc_uvc_input_terminal
  usb_desc_uvc_input_terminal_c;

struct usb_desc_uvc_output_terminal {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  uint8_t bSourceID;
  uint8_t iTerminal;
  uint8_t _tail[]; // additional fields depends on Terminal type
};

typedef __code const struct usb_desc_uvc_output_terminal
  usb_desc_uvc_output_terminal_c;

struct usb_desc_uvc_camera_terminal {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  uint8_t iTerminal;
  uint16_t wObjectiveFocalLengthMin;
  uint16_t wObjectiveFocalLengthMax;
  uint16_t wOcularFocalLength;
  uint8_t bControlSize;
  uint8_t bmControls[];
};

typedef __code const struct usb_desc_uvc_camera_terminal
  usb_desc_uvc_camera_terminal_c;

struct usb_desc_uvc_processing_unit {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bUnitID;
  uint8_t bSourceID;
  uint16_t wMaxMultiplier;
  uint8_t bControlSize;
  uint8_t _tail[];
  // put those in '_tail'
  /* uint8_t bmControls[]; */
  /* uint8_t iProcessing; */
  /* uint8_t bmVideoStandards; */
};

typedef __code const struct usb_desc_uvc_processing_unit
  usb_desc_uvc_processing_unit_c;

struct usb_desc_uvc_extension_unit {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bUnitID;
  uint8_t guidExtensionCode[16];
  uint8_t bNumControls;
  uint8_t bNrInPins;
  uint8_t _tail[];
  // put these in '_tail'
  /* uint8_t baSourceID[]; */
  /* uint8_t bControlSize; */
  /* uint8_t bmControls[]; */
  /* uint8_t iExtension; */
};

typedef __code const struct usb_desc_uvc_extension_unit
  usb_desc_uvc_extension_unit_c;

struct usb_desc_vs_if_in_header {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bNumFormats;
  uint16_t wTotalLength;
  uint8_t bEndpointAddress;
  uint8_t bmInfo;
  uint8_t bTerminalLink;
  uint8_t bStillCaptureMethod;
  uint8_t bTriggerSupport;
  uint8_t bTriggerUsage;
  uint8_t bControlSize;
  uint8_t bmaControls[];
};

typedef __code const struct usb_desc_vs_if_in_header
  usb_desc_vs_if_in_header_c;

struct usb_desc_uvc_vs_format_mjpeg {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFormatIndex;
  uint8_t bNumFrameDescriptors;
  uint8_t bmFlags;
  uint8_t bDefaultFrameIndex;
  uint8_t bAspectRatioX;
  uint8_t bAspectRatioY;
  uint8_t bmInterlaceFlags;
  uint8_t bCopyProtect;
};

typedef __code const struct usb_desc_uvc_vs_format_mjpeg
  usb_desc_uvc_vs_format_mjpeg_c;

struct usb_desc_uvc_vs_format_uncompressed {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFormatIndex;
  uint8_t bNumFrameDescriptors;
  uint8_t guidFormat[16];
  uint8_t bBitsPerPixel;
  uint8_t bDefaultFrameIndex;
  uint8_t bAspectRatioX;
  uint8_t bAspectRatioY;
  uint8_t bmInterlaceFlags;
  uint8_t bCopyProtect;
};

typedef __code const struct usb_desc_uvc_vs_format_uncompressed
  usb_desc_uvc_vs_format_uncompressed_c;

struct usb_desc_uvc_vs_frame {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFrameIndex;
  uint8_t bmCapabilities;
  uint16_t wWidth;
  uint16_t wHeight;
  uint32_t dwMinBitRate;
  uint32_t dwMaxBitRate;
  uint32_t dwMaxVideoFrameBufferSize;
  uint32_t dwDefaultFrameInterval;
  uint8_t bFrameIntervalType;
  union {
    struct {
      uint32_t dwMinFrameInterval;
      uint32_t dwMaxFrameInterval;
      uint32_t dwFrameIntervalStep;
    } continuousFrameIntervals;
    // or discreteFrameIntervals:
    uint32_t dwFrameInterval[];
  } frameIntervals;
};

typedef __code const struct usb_desc_uvc_vs_frame
  usb_desc_uvc_vs_frame_c;

struct usb_desc_uvc_color_matching {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bColorPrimaries;
  uint8_t bTransferCharacteristics;
  uint8_t bMatrixCoefficients;
};

typedef __code const struct usb_desc_uvc_color_matching
  usb_desc_uvc_color_matching_c;

enum {
  USB_DESC_IF_ASSOC = 0x0b,

  USB_UVC_CC_VIDEO = 0x0e,
  USB_UVC_SC_VIDEO_INTERFACE_COLLECTION = 0x03,
  USB_UVC_SUBCLASS_CC_VIDEOCONTROL = 0x01,
  USB_UVC_SUBCLASS_CC_VIDEOSTREAMING = 0x02,

  USB_UVC_CS_INTERFACE = 0x24,
  USB_UVC_VC_HEADER = 0x01,
  USB_UVC_VC_INPUT_TERMINAL = 0x02,
  USB_UVC_VC_OUTPUT_TERMINAL = 0x03,
  USB_UVC_VC_PROCESSING_UNIT = 0x05,
  USB_UVC_VC_EXTENSION_UNIT = 0x06,
  USB_UVC_VS_INPUT_HEADER = 0x01,
  USB_UVC_VS_FORMAT_MJPEG = 0x06,
  USB_UVC_VS_FRAME_MJPEG = 0x07,
  USB_UVC_VS_COLORFORMAT = 0x0d,
  USB_UVC_VS_FORMAT_UNCOMPRESSED = 0x04,
  USB_UVC_VS_FRAME_UNCOMPRESSED = 0x05,
};


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
  .bLength                  = sizeof(struct usb_desc_uvc_camera_terminal),
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
  .bLength            = sizeof(struct usb_desc_uvc_processing_unit),
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
  .bLength               = sizeof(struct usb_desc_uvc_extension_unit),
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
  .bLength            = sizeof(struct usb_desc_uvc_output_terminal),
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
  .bLength              = sizeof(struct usb_desc_vc_if_header),
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
usb_desc_interface_c usb_uvc_std_streaming_iface = {
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
  .bLength             = sizeof(struct usb_desc_vs_if_in_header),
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

// MJPEG

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
  .bLength                   = sizeof(struct usb_desc_uvc_vs_frame),
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
  .frameIntervals            = {
    .dwFrameInterval = {
      666666,
    }
  }
};


/* Class specific VS frame descriptor */
usb_desc_uvc_vs_frame_c usb_uvc_mjpeg_vs_frame_2 = {
  .bLength                   = sizeof(struct usb_desc_uvc_vs_frame),
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
  .frameIntervals            = {
    .dwFrameInterval = {
      666666,
    }
  }
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

// YUY2

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
  .bLength                   = sizeof(struct usb_desc_uvc_vs_frame),
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
  .frameIntervals            = {
    .dwFrameInterval = {
      1333332,
    }
  }
};


/* Frame descriptors 2 */
usb_desc_uvc_vs_frame_c usb_uvc_yuy2_vs_frame_2 = {
  .bLength                   = sizeof(struct usb_desc_uvc_vs_frame),
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
  .frameIntervals            = {
    .dwFrameInterval = {
      1333332,
    }
  }
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



usb_configuration_c usb_config = {
  {
    .bLength              = sizeof(struct usb_desc_configuration),
    .bDescriptorType      = USB_DESC_CONFIGURATION,
    .bNumInterfaces       = 2,
    .bConfigurationValue  = 1,
    .iConfiguration       = 0,
    .bmAttributes         = USB_ATTR_RESERVED_1,
    .bMaxPower            = 250,
  },
  {
    { .interface = &usb_iface_cic },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_header },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_acm },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_union },
    { .endpoint  = &usb_endpoint_ep1_in },
    { .interface = &usb_iface_dic },
    { .endpoint  = &usb_endpoint_ep2_out },
    { .endpoint  = &usb_endpoint_ep6_in },
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

