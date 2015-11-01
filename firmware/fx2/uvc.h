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

#define UVC_BUTTONS 1
struct uvc_status_packet_from_stream_interface {
	BYTE bEvent;			// 0x00 == Button Press
	BYTE bValue[UVC_BUTTONS];	// Buttons press status, 0 == released, 1 == pressed
};


// Video Control / Video Streaming Requests

struct uvc_control_request {
	BYTE bmRequestType;
	BYTE bRequest;
	WORD wValue;
	WORD wIndex;
	WORD wLength;
	// data
};

/*
Sets the streaming interface Probe state. This is the attribute used for
stream parameter negotiation.
*/
//#define UVC_SET_CUR                                     (0x01)

// GET
#define UVC_CONTROL_GET_SUPPORTS_GET			(1 << 0)
#define UVC_CONTROL_GET_SUPPORTS_SET			(1 << 1)
#define UVC_CONTROL_GET_DISABLED_AUTO			(1 << 2)
#define UVC_CONTROL_GET_AUTOUPDATE			(1 << 3)
#define UVC_CONTROL_GET_ASYNC				(1 << 4)
#define UVC_CONTROL_GET_DISABLED_INCOMPAT		(1 << 5)

struct uvc_control_request_get {
	BYTE	supportsGet	: 1;
	BYTE	supportsSet	: 1;
	BYTE	disabledAuto	: 1; // Disabled due to automatic mode
	BYTE	autoupdate	: 1;
	BYTE	async		: 1;
	BYTE	disabledIncompat: 1; // Disabled due to incompatibility
};


/*
Returns the current state of the streaming interface. All supported fields set
to zero will be returned with an acceptable negotiated value.  Prior to the
initial SET_CUR operation, the GET_CUR state is undefined. This request shall
stall in case of negotiation failure.
*/
#define UVC_GET_CUR                                     (0x81)

/* Returns the minimum value for negotiated fields. */
#define UVC_GET_MIN                                     (0x82)

/* Returns the maximum value for negotiated fields. */
#define UVC_GET_MAX                                     (0x83)

/* 
Return the resolution of each supported field in the Probe/Commit data
structure. 
*/
//#define UVC_GET_RES                                     (0x84)

/*
Returns the length of the Commit or Probe data structure.
*/
//#define UVC_GET_LEN                                     (0x85)

/*
Queries the capabilities and status of the Control. The value returned for this
request shall have bits D0 and D1 each set to one (1), and the remaining bits
set to zero (0) (see section 4.1.2, “Get Request”).
*/
//#define UVC_GET_INFO                                    (0x86)

/* 
Returns the default value for the negotiated fields.
*/
//#define UVC_GET_DEF                                     (0x87)

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
