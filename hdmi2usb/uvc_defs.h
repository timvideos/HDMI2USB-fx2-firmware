#ifndef UVC_DEFS_H
#define UVC_DEFS_H

/* USB Video Class */

#include <fx2lib.h>
#include <fx2usb.h>

// Macro to easily define typedefs for descriptor structures
#define USB_DESC_CONST_CODE_TYPEDEF(desc) \
    typedef __code const struct desc \
    desc ## _c;

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_if_assoc)

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_vc_if_header)

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_input_terminal)

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_output_terminal)

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_camera_terminal)

struct usb_desc_uvc_processing_unit {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bUnitID;
  uint8_t bSourceID;
  uint16_t wMaxMultiplier;
  uint8_t bControlSize;
  uint8_t _tail[]; // FIXME: better inteface?
  // put those in '_tail'
  /* uint8_t bmControls[]; */
  /* uint8_t iProcessing; */
  /* uint8_t bmVideoStandards; */
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_processing_unit)

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_extension_unit)

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_vs_if_in_header)

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_vs_format_mjpeg)

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

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_vs_format_uncompressed)

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
  /* 
   * frameIntervals must be either:
   *   dwMinFrameInterval, dwMaxFrameInterval, dwFrameIntervalStep,
   * or:
   *   N times dwFrameInterval                                      
   */
  union {
    // for continuous frame intervals
    uint32_t dwMinFrameInterval;
    uint32_t dwMaxFrameInterval;
    uint32_t dwFrameIntervalStep;
    // for discrete frame intervals
    uint32_t dwFrameInterval;
  } frameIntervals[];
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_vs_frame)

struct usb_desc_uvc_color_matching {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bColorPrimaries;
  uint8_t bTransferCharacteristics;
  uint8_t bMatrixCoefficients;
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_uvc_color_matching)

enum { // TODO: reorganise logically
  // required for interface association descriptor
  // see: https://www.usb.org/sites/default/files/iadclasscode_r10.pdf
  USB_DEV_CLASS_MISCELLANEOUS = 0xef,
  USB_DEV_SUBCLASS_COMMON = 0x02,
  USB_DEV_PROTOCOL_INTERFACE_ASSOCIATION_DESCRIPTOR = 0x01,
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

#undef USB_DESC_CONST_CODE_TYPEDEF

#endif /* UVC_DEFS_H */
