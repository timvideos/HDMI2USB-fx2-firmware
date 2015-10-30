
#include "date.h"
#include <linux/ch9.h>
#include <linux/ch9-extra.h>
#include <linux/video.h>
#include <linux/video-extra.h>
#include <linux/uvcvideo.h>

#include <linux/cdc.h>
#include <linux/cdc-extra.h>

DECLARE_UVC_HEADER_DESCRIPTOR(1);
DECLARE_UVC_HEADER_DESCRIPTOR(2);

struct usb_section {
	struct usb_config_descriptor config;
	/* ;;;;;;;;;;;;;;;;;;;;;;;;;; UVC ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; */
	struct usb_uvc {
                /* Interface association descriptor */
		struct usb_interface_assoc_descriptor assoc_interface;
		struct usb_control {
                        /* Standard video control interface descriptor */
			struct usb_interface_descriptor interface;
                        /* Class specific VC interface header descriptor */
			struct UVC_HEADER_DESCRIPTOR(1) header;
			/* Input (camera) terminal descriptor */
			struct uvc_camera_terminal_descriptor camera;
			/* Processing unit descriptor */
			DECLARE_UVC_PROCESSING_UNIT_DESCRIPTOR(3) processing;
			/* Extension unit descriptor */
			DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(1, 3) extension;
			/* Output terminal descriptor */
			struct uvc_output_terminal_descriptor output;
		} videocontrol;
		struct usb_stream {
			/* Standard video streaming interface descriptor (alternate setting 0) */
			struct usb_interface_descriptor interface;
			/* Class-specific video streaming input header descriptor */
			DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(2, 1) header;
			/* ;;;;;;;;;;;;;; MJPEG ;;;;;;;;;;;;; */
			struct usb_stream_mjpeg {
				/* Class specific VS format descriptor */
				struct uvc_format_mjpeg format;
				/* Class specific VS frame descriptor 1 + 2 */
				DECLARE_UVC_FRAME_MJPEG(1) frames[2];
				/* VS Color Matching Descriptor Descriptor */
				struct uvc_color_matching_descriptor color;
			} mjpeg_stream;
			/* ;;;;;;;;;;;;;;;;;;;; YUY2 ;;;;;;;;;;;;;;;;;;;;;;;; */
			struct usb_stream_yuy2 {
				/* Class specific VS format descriptor */
				struct uvc_format_uncompressed format;
				/* Frame descriptors 1 + 2 */
				DECLARE_UVC_FRAME_UNCOMPRESSED(1) frames[2];
				/* VS Color Matching Descriptor Descriptor */
				struct uvc_color_matching_descriptor color;
			} yuy2_stream;
			/* Standard video streaming interface descriptor (alternate setting 1) */
			struct usb_interface_descriptor interface_alt;
		} videostream;
		/* Endpoint descriptor for streaming video data */
		struct usb_endpoint_descriptor endpoints[1];
	} uvc;
	/* ;;;;;;;;;;;;;;;;;;;;;;;;;; CDC ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; */
	struct usb_cdc {
		struct usb_interface_assoc_descriptor assoc_interface;

		struct usb_cdc_interface1 {
			struct usb_interface_descriptor interface;
			/* Header Functional Descriptor */
			struct usb_cdc_header_desc header;
			/* Union Functional Descriptor */
			struct usb_cdc_union_desc union_;
			/* CM Functional Descriptor */
			struct usb_cdc_call_mgmt_descriptor cm;
			/* ACM Functional Descriptor */
			struct usb_cdc_acm_descriptor acm;
			/* EP1 Descriptor */
			struct usb_endpoint_descriptor endpoints[1];
		} interface1;

		struct usb_cdc_interface2 {
			/* Virtual COM Port Data Interface Descriptor */
			struct usb_interface_descriptor interface;
			/* EP2OUT Descriptor + EP4 Descriptor */
			struct usb_endpoint_descriptor endpoints[2];
		} interface2;
	} cdc;
};

#define ARRAY_SIZE(x) \
	(sizeof(x)/sizeof((x)[0]))

struct usb_descriptor {
	struct usb_device_descriptor device;
	struct usb_section highspeed;
	struct usb_qualifier_descriptor qualifier;
//	struct usb_section fullspeed;
};

//        .db 0x2A,0x2C,0x0A,0x00          ;/* Default frame interval */
// 666666
#define MJPEG_FRAME_INTERVAL \
	0x0A2C2A

//        .db 0x00,0x20,0x1C,0x00          ;/* Maximum video or still frame size in bytes */ 
// 1843200
#define FRAME_SIZE_1280x720 \
	0x1C2000

//        .db 0x54,0x58,0x14,0x00          ;/* Default frame interval */ 
// 1333332
#define YUY2_FRAME_INTERVAL \
	0x145854

//        .db 0x00,0x00,0x18,0x00          ;/* Maximum video or still frame size in bytes */
// 1572864
#define FRAME_SIZE_1024x768 \
	0x180000

#define UNKNOWN_DESC_TYPE_24 \
	0x24

//        .db 0x00,0x00,0x00,0x0E          ;/* Min bit rate bits/s */ 
// 234881024
#define BIT_RATE \
	0xE000000

__xdata struct usb_descriptor descriptor = {
	.device = {
		.bLength		= USB_DT_DEVICE_SIZE,
		.bDescriptorType	= USB_DT_DEVICE,
		.bcdUSB			= USB_BCD_V20,
		.bDeviceClass 		= USB_CLASS_MISC,
		.bDeviceSubClass	= UVC_SC_VIDEOSTREAMING,
		.bDeviceProtocol	= 0x01, // ?? Protocol code?
		.bMaxPacketSize0	= 64,
		.idVendor		= VID,
		.idProduct		= PID,
		.bcdDevice		= DID,
		.iManufacturer		= USB_STRING_INDEX(0),
		.iProduct		= USB_STRING_INDEX(1),
		.iSerialNumber		= USB_STRING_INDEX(2),
		.bNumConfigurations	= 1
	},
	.highspeed = {
		.config = {
			.bLength		= USB_DT_CONFIG_SIZE,
			.bDescriptorType	= USB_DT_CONFIG,
			.wTotalLength		= sizeof(descriptor.highspeed),
			.bNumInterfaces		= 4,
			.bConfigurationValue	= 1,
			.iConfiguration		= USB_STRING_INDEX_NONE,
			.bmAttributes		= USB_CONFIG_ATT_ONE,
			.bMaxPower		= 250, // FIXME: ???
		},
		.uvc = {
			/* Interface association descriptor */
			.assoc_interface = {
				.bLength		= sizeof(struct usb_interface_assoc_descriptor),
				.bDescriptorType	= USB_DT_INTERFACE_ASSOCIATION,

				.bFirstInterface	= 0, // Number of first video control interface
				.bInterfaceCount	= 2, // Number of video streaming interfaces?
				.bFunctionClass		= USB_CLASS_VIDEO,
				.bFunctionSubClass	= UVC_SC_VIDEO_INTERFACE_COLLECTION,
				.bFunctionProtocol	= 0, // Not used
				.iFunction		= USB_STRING_INDEX(0),
			},
			.videocontrol = {
				/* Standard video control interface descriptor */
				.interface = {
					.bLength		= USB_DT_INTERFACE_SIZE,
					.bDescriptorType	= USB_DT_INTERFACE,
					.bInterfaceNumber	= 0,
					.bAlternateSetting	= 0,
					.bNumEndpoints		= 0,
					.bInterfaceClass	= USB_CLASS_VIDEO,
					.bInterfaceSubClass	= UVC_SC_VIDEOCONTROL,
					.bInterfaceProtocol	= 0,
					.iInterface		= USB_STRING_INDEX_NONE,
				},
				/* Class specific VC interface header descriptor */
				.header = {
					.bLength		= UVC_DT_HEADER_SIZE(1),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24, 
					.bDescriptorSubType	= UVC_VC_HEADER,
					.bcdUVC			= UVC_BCD_V10,
					/* Total size of class specific descriptors (till output terminal) */
					.wTotalLength		= sizeof(descriptor.highspeed.uvc.videocontrol) - sizeof(descriptor.highspeed.uvc.videocontrol.interface),
					.dwClockFrequency	= 48000000, // 48MHz?
					.bInCollection		= 1, // Number of streaming interfaces
					// Video streaming interface 1 belongs to this video
					// control interface
					.baInterfaceNr		= { 1 },
				},
				/* Input (camera) terminal descriptor */
				.camera = {
					.bLength		= UVC_DT_CAMERA_TERMINAL_SIZE(0),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24, 
					.bDescriptorSubType	= UVC_VC_INPUT_TERMINAL,
					.bTerminalID		= 1,
					.wTerminalType		= UVC_ITT_CAMERA,
					.bAssocTerminal		= 0, 	// No associated terminal
					.iTerminal		= USB_STRING_INDEX_NONE, // Not used
					.wObjectiveFocalLengthMin = 0,	// No optical support
					.wObjectiveFocalLengthMax = 0,	// No optical support
					.wOcularFocalLength 	= 0,	// No optical support
					.bControlSize		= ARRAY_SIZE(descriptor.highspeed.uvc.videocontrol.camera.bmControls),
					.bmControls		= { 0, 0, 0 },
				},
				/* Processing unit descriptor */
				.processing = {
					.bLength		= UVC_DT_PROCESSING_UNIT_SIZE(3),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24, 
					.bDescriptorSubType	= UVC_VC_PROCESSING_UNIT,
					.bUnitID		= 2, //UVC_PU_POWER_LINE_FREQUENCY_CONTROL,
					.bSourceID		= 1 /* descriptor.highspeed.uvc.videocontrol.camera.bTerminalID */,
					.wMaxMultiplier		= 0,
					.bControlSize		= ARRAY_SIZE(descriptor.highspeed.uvc.videocontrol.processing.bmControls),
					.bmControls		= { 0, 0, 0 },
					.iProcessing		= USB_STRING_INDEX_NONE,
				},
				/* Extension unit descriptor */
				.extension = {
					.bLength		= UVC_DT_EXTENSION_UNIT_SIZE(1, 3),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24, 
					.bDescriptorSubType	= UVC_VC_EXTENSION_UNIT,
					.bUnitID		= 3,
					.guidExtensionCode	= UVC_GUID_UNDEFINED,
					.bNumControls		= 0,
					.bNrInPins		= 1,
					.baSourceID		= { 2 /* descriptor.highspeed.uvc.videocontrol.processing.bUnitID */ },
					.bControlSize		= ARRAY_SIZE(descriptor.highspeed.uvc.videocontrol.extension.bmControls),
					.bmControls		= { 0, 0, 0 },
					.iExtension		= USB_STRING_INDEX_NONE,
				},
				/* Output terminal descriptor */
				.output = {
					.bLength		= UVC_DT_OUTPUT_TERMINAL_SIZE,
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
					.bDescriptorSubType	= UVC_VC_OUTPUT_TERMINAL,
					.bTerminalID		= 4,
					.wTerminalType		= UVC_TT_STREAMING,
					.bAssocTerminal		= 0, // No associated terminal
					.bSourceID		= 3 /* descriptor.highspeed.uvc.videocontrol.extension.bUnitID */,
					.iTerminal		= USB_STRING_INDEX_NONE,
				},
			},

			/* Class-specific video streaming input header descriptor */
			.videostream = {
				/* Standard video streaming interface descriptor (alternate setting 0) */
				.interface = {
					.bLength		= USB_DT_INTERFACE_SIZE,
					.bDescriptorType	= USB_DT_INTERFACE,
					.bInterfaceNumber	= 1,
					.bAlternateSetting	= 0,
					.bNumEndpoints		= 0,
					.bInterfaceClass	= USB_CLASS_VIDEO,
					.bInterfaceSubClass	= UVC_SC_VIDEOSTREAMING,
					.bInterfaceProtocol	= 0,
					.iInterface		= USB_STRING_INDEX_NONE,
				},
				/* Class-specific video streaming input header descriptor */
				.header = {
					.bLength		= UVC_DT_INPUT_HEADER_SIZE(1, 2),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
					.bDescriptorSubType	= UVC_VS_INPUT_HEADER,
					.bNumFormats		= 2,
					// FIXME: Restructure so wTotalLength is easier to calculate...
					.wTotalLength		= 
						sizeof(descriptor.highspeed.uvc.videostream)
						- sizeof(descriptor.highspeed.uvc.videostream.interface)
						- sizeof(descriptor.highspeed.uvc.videostream.interface_alt),
					.bEndpointAddress	= USB_ENDPOINT_NUMBER(6) | USB_DIR_IN, /* EP address for BULK video data */
					.bmInfo			= 0, // No dynamic format change supported?
					.bTerminalLink		= 4 /* descriptor.highspeed.uvc.videocontrol.output.bTerminalID */,
					.bStillCaptureMethod	= 1, //
					.bTriggerSupport	= 1, // Hardware trigger supported for still image?
					.bTriggerUsage		= 0, // Hardware to initiate still image capture
					.bControlSize		= ARRAY_SIZE(descriptor.highspeed.uvc.videostream.header.bmaControls),
					.bmaControls		= { { 0, 0 } },
				},
				.mjpeg_stream = {
					.format = {
						// ??? - UVC_GUID_FORMAT_MJPEG
						.bLength		= UVC_DT_FORMAT_MJPEG_SIZE, 
						.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
						.bDescriptorSubType	= UVC_VS_FORMAT_MJPEG,
						.bFormatIndex		= 1,
						.bNumFrameDescriptors	= ARRAY_SIZE(descriptor.highspeed.uvc.videostream.mjpeg_stream.frames),
						.bmFlags		= 1, // Uses fixed size samples
						.bDefaultFrameIndex	= 1,
						.bAspectRatioX		= 0, // Not used
						.bAspectRatioY		= 0, // Not used
						.bmInterfaceFlags	= 0, // Not used,
						.bCopyProtect		= 0, // Duplication unrestricted
					},
					.frames = {
						{
							// Class specific VS frame descriptor - 1
							.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1), 
							.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
							.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
							.bFrameIndex		= 1,
							.bmCapabilities		= 0x02, // Still image capture not supported?
							.wWidth			= 1024,
							.wHeight		=  768,
							.dwMinBitRate		= BIT_RATE,
							.dwMaxBitRate		= BIT_RATE,
							.dwMaxVideoFrameBufferSize = FRAME_SIZE_1024x768,
							.dwDefaultFrameInterval	= MJPEG_FRAME_INTERVAL,
							.bFrameIntervalType	= 1,
							.dwFrameInterval	= { MJPEG_FRAME_INTERVAL }, // Frame interval 1?
						}, {
							// Class specific VS frame descriptor - 2
							.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1), 
							.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
							.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
							.bFrameIndex		= 2,
							.bmCapabilities		= 0x02, // Still image capture not supported?
							.wWidth			= 1280,
							.wHeight		=  720,
							.dwMinBitRate		= BIT_RATE,
							.dwMaxBitRate		= BIT_RATE,
							.dwMaxVideoFrameBufferSize = FRAME_SIZE_1280x720,
							.dwDefaultFrameInterval	= MJPEG_FRAME_INTERVAL,
							.bFrameIntervalType	= 1,
							.dwFrameInterval	= { MJPEG_FRAME_INTERVAL },
						},
					},
					// VS Color Matching Descriptor Descriptor
					.color = {
						.bLength		= UVC_DT_COLOR_MATCHING_SIZE,
						.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
						.bDescriptorSubType	= UVC_VS_COLORFORMAT, // Color matching?
						.bColorPrimaries	= 1,	// BT.709, sRGB
						.bTransferCharacteristics = 1, 	// BT.709
						.bMatrixCoefficients	= 4, 	// SMPTE 170M
					},
				},
				.yuy2_stream = {
					.format = {
						.bLength		= UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
						.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
						.bDescriptorSubType	= UVC_VS_FORMAT_UNCOMPRESSED,
						.bFormatIndex		= 2,
						.bNumFrameDescriptors	= ARRAY_SIZE(descriptor.highspeed.uvc.videostream.yuy2_stream.frames),
						.guidFormat		= UVC_GUID_FORMAT_YUY2,
						.bBitsPerPixel		= 16,
						.bDefaultFrameIndex	= 1,
						.bAspectRatioX		= 0, // Not used
						.bAspectRatioY		= 0, // Not used
						.bmInterfaceFlags	= 0, // Not used,
						.bCopyProtect		= 0, // Duplication unrestricted
					},
					.frames = {
						{
							// Frame descriptors 1
							.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(1), 
							.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
							.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
							.bFrameIndex		= 1,
							.bmCapabilities		= 0x02, // Still image capture not supported?
							.wWidth			= 1024,
							.wHeight		=  768,
							.dwMinBitRate		= BIT_RATE,
							.dwMaxBitRate		= BIT_RATE,
							.dwMaxVideoFrameBufferSize = FRAME_SIZE_1024x768,
							.dwDefaultFrameInterval	= YUY2_FRAME_INTERVAL,
							.bFrameIntervalType	= 1,
							.dwFrameInterval	= { YUY2_FRAME_INTERVAL }, // Frame interval 3?
						}, {
							// Frame descriptors 2
							.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(1), 
							.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
							.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
							.bFrameIndex		= 2,
							.bmCapabilities		= 0x02, // Still image capture not supported?
							.wWidth			= 1280,
							.wHeight		=  720,
							.dwMinBitRate		= BIT_RATE,
							.dwMaxBitRate		= BIT_RATE,
							.dwMaxVideoFrameBufferSize = FRAME_SIZE_1280x720,
							.dwDefaultFrameInterval	= YUY2_FRAME_INTERVAL,
							.bFrameIntervalType	= 1,
							.dwFrameInterval	= { YUY2_FRAME_INTERVAL },
						},
					},
					.color = {
						.bLength		= UVC_DT_COLOR_MATCHING_SIZE,
						.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
						.bDescriptorSubType	= UVC_VS_COLORFORMAT,
						.bColorPrimaries	= 1,	// BT.709, sRGB
						.bTransferCharacteristics = 1, 	// BT.709
						.bMatrixCoefficients	= 4, 	// SMPTE 170M
					},
				},
				/* Standard video streaming interface descriptor (alternate setting 1) */
				.interface_alt = {
					.bLength		= USB_DT_INTERFACE_SIZE,
					.bDescriptorType	= USB_DT_INTERFACE,
					.bInterfaceNumber	= 1,
					.bAlternateSetting	= 1,
					.bNumEndpoints		= 1,
					.bInterfaceClass	= USB_CLASS_VIDEO,
					.bInterfaceSubClass	= UVC_SC_VIDEOSTREAMING,
					.bInterfaceProtocol	= 0,
					.iInterface		= USB_STRING_INDEX_NONE,
				},
			},
			.endpoints = {
				{
					.bLength		= USB_DT_ENDPOINT_SIZE,
					.bDescriptorType 	= USB_DT_ENDPOINT,
					.bEndpointAddress	= USB_ENDPOINT_NUMBER(0x6) | USB_DIR_IN,
					.bmAttributes		= USB_ENDPOINT_XFER_ISOC,
					.wMaxPacketSize		= 1024,
					.bInterval		= 1,
				},
			},
		},
		.cdc = {
			/* Interface association descriptor */
			.assoc_interface = {
				.bLength		= sizeof(struct usb_interface_assoc_descriptor),
				.bDescriptorType	= USB_DT_INTERFACE_ASSOCIATION,
				.bFirstInterface	= 2, // Number of first video control interface
				.bInterfaceCount	= 2, // Number of video streaming interfaces?
				.bFunctionClass		= USB_CLASS_COMM,
				.bFunctionSubClass	= USB_CLASS_PER_INTERFACE,
				.bFunctionProtocol	= USB_CDC_ACM_PROTO_AT_V25TER,
				.iFunction		= USB_STRING_INDEX(0),
			},
			.interface1 = {
				.interface = {
					.bLength		= USB_DT_INTERFACE_SIZE,
					.bDescriptorType	= USB_DT_INTERFACE,
					.bInterfaceNumber	= 2,
					.bAlternateSetting	= 0,
					.bNumEndpoints		= 1,
					.bInterfaceClass	= USB_CLASS_COMM,
					.bInterfaceSubClass	= USB_CDC_SUBCLASS_ACM,
					.bInterfaceProtocol	= USB_CDC_ACM_PROTO_AT_V25TER,
					.iInterface		= USB_STRING_INDEX(0),
				},
				.header = {
					.bLength		= sizeof(struct usb_cdc_header_desc),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
					.bDescriptorSubType	= USB_CDC_HEADER_TYPE,
					.bcdCDC			= CDC_BCD_V11,
				},
				.union_ = {
					.bLength		= sizeof(struct usb_cdc_union_desc),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
					.bDescriptorSubType	= USB_CDC_UNION_TYPE,
					.bMasterInterface0	= 2,
					.bSlaveInterface0	= 3,			
				},
				.cm = {
					.bLength		= sizeof(struct usb_cdc_call_mgmt_descriptor),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
					.bDescriptorSubType	= USB_CDC_CALL_MANAGEMENT_TYPE,
					.bmCapabilities		= 0,
					.bDataInterface		= 3,
				},
				.acm = {
					.bLength		= sizeof(struct usb_cdc_acm_descriptor),
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
					.bDescriptorSubType	= USB_CDC_ACM_TYPE,
					.bmCapabilities		= USB_CDC_CAP_LINE,
				},
				.endpoints = {
					{
						.bLength		= USB_DT_ENDPOINT_SIZE,
						.bDescriptorType 	= USB_DT_ENDPOINT,
						.bEndpointAddress	= USB_ENDPOINT_NUMBER(0x1) | USB_DIR_IN,
						.bmAttributes		= USB_ENDPOINT_XFER_INT,
						.wMaxPacketSize		= 16,
						.bInterval		= 64,
					}
				},
			},
			.interface2 = {
				.interface = {
					.bLength		= USB_DT_INTERFACE_SIZE,
					.bDescriptorType	= USB_DT_INTERFACE,
					.bInterfaceNumber	= 3,
					.bAlternateSetting	= 0,
					.bNumEndpoints		= 2,
					.bInterfaceClass	= USB_CLASS_CDC_DATA,
					.bInterfaceSubClass	= 0x0, // FIXME?
					.bInterfaceProtocol	= USB_CDC_PROTO_NONE,
					.iInterface		= USB_STRING_INDEX(0),
				},
				.endpoints = {
					{
						.bLength		= USB_DT_ENDPOINT_SIZE,
						.bDescriptorType 	= USB_DT_ENDPOINT,
						.bEndpointAddress	= USB_ENDPOINT_NUMBER(0x2) | USB_DIR_OUT,
						.bmAttributes		= USB_ENDPOINT_XFER_BULK,
						.wMaxPacketSize		= 512,
						.bInterval		= 0,
					}, {
						.bLength		= USB_DT_ENDPOINT_SIZE,
						.bDescriptorType 	= USB_DT_ENDPOINT,
						.bEndpointAddress	= USB_ENDPOINT_NUMBER(0x4) | USB_DIR_IN,
						.bmAttributes		= USB_ENDPOINT_XFER_BULK,
						.wMaxPacketSize		= 512,
						.bInterval		= 0,
					},
				},
			},
		},
	},
	.qualifier = {
		.bLength = USB_DT_DEVICE_QUALIFIER_SIZE, // FIXME: The old descriptor had this set as 0xF9!?
		.bDescriptorType = USB_DT_DEVICE_QUALIFIER,
		.bcdUSB = USB_BCD_V20,
		.bDeviceClass = USB_CLASS_MISC,
		.bDeviceSubClass = USB_CLASS_COMM,
		.bDeviceProtocol = USB_CDC_ACM_PROTO_AT_V25TER,
		.bMaxPacketSize0 = 64,
		.bNumConfigurations = 1,
		.bRESERVED = 0,
	},
};
