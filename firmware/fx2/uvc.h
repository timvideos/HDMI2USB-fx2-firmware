#ifndef UVC_H
#define UVC_H

#include <fx2types.h>

//----------------------------------------------------------------------------
//	UVC definitions
//----------------------------------------------------------------------------

// UVC Status packets sent when a control updates

struct uvc_status_packet_header {
	BYTE	bStatusType;	// UVC_STATUS_TYPE_CONTROL || UVC_STATUS_TYPE_STREAMING
	BYTE	bOriginator;	// ID of terminal unit
};
struct uvc_status_packet_from_control_interface {
	BYTE	bEvent;		// 0x00 == Control Change, everything else reserved
	BYTE	bSelector;	// Control Selector of the control that issues the interrupt
	BYTE	bAttribute;	// Type of control change
	BYTE	bValue;		// Video Control Requests, GET_CUR/GET_INFO/etc...
};

#define UVC_STATUS_BATTRIBUTE_CHANGED_MAX		(1 << 4)
#define UVC_STATUS_BATTRIBUTE_CHANGED_MIN		(1 << 3)
#define UVC_STATUS_BATTRIBUTE_CHANGED_FAILURE		(1 << 2)
#define UVC_STATUS_BATTRIBUTE_CHANGED_INFO		(1 << 1)
#define	UVC_STATUS_BATTRIBUTE_CHANGED_VALUE		(1 << 0)

#define UVC_STATUS_BVALUE_GET_CUR		0x00
#define UVC_STATUS_BVALUE_GET_INFO		0x01
#define UVC_STATUS_BVALUE_GET_CUR_ERROR		0x02
#define UVC_STATUS_BVALUE_GET_MIN		0x03
#define UVC_STATUS_BVALUE_GET_MAX		0x03


#define UVC_BUTTONS 1
struct uvc_status_packet_from_stream_interface {
	BYTE bEvent;			// 0x00 == Button Press
	BYTE bValue[UVC_BUTTONS];	// Buttons press status, 0 == released, 1 == pressed
};


// Video Control / Video Streaming Requests


#define UVC_REQUEST_TYPE_MASK		0x0f
#define UVC_REQUEST_TYPE_CONTROL	(1 << 0)	// 4.2 VideoControl Requests
#define UVC_REQUEST_TYPE_STREAM		(1 << 1)	// 4.3 VideoStreaming Requests

struct uvc_control_request {
	// D7 == Get 1 or Set 0
	// D6..5 == 01 -> Class specific request
	// D4..0 == 00001 -> Video Control/Video Streaming Interfaces or Video Function
	// D4..0 == 00010 -> Video Data endpoint (of Video Streaming Interface)
	BYTE bmRequestType;
	BYTE bRequest;

	// The wValue field specifies the Control Selector (CS) in the high
	// byte, and zero in the low byte.

	// When processing all Controls as part of a batch request
	// (GET_###_ALL), wValue is not needed and must be set to 0. If the
	// request specifies an unknown or unsupported CS to that Unit or
	// Terminal, the control pipe must indicate a protocol STALL.

	// wValue == Control Selector in high byte
	union {
		BYTE bControl;
		BYTE bZero;
	} wValue;

	//WORD wIndex;
	struct {
		BYTE bUnitId;
		BYTE bInterface;
	} wIndex;

	// Length of parameter block
	WORD wLength;
};

// interface control request
// wValue == Control Selector in the high byte, low byte must be zero
// wIndex == ???

// Power Mode Control
// VC_VIDEO_POWER_MODE_CONTROL


// 4.2 VideoControl Requests
// =======================================================================
// 4.2.1 Interface Control Requests

// 4.2.1.1 Power Mode Control
// -----------------------------------------------------------------------
// UVC_VC_VIDEO_POWER_MODE_CONTROL
// Supports: SET_CUR, GET_CUR, GET_INFO
// wLength: 1 byte
#define UVC_VC_POWER_MODE_CONTROL_DATA_MODE_MASK	0x07 // 0b0000111
#define UVC_VC_POWER_MODE_CONTROL_DATA_MODE_FULL_POWER  0x00
#define UVC_VC_POWER_MODE_CONTROL_DATA_MODE_DEPENDENT	0x01

// Read only
#define UVC_VC_POWER_MODE_CONTROL_SUPPORTS_DEPENDENT	(1 << 4)
#define UVC_VC_POWER_MODE_CONTROL_POWERED_USB		(1 << 5)
#define UVC_VC_POWER_MODE_CONTROL_POWERED_BATTERY	(1 << 6)
#define UVC_VC_POWER_MODE_CONTROL_POWERED_AC		(1 << 7)

struct uvc_vc_data_power_mode_control {
	BYTE bmDevicePowerMode;
};

// 4.2.1.2 Request Error Code Control
// -----------------------------------------------------------------------
// UVC_VC_REQUEST_ERROR_CODE_CONTROL
// Supports: GET_CUR, GET_INFO
// wLength: 1 byte
enum {
	ERROR_CODE_NONE = 0,
	ERROR_CODE_NOT_READY,
	ERROR_CODE_WRONG_STATE,
	ERROR_CODE_POWER,
	ERROR_CODE_OUT_OF_RANGE,
	ERROR_CODE_INVALID_UNIT,
	ERROR_CODE_INVALID_CONTROL,
	ERROR_CODE_INVALID_REQUEST,
	ERROR_CODE_INVALID_VALUE,
	ERROR_CODE_UNKNOWN = 0xFF,
} bmRequestControlErrorCodeType;
struct uvc_vc_data_error_code_control {
	bmRequestControlErrorCodeType bmRequestErrorCode;
};

// 4.2.2 Unit and Terminal Control Requests
// 4.2.2.1 Camera Terminal Control Requests
// -----------------------------------------------------------------------
// We don't implement most of the camera controls, not being a camera.

// 4.2.2.1.1 Scanning Mode Control
// -----------------------------------------------------------------------
// UVC_CT_SCANNING_MODE_CONTROL
// Supports: SET_CUR, GET_CUR, GET_INFO
// wLength: 1 byte
#define UVC_VC_SCANNING_MODE_CONTROL_INTERLACED		0
#define UVC_VC_SCANNING_MODE_CONTROL_PROGRESSIVE	1
struct uvc_vc_data_scanning_mode_control {
	BYTE bScanningMode;
};

// 4.2.2.2 Selector Unit Control Requests
// -----------------------------------------------------------------------
// UVC_SU_INPUT_SELECT_CONTROL
// Supports: SET_CUR, GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO
// wLength: 1 byte

// 4.2.2.3 Processing Unit Control Requests
// -----------------------------------------------------------------------
// We don't implement most of the processing unit controls, not being a
// camera.

// 4.2.2.3.6 Power Line Frequency Control
// -----------------------------------------------------------------------
// UVC_PU_POWER_LINE_FREQUENCY_CONTROL
// Supports: SET_CUR, GET_CUR, GET_INFO, GET_DEF
// wLength: 1 byte
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL_DISABLED	0
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL_50HZ	1
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL_60HZ	2
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL_AUTO	3

// 4.2.2.3.19 Analog Video Lock Status Control
// -----------------------------------------------------------------------
// UVC_PU_ANALOG_LOCK_STATUS_CONTROL
// GET_CUR, GET_INFO
// wLength: 1 byte
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL_SIGNAL	0
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL_NOSIGNAL	1

// 4.2.2.4 Encoding Units
// -----------------------------------------------------------------------
// Doesn't seem to be supported under Linux? Complicated!?

// 4.2.2.5 Extension Unit Control Requests
// -----------------------------------------------------------------------
// Not sure what to do here...
// GET_LEN will have a wLength == 2!?

// 4.3 VideoStreaming Requests
// 4.3.1 Interface Control Requests
// =======================================================================

// 4.3.1.1. Video Probe and Commit Controls
// -----------------------------------------------------------------------
// UVC_VS_PROBE_CONTROL
// Supports: SET_CUR, GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_DEF, GET_LEN, GET_INFO
// UVC_VS_COMMIT_CONTROL
// Supports: SET_CUR, GET_CUR, GET_LEN, GET_INFO
// wLength: sizeof(uvc_vs_control_data) bytes


#define UVC_PROBE_BMHINT_wCompWindowSize	(1 << 4)
#define UVC_PROBE_BMHINT_wCompQuality		(1 << 3)
#define UVC_PROBE_BMHINT_wPFrameRate		(1 << 2)
#define UVC_PROBE_BMHINT_wKeyFrameRate		(1 << 1)
#define	UVC_PROBE_BMHINT_dwFrameInterval	(1 << 0)

struct uvc_vs_control_data_v10 {
	WORD  bmHint;

	BYTE  bFormatIndex;		// Index to descriptor values, 1 == first value
	BYTE  bFrameIndex;
	DWORD dwFrameInterval;		// 100 ns


	WORD  wKeyFrameRate;
	WORD  wPFrameRate;
	WORD  wCompQuality;		// 1 == lowest, 10000 == highest
	WORD  wCompWindowSize;

	WORD  wDelay;			// in ms 	(always device)

	DWORD dwMaxVideoFrameSize;	// in bytes	(always device)

	// Set MaxPayloadTransferSize fields to zero
	// Set MaxPayloadTransferSize field to current alternate interface max packet size

	DWORD dwMaxPayloadTransferSize; // in bytes	(???)
};
struct uvc_vs_control_data_v11 {
	WORD  bmHint;

	BYTE  bFormatIndex;		// Index to descriptor values, 1 == first value
	BYTE  bFrameIndex;
	DWORD dwFrameInterval;		// 100 ns

	WORD  wKeyFrameRate;
	WORD  wPFrameRate;
	WORD  wCompQuality;		// 1 == lowest, 10000 == highest
	WORD  wCompWindowSize;

	WORD  wDelay;			// in ms

	DWORD dwMaxVideoFrameSize;	// in bytes
	DWORD dwMaxPayloadTransferSize; // in bytes

	// New in v1.1
	DWORD dwClockFrequency;		// in Hz
	BYTE  bmFramingInfo;		// Ignored for MJPEG/Uncompressed/DV
	BYTE  bPreferedVersion;		// ??? -- bNumFormats
	BYTE  bMinVersion;		// ???
	BYTE  bMaxVersion;		// ???
};

#define UVC_PROBE_PAYLOAD_MJPEG_V10		0
#define UVC_PROBE_PAYLOAD_MJPEG_V11		1
#define UVC_PROBE_PAYLOAD_MJPEG_V15		5

struct uvc_vs_control_data_15 {
	WORD  bmHint;

	BYTE  bFormatIndex;		// Index to descriptor values, 1 == first value
	BYTE  bFrameIndex;

	DWORD dwFrameInterval;		// 100 ns
	WORD  wKeyFrameRate;
	WORD  wPFrameRate;
	WORD  wCompQuality;		// 1 == lowest, 10000 == highest
	WORD  wCompWindowSize;

	WORD  wDelay;			// in ms

	DWORD dwMaxVideoFrameSize;	// in bytes
	DWORD dwMaxPayloadTransferSize; // in bytes

	// New in v1.1
	DWORD dwClockFrequency;		// in Hz
	BYTE  bmFramingInfo;		// Ignored for MJPEG/Uncompressed/DV
	BYTE  bPreferedVersion;		// ??? -- bNumFormats
	BYTE  bMinVersion;		// ???
	BYTE  bMaxVersion;		// ???

	// New in v1.5
	BYTE  bUsage;
	BYTE  bBitDepthLuma;
	BYTE  bmSettings;
	BYTE  bMaxNumberOfRefFramesPlus1;
	BYTE  bmRateControlModes;
	BYTE  bmLayoutPerStream;
};

// size = stream->dev->uvc_version >= 0x0110 ? 34 : 26;
// 26 bytes for UVC1.0
// 34 bytes for UVC1.1 and above...
// 48 bytes for UVC1.5
BUILD_BUG_ON(sizeof(uvc_vs_control_data_v10) != 26); // 0x1a
BUILD_BUG_ON(sizeof(uvc_vs_control_data_v11) != 34); // 0x22
BUILD_BUG_ON(sizeof(uvc_vs_control_data_v15) != 48); // 0x30


// 4.3.1.7 Stream Error Code Control
// -----------------------------------------------------------------------
// UVC_VS_STREAM_ERROR_CODE_CONTROL
// Supports: GET_CUR, GET_INFO
// wLength: 1 byte
#define UVC_VS_STREAM_ERROR_CODE_CONTROL_
enum {
	ERROR_CODE_NONE = 0,
	ERROR_CODE_PROTECTED_CONTENT,
	ERROR_CODE_INPUT_UNDERRUN,
	ERROR_CODE_DISCONTINUITY,
	ERROR_CODE_OUTPUT_UNDERRUN,
	ERROR_CODE_OUTPUT_OVERRUN,
	ERROR_CODE_FORMAT_CHANGE,
	ERROR_CODE_STILL_CAPTURE_ERROR,
	ERROR_CODE_UNKNOWN,
} bmRequestStreamErrorCodeType;

// =======================================================================

BOOL handleUVCCommand(BYTE cmd);

/* 2.4.2.2. Status Packet Type */
#define UVC_STATUS_TYPE_CONTROL				1
#define UVC_STATUS_TYPE_STREAMING			2

struct uvc_payload_header {
	BYTE bHeaderLength;
	BYTE bmHeaderInfo;
};


#endif // UVC_H
