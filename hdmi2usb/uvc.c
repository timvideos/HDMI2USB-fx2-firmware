#include "uvc.h"

__xdata uint8_t valuesArray[26] =
    {
        0x01, 0x00,             /* bmHint : No fixed parameters */
        0x01,                   /* Use 1st Video format index */
        0x01,                   /* Use 1st Video frame index */
        0x2A, 0x2C, 0x0A, 0x00, /* Desired frame interval in 100ns */

        0x00, 0x00,             /* Key frame rate in key frame/video frame units */
        0x00, 0x00,             /* PFrame rate in PFrame / key frame units */
        0x00, 0x00,             /* Compression quality control */
        0x00, 0x00,             /* Window size for average bit rate */

        0x05, 0x00,             /* Internal video streaming i/f latency in ms */

        0x00, 0x20, 0x1C, 0x00, /* Max video frame size in bytes*/
        0x00, 0x04, 0x00, 0x00  /* No. of bytes device can rx in single payload (1024) */
};

__xdata uint8_t fps[2][4] = {{0x2A, 0x2C, 0x0A, 0x00}, {0x54, 0x58, 0x14, 0x00}};        // 15 ,7
__xdata uint8_t frameSize[2][4] = {{0x00, 0x00, 0x18, 0x00}, {0x00, 0x20, 0x1C, 0x00}};  // Dvi , HDMI

bool uvc_handle_usb_setup(__xdata struct usb_req_setup *req) {
  if (req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
      req->bRequest == USB_REQ_CLEAR_FEATURE)
  {
    EP0BCH = 0;
    EP0BCL = 26;
    SYNCDELAY;
    while (EP0CS & _BUSY)
      ;
    while (EP0BCL != 26)
      ;

    valuesArray[2] = EP0BUF[2];  // formate
    valuesArray[3] = EP0BUF[3];  // frame

    // fps
    valuesArray[4] = fps[EP0BUF[2] - 1][0];
    valuesArray[5] = fps[EP0BUF[2] - 1][1];
    valuesArray[6] = fps[EP0BUF[2] - 1][2];
    valuesArray[7] = fps[EP0BUF[2] - 1][3];

    valuesArray[18] = frameSize[EP0BUF[3] - 1][0];
    valuesArray[19] = frameSize[EP0BUF[3] - 1][1];
    valuesArray[20] = frameSize[EP0BUF[3] - 1][2];
    valuesArray[21] = frameSize[EP0BUF[3] - 1][3];

    EP0BCH = 0;  // ACK
    EP0BCL = 0;  // ACK
    return true;
  }

  if (req->bRequest == USB_UVC_GET_CUR || req->bRequest == USB_UVC_GET_MIN ||
      req->bRequest == USB_UVC_GET_MAX)
  {
    int i;
    SUDPTRCTL = 0x01;

    for (i = 0; i < 26; i++)
      EP0BUF[i] = valuesArray[i];

    EP0BCH = 0x00;
    SYNCDELAY;
    EP0BCL = 26;
    return true;

    // FIXME: What do these do????
    // case UVC_SET_CUR:
    // case UVC_GET_RES:
    // case UVC_GET_LEN:
    // case UVC_GET_INFO:

    // case UVC_GET_DEF:
    // FIXME: Missing this case causes the following errors
    // uvcvideo: UVC non compliance - GET_DEF(PROBE) not supported. Enabling workaround.
    // Unhandled Vendor Command: 87

  }

  return false;
}

void uvc_config(struct uvc_configuration *config) {
  uint8_t if_num_streaming = config->if_num_ctrl + 1;
  // interface association
  ((__xdata struct usb_desc_interface *) &usb_uvc_if_assoc)->bInterfaceNumber = config->if_num_ctrl;
  // interface numbers
  ((__xdata struct usb_desc_if_assoc *) &usb_uvc_std_ctrl_iface)->bFirstInterface          = config->if_num_ctrl;
  ((__xdata struct usb_desc_interface *) &usb_uvc_std_streaming_iface_0)->bInterfaceNumber = if_num_streaming;
  ((__xdata struct usb_desc_interface *) &usb_uvc_std_streaming_iface_1)->bInterfaceNumber = if_num_streaming;
  // endpoint numbers
  ((__xdata struct usb_desc_vs_if_in_header *) &usb_uvc_vs_if_in_header)->bEndpointAddress = config->ep_addr_streaming | USB_DIR_IN;
  ((__xdata struct usb_desc_endpoint *) &usb_uvc_ep_in)->bEndpointAddress                  = config->ep_addr_streaming | USB_DIR_IN;
}

/*** Descriptors **************************************************************/

/* Interface association descriptor */
usb_desc_if_assoc_c usb_uvc_if_assoc = {
  .bLength              = sizeof(struct usb_desc_if_assoc),
  .bDescriptorType      = USB_DESC_IF_ASSOC,
  .bFirstInterface      = 0, // uvc_config
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
  .bInterfaceNumber     = 0, // uvc_config
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
  .bInterfaceNumber     = 0, // uvc_config
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
  .bEndpointAddress    = 0, // uvc_config
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
  .bInterfaceNumber     = 0, // uvc_config
  .bAlternateSetting    = 1,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_UVC_CC_VIDEO,
  .bInterfaceSubClass   = USB_UVC_SUBCLASS_CC_VIDEOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

/*** UVC: endpoints ***********************************************************/

usb_desc_endpoint_c usb_uvc_ep_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 0, // uvc_config
  .bmAttributes         = USB_XFER_ISOCHRONOUS,
  .wMaxPacketSize       = 512,
  .bInterval            = 1,
};
