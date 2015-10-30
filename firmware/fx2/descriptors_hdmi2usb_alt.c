
#include <linux/ch9.h>
#include <linux/ch9-extra.h>
#include <linux/video.h>
#include <linux/video-extra.h>
#include <linux/uvcvideo.h>

struct usb_section {
	struct usb_config_descriptor		config;
	struct usb_interface_assoc_descriptor	assoc_interface;

	struct usb_control {
		struct usb_interface_descriptor			interface;
		DECLARE_UVC_HEADER_DESCRIPTOR(1)		header;
		struct uvc_camera_terminal_descriptor		camera;
		struct uvc_processing_unit_descriptor		processing;
		DECLARE_UVC_DT_EXTENSION_UNIT(1, 3)		extension;
		struct uvc_output_terminal_descriptor		output;
	} 						videocontrol;
	struct usb_stream {
		struct usb_interface_descriptor			interface;
		DECLARE_UVC_HEADER_DESCRIPTOR(1)		header;
		struct usb_stream_mjpeg {
			struct uvc_format_mjpeg				format;
			DECLARE_UVC_FRAME_MJPEG(1)			frames[2];
			struct uvc_color_matching_descriptor		color;
		} 						mjpeg_stream;
		struct usb_stream_yuy2 {
			struct uvc_format_uncompressed			format;
			DECLARE_UVC_FRAME_UNCOMPRESSED(1)		frames[2];
			struct uvc_color_matching_descriptor		color;
		} 						yuy2_stream;
	} 						videostream;


	struct usb_endpoint_descriptor endpoints[2];
};

#define ARRAY_SIZE(x) \
	(sizeof(x)/sizeof(x[0]))

struct usb_descriptor {
	struct usb_device_descriptor device;
	struct usb_qualifier_descriptor qualifier;
	struct usb_section highspeed;
//	struct usb_section fullspeed;
};

//        .db 0x2A,0x2C,0x0A,0x00          ;/* Default frame interval */
#define MJPEG_FRAME_INTERVAL \
	??

//        .db 0x00,0x20,0x1C,0x00          ;/* Maximum video or still frame size in bytes */ 
#define MJPEG_FRAME_SIZE \
	??

//        .db 0x54,0x58,0x14,0x00          ;/* Default frame interval */ 
#define YUY2_FRAME_INTERVAL \
	??

//        .db 0x00,0x00,0x18,0x00          ;/* Maximum video or still frame size in bytes */
#define YUY2_FRAME_SIZE \
	??

__xdata struct usb_descriptor descriptor = {
	.device = {
		.bLength		= USB_DT_DEVICE_SIZE,
		.bDescriptorType	= USB_DT_DEVICE,
		.bcdUSB			= USB_BCD_V20,
		.bDeviceClass 		= USB_CLASS_MISC,
		.bDeviceSubClass	= UVC_SC_VIDEOSTREAMING,
		.bDeviceProtocol	= 0x01, // ?? Protocol code?
		.bMaxPacketSize0	= 64,
		.idVendor		= VID_LE,
		.idProduct		= PID_LE,
		.bcdDevice		= DID_LE,
		.iManufacturer		= 1,
		.iProduct		= 2,
		.iSerialNumber		= 3,
		.bNumConfigurations	= 1
	},
	.highspeed = {
		.config = {
			.bLength		= USB_DT_CONFIG_SIZE,
			.bDescriptorType	= USB_DT_CONFIG,
			.wTotalLength		= sizeof(descriptor.highspeed),
			.bNumInterfaces		= 4,
			.bConfigurationValue	= 1,
			.iConfiguration		= 0,
			.bmAttributes		= USB_CONFIG_ATT_ONE,
			.bMaxPower		= 250, // FIXME: ???
		},
		/* Interface association descriptor */
		.assoc_interface = {
			.bLength		= sizeof(struct usb_interface_assoc_descriptor),
			.bDescriptorType	= USB_DT_INTERFACE_ASSOCIATION,

			.bFirstInterface	= 0, // Number of first video control interface
			.bInterfaceCount	= 2, // Number of video streaming interfaces?
			.bFunctionClass		= USB_CLASS_VIDEO,
			.bFunctionSubClass	= UVC_SC_VIDEO_INTERFACE_COLLECTION,
			.bFunctionProtocol	= 0, // Not used
			.iFunction		= 1,
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
				.iInterface		= 0,
			},
			/* Class specific VC interface header descriptor */
			.header = {
				.bLength		= UVC_DT_HEADER_SIZE(1),
				.bDescriptorType	= UNKNOWN_DESC_TYPE_24, 
				.bDescriptorSubType	= UVC_VC_HEADER,
				.bcdUVC			= UVC_BCD_V10,
				/* Total size of class specific descriptors (till output terminal) */
				.wTotalLength		= ,
				.dwClockFrequency	= , // 48MHz?
				.bInCollection		= 1, // Number of streaming interfaces
				// Video streaming interface 1 belongs to this video
				// control interface
				.baInterfaceNr		= { 1 },
			},
			/* Input (camera) terminal descriptor */
			.camera = {
				.bLength		= UVC_DT_INPUT_TERMINAL_SIZE,
				.bDescriptorType	= UNKNOWN_DESC_TYPE_24, 
				.bDescriptorSubType	= UVC_VC_INPUT_TERMINAL,
				.bTerminalID		= 1,
				.wTerminalType		= UVC_ITT_CAMERA,
				.bAssocTerminal		= 0, 	// No associated terminal
				.iTerminal		= 0,	// Not used
				.wObjectiveFocalLengthMin = 0,	// No optical support
				.wObjectiveFocalLengthMax = 0,	// No optical support
				.wOcularFocalLength 	= 0,	// No optical support
				.bControlSize		= sizeof(.bmControls),
				.bmControls		= { 0, 0, 0 },
			},
			/* Processing unit descriptor */
			.processing = {
				.bLength		= UVC_DT_PROCESSING_UNIT_SIZE(0),
				.bDescriptorType	= UNKNOWN_DESC_TYPE_24, 
				.bDescriptorSubType	= UVC_VC_PROCESSING_UNIT,
				.bUnitID		= 2, //UVC_PU_POWER_LINE_FREQUENCY_CONTROL,
				.bSourceID		= .camera.bTerminalID,
				.wMaxMultiplier		= 0,
				.bControlSize		= sizeof(.bmControls),
				.bmControls		= { 0, 0 },
				.iProcessing		= 0,
			},
			/* Extension unit descriptor */
			.extension = {
				.bLength		= UVC_DT_EXTENSION_UNIT_SIZE(1, 3),
				.bDescriptorType	= UNKNOWN_DESC_TYPE_24, 
				.bDescriptorSubType	= UVC_VC_EXTENSION_UNIT,
				.bUnitID		= 3,
				.guidExtensionCode[16]	= { [0 .. 16] = 0xff },
				.bNumControls		= 0,
				.bNrInPins		= 1,
				.baSourceID		= .processing.bUintID,
				.bControlSize		= 3,
				.bmControls		= { 0, 0, 0 },
				.iExtension		= 0,
			},
			/* Output terminal descriptor */
			.output = {
				.bLength		= UVC_DT_OUTPUT_TERMINAL_SIZE,
				.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
				.bDescriptorSubType	= UVC_VC_OUTPUT_TERMINAL,
				.bTerminalID		= 4,
				.wTerminalType		= UVC_TT_STREAMING,
				.bAssocTerminal		= 0, // No associated terminal
				.bSourceID		= .extension.bUintID,
				.iTerminal		= 0,
			},
		},

		/* Class-specific video streaming input header descriptor */
		.videostream = {
			/* Standard video streaming interface descriptor (alternate setting 0) */
			.interface_alt = {
				.bLength		= USB_DT_INTERFACE_SIZE,
				.bDescriptorType	= USB_DT_INTERFACE,
				.bInterfaceNumber	= 1,
				.bAlternateSetting	= 0,
				.bNumEndpoints		= 0,
				.bInterfaceClass	= USB_CLASS_VIDEO,
				.bInterfaceSubClass	= UVC_SC_VIDEOSTREAMING,
				.bInterfaceProtocol	= 0,
				.iInterface		= 0,
			},
			/* Class-specific video streaming input header descriptor */
			.header = {
				.bLength		= DECLARE_UVC_HEADER_DESCRIPTOR(1),
				.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
				.bDescriptorSubType	= UVC_VC_HEADER,
				.bNumFormats		= 2,
				.wTotalLength		= //
				.bEndpointAddress	= USB_ENDPOINT_NUMBER(6) | USB_DIR_OUT,
				.bmInfo			= 0, // No dynamic format change supported?
				.bTerminalLink		= .videocontrol.output.bTerminalID,
				.bStillCaptureMethod	= 1, //
				.bTriggerSupport	= 1, // Hardware trigger supported for stil image?
				.bTriggerUsage		= 0, // Hardware to initiate still image capture
				.bControlSize		= sizeof(.bmaControls),
				.bmaControls[]		= { 0, 0 },
			},
			.mjpeg_stream = {
				.format = {
					// ??? - UVC_GUID_FORMAT_MJPEG
					.bLength		= UVC_DT_FORMAT_MJPEG_SIZE, 
					.bDescriptorType	= UNKNOWN_DESC_TYPE_24,
					.bDescriptorSubType	= UVC_VS_FORMAT_MJPEG,
					.bFormatIndex		= 1,
					.bNumFrameDescriptors	= ARRAY_SIZE(.mjpeg_stream.frames),
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
						.bFrameIndex		= 1, // FIXME
						.bmCapabilities		= 0x02, // Still image capture not supported?
						.wWidth			= 1024,
						.wHeight		=  768,
						.dwMinBitRate		= BIT_RATE,
						.dwMaxBitRate		= BIT_RATE,
						.dwMaxVideoFrameBufferSize = MJPEG_FRAME_SIZE,
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
						.dwMaxVideoFrameBufferSize = MJPEG_FRAME_SIZE,
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
					.bFormatIndex		= 2
					.bNumFrameDescriptors	= ARRAY_SIZE(.yuy2_stream.frames),
					.guidFormat[16]		= UVC_GUID_FORMAT_YUY2,
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
						.bFrameIndex		= 1, // FIXME
						.bmCapabilities		= 0x02, // Still image capture not supported?
						.wWidth			= 1024,
						.wHeight		=  768,
						.dwMinBitRate		= BIT_RATE,
						.dwMaxBitRate		= BIT_RATE,
						.dwMaxVideoFrameBufferSize = YUY2_FRAME_SIZE,
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
						.dwMaxVideoFrameBufferSize = YUY2_FRAME_SIZE,
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
			}



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
/*	.fullspeed = {
		.config = {
			.bLength		= USB_DT_CONFIG_SIZE,
			.bDescriptorType	= USB_DT_CONFIG,
			.wTotalLength		= sizeof(descriptor.fullspeed),
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
*/
};
