#ifndef UVC_H
#define UVC_H

#include <linux/video.h>
#include <linux/video-extra.h>

#include <fx2types.h>

BYTE handle_get_configuration();
BOOL handle_set_configuration(BYTE cfg);
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc);
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc);
BOOL handle_get_descriptor();

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

struct uvc_control_request {
	// D7 == Get 1 or Set 0
	// D6..5 == 01 -> Class specific request
	// D4..0 == 00001 -> Video Control/Video Streaming Interfaces or Video Function
	// D4..0 == 00010 -> Video Data endpoint (of Video Streaming Interface)
#define UVC_REQUEST_TYPE_MASK				0x60
#define UVC_REQUEST_TYPE_CLASS				0x20
#define UVC_REQUEST_RECIPIENT_MASK			0x1f
#define UVC_REQUEST_RECIPIENT_INTERFACE		(1 << 0)	// Directed to an interface
#define UVC_REQUEST_RECIPIENT_ENDPOINT		(1 << 1)	// Directed to an endpoint
	BYTE bmRequestType;

	// Request type - A.8. Video Class-Specific Request Codes section.
	BYTE bRequest;

	// The wValue field specifies the Control Selector (CS) in the high
	// byte, and zero in the low byte.

	// When processing all Controls as part of a batch request
	// (GET_###_ALL), wValue is not needed and must be set to 0. If the
	// request specifies an unknown or unsupported CS to that Unit or
	// Terminal, the control pipe must indicate a protocol STALL.

	// WORD wValue;
	struct {
		BYTE bZero;
		BYTE bControl;
	} wValue;

	//WORD wIndex;
	struct {
		BYTE bInterface;
		BYTE bUnitId;
	} wIndex;

	// Length of parameter block
	WORD wLength;
};

BOOL uvc_control_get_info(BYTE control, BYTE capabilities);
BOOL uvc_control_return_byte(BYTE data);

// 4.2 VideoControl Requests
// =======================================================================
inline BOOL uvc_control_request();

// 4.2.1 Interface Control Requests
// -----------------------------------------------------------------------
inline BOOL uvc_interface_control_request();

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
enum bmRequestControlErrorCodeType {
	CONTROL_ERROR_CODE_NONE = 0,
	CONTROL_ERROR_CODE_NOT_READY,
	CONTROL_ERROR_CODE_WRONG_STATE,
	CONTROL_ERROR_CODE_POWER,
	CONTROL_ERROR_CODE_OUT_OF_RANGE,
	CONTROL_ERROR_CODE_INVALID_UNIT,
	CONTROL_ERROR_CODE_INVALID_CONTROL,
	CONTROL_ERROR_CODE_INVALID_REQUEST,
	CONTROL_ERROR_CODE_INVALID_VALUE,
	CONTROL_ERROR_CODE_UNKNOWN = 0xFF,
};

inline BOOL uvc_control_set_error(enum bmRequestControlErrorCodeType);
inline void uvc_control_clear_error();

// 4.2.2 Unit and Terminal Control Requests
// 4.2.2.1 Camera Terminal Control Requests
// -----------------------------------------------------------------------
// We don't implement most of the camera controls, not being a camera.
inline BOOL uvc_camera_control_request();

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
inline BOOL uvc_selector_control_request();

// 4.2.2.3 Processing Unit Control Requests
// -----------------------------------------------------------------------
// We don't implement most of the processing unit controls, not being a
// camera.
inline BOOL uvc_processing_control_request();

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
inline BOOL uvc_encoding_control_request();

// 4.2.2.5 Extension Unit Control Requests
// -----------------------------------------------------------------------
// Not sure what to do here...
// GET_LEN will have a wLength == 2!?
inline BOOL uvc_extension_control_request();

// 4.2.2.?? Output Terminal Control Requests
// -----------------------------------------------------------------------
// Output Terminal's don't have any control.
inline BOOL uvc_output_control_request();

// 4.3 VideoStreaming Requests
// =======================================================================
inline BOOL uvc_stream_request();

// 4.3.1 Interface Control Requests
// 4.3.1.1. Video Probe and Commit Controls
// -----------------------------------------------------------------------
// UVC_VS_PROBE_CONTROL
// Supports: SET_CUR, GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_DEF, GET_LEN, GET_INFO
// UVC_VS_COMMIT_CONTROL
// Supports: SET_CUR, GET_CUR, GET_LEN, GET_INFO
// wLength: sizeof(uvc_vs_control_data) bytes
inline BOOL uvc_stream_probe_request();
inline BOOL uvc_stream_commit_request();

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

/*
#define UVC_PROBE_PAYLOAD_MJPEG_V10		0
#define UVC_PROBE_PAYLOAD_MJPEG_V11		1
#define UVC_PROBE_PAYLOAD_MJPEG_V15		5
struct uvc_vs_control_data_v15 {
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
*/
// size = stream->dev->uvc_version >= 0x0110 ? 34 : 26;
// 26 bytes for UVC1.0
// 34 bytes for UVC1.1 and above...
// 48 bytes for UVC1.5
//BUILD_BUG_ON(sizeof(struct uvc_vs_control_data_v10) != 26); // 0x1a
//BUILD_BUG_ON(sizeof(struct uvc_vs_control_data_v11) != 34); // 0x22
//BUILD_BUG_ON(sizeof(struct uvc_vs_control_data_v15) != 48); // 0x30

// 4.3.1.2 Video Still Probe Control and Still Commit Control
// VS_STILL_PROBE_CONTROL
// VS_STILL_COMMIT_CONTROL

// 4.3.1.3 Synch Delay Control
// 4.3.1.4 Still Image Trigger Control
// 4.3.1.5 Generate Key Frame Control
// 4.3.1.6 Update Frame Segment Control

// 4.3.1.7 Stream Error Code Control
// -----------------------------------------------------------------------
// UVC_VS_STREAM_ERROR_CODE_CONTROL
// Supports: GET_CUR, GET_INFO
// wLength: 1 byte
//#define UVC_VS_STREAM_ERROR_CODE_CONTROL_
inline BOOL uvc_stream_error_request();

enum bmRequestStreamErrorCodeType {
	STREAM_ERROR_CODE_NONE = 0,
	STREAM_ERROR_CODE_PROTECTED_CONTENT,
	STREAM_ERROR_CODE_INPUT_UNDERRUN,
	STREAM_ERROR_CODE_DISCONTINUITY,
	STREAM_ERROR_CODE_OUTPUT_UNDERRUN,
	STREAM_ERROR_CODE_OUTPUT_OVERRUN,
	STREAM_ERROR_CODE_FORMAT_CHANGE,
	STREAM_ERROR_CODE_STILL_CAPTURE_ERROR,
	STREAM_ERROR_CODE_UNKNOWN,
};

inline BOOL uvc_stream_set_error(enum bmRequestStreamErrorCodeType);
inline void uvc_stream_clear_error();

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
