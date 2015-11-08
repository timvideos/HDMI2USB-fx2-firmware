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



struct uvc_control_request {
	// D7 == Get 1 or Set 0
	// D6..5 == 01 -> Class specific request
	// D4..0 == 00001 -> Video Control/Video Streaming Interfaces or Video Function
	// D4..0 == 00010 -> Vide Data endpoint (of Video Streaming Interface)
	BYTE bmRequestType;
	BYTE bRequest;

	// The wValue field specifies the Control Selector (CS) in the high
	// byte, and zero in the low byte.

	// When processing all Controls as part of a batch request
	// (GET_###_ALL), wValue is not needed and must be set to 0. If the
	// request specifies an unknown or unsupported CS to that Unit or
	// Terminal, the control pipe must indicate a protocol STALL.

	// wValue == Control Selector in high byte
	WORD wValue;

	// Unit/Terminal ID and Interface
	// --or--
	// Endpoint
	//WORD wIndex;
	BYTE bEntityId;
	BYTE bEndpoint;

	// Length of parameter block
	WORD wLength;
};

// interface control request
// wValue == Control Selector in the high byte, low byte must be zero
// wIndex == ???

// Power Mode Control
// VC_VIDEO_POWER_MODE_CONTROL
// SET_CUR, GET_CUR, GET_INFO
struct uvc_video_power_mode_control {
	
};

/*
Sets the streaming interface Probe state. This is the attribute used for
stream parameter negotiation.
*/
//#define UVC_SET_CUR                                     (0x01)

// GET


enum uvc_get_values {
	UVC_CUR = 1,
	UVC_MIN,
	UVC_MAX,
	UVC_RES,	// Resolution attribute
	UVC_LEN,	// Data length
	UVC_INFO,	// Information attribute
	UVC_DEF,	// Default
};

#define UVC_GET_XXX_ONE					(0x80)
#define UVC_GET_XXX_ALL					(0x90)


/*
Returns the current state of the streaming interface. All supported fields set
to zero will be returned with an acceptable negotiated value.  Prior to the
initial SET_CUR operation, the GET_CUR state is undefined. This request shall
stall in case of negotiation failure.
*/
#define UVC_GET_CUR                                     (UVC_GET_XXX_ONE|UVC_CUR)
#define UVC_GET_CUR_ALL					(UVC_GET_XXX_ALL|UVC_CUR)

/* Returns the minimum value for negotiated fields. */
#define UVC_GET_MIN                                     (UVC_GET_XXX_ONE|UVC_MIN)
#define UVC_GET_MIN_ALL                                 (UVC_GET_XXX_ALL|UVC_MIN)

/* Returns the maximum value for negotiated fields. */
#define UVC_GET_MAX                                     (UVC_GET_XXX_ONE|UVC_MAX)
#define UVC_GET_MAX_ALL                                 (UVC_GET_XXX_ALL|UVC_MAX)

/* 
Return the resolution of each supported field in the Probe/Commit data
structure. 
*/
#define UVC_GET_RES                                     (UVC_GET_XXX_ONE|UVC_RES)
#define UVC_GET_RES_ALL                                 (UVC_GET_XXX_ALL|UVC_RES)

/*
Returns the length of the Commit or Probe data structure.
*/
#define UVC_GET_LEN                                     (UVC_GET_XXX_ONE|UVC_LEN)
#define UVC_GET_LEN_ALL                                 (UVC_GET_XXX_ALL|UVC_LEN)

/*
Queries the capabilities and status of the Control. The value returned for this
request shall have bits D0 and D1 each set to one (1), and the remaining bits
set to zero (0) (see section 4.1.2, “Get Request”).
*/
#define UVC_GET_INFO                                    (UVC_GET_XXX_ONE|UVC_INFO)
#define UVC_GET_INFO_ALL                                (UVC_GET_XXX_ALL|UVC_INFO)

// UVC_GET_INFO
// wLength == 1
#define UVC_GET_INFO_SUPPORTS_GET			(1 << 0)
#define UVC_GET_INFO_SUPPORTS_SET			(1 << 1)
#define UVC_GET_INFO_DISABLED_AUTO			(1 << 2)
#define UVC_GET_INFO_AUTOUPDATE				(1 << 3)
#define UVC_GET_INFO_ASYNC				(1 << 4)
#define UVC_GET_INFO_DISABLED_INCOMPAT			(1 << 5)
struct uvc_control_request_get_info_data {
	BYTE	supportsGet	: 1;
	BYTE	supportsSet	: 1;
	BYTE	disabledAuto	: 1; // Disabled due to automatic mode
	BYTE	autoupdate	: 1;
	BYTE	async		: 1;
	BYTE	disabledIncompat: 1; // Disabled due to incompatibility
};


/* 
Returns the default value for the negotiated fields.
*/
#define UVC_GET_DEF                                     (UVC_GET_XXX_ONE|UVC_DEF)
#define UVC_GET_DEF_ALL                                 (UVC_GET_XXX_ALL|UVC_DEF)

/* 2.4.2.2. Status Packet Type */
#define UVC_STATUS_TYPE_CONTROL				1
#define UVC_STATUS_TYPE_STREAMING			2

/* 4.3.1.1. Video Probe and Commit Controls */
struct uvc_streaming_control {
	WORD  bmHint;
	BYTE  bFormatIndex;
	BYTE  bFrameIndex;
	DWORD dwFrameInterval;
	WORD  wKeyFrameRate;
	WORD  wPFrameRate;
	WORD  wCompQuality;
	WORD  wCompWindowSize;

	WORD  wDelay;

	DWORD dwMaxVideoFrameSize;
	DWORD dwMaxPayloadTransferSize;
/*
	DWORD dwClockFrequency;
	BYTE  bmFramingInfo;
	BYTE  bPreferedVersion;
	BYTE  bMinVersion;
	BYTE  bMaxVersion;
 */
/*
	BYTE  bUsage;
	BYTE  bBitDepthLuma;
	BYTE  bmSettings;
	BYTE  bMaxNumberOfRefFramesPlus1;
	BYTE  bmRateControlModes;
	BYTE  bmLayoutPerStream;
*/
};


#define UVC_PROBE_BMHINT_wCompWindowSize	(1 << 4)
#define UVC_PROBE_BMHINT_wCompQuality		(1 << 3)
#define UVC_PROBE_BMHINT_wPFrameRate		(1 << 2)
#define UVC_PROBE_BMHINT_wKeyFrameRate		(1 << 1)
#define	UVC_PROBE_BMHINT_dwFrameInterval	(1 << 0)


BOOL handleUVCCommand(BYTE cmd);


struct uvc_payload_header {
	BYTE bHeaderLength;
	BYTE bmHeaderInfo;
};



#endif // UVC_H
