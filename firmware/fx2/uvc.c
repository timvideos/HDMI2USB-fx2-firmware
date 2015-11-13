/*
 * Copyright 2013 Jahanzeb Ahmad
 * Copyright 2014 Tim Ansell <mithro@mithis.com>
 *
 * This file available under the MIT Licence
 *
 * http://opensource.org/licenses/MIT
 *
 */

#include "cdc-config.h"

#include "uvc.h"
#include "uvclocal.h"

#include <fx2regs.h>
#include <fx2macros.h>
#include <setupdat.h>
#include <eputils.h>
#include <delay.h>

#define SYNCDELAY SYNCDELAY4

//#define UVC_XXX_SIZE 26
#define UVC_XXX_SIZE (sizeof(struct uvc_streaming_control))

// FIXME: These are actually stored in the descriptor table!
const DWORD fps[] = {
	FRAME_INTERVAL_30FPS,
	FRAME_INTERVAL_7FPS,
};
const DWORD frameSize[] = {
	FRAME_SIZE_1024x768, // DVI
	FRAME_SIZE_1280x720, // HDMI
};

union uvc_streaming_control_array {
	struct uvc_vs_control_data control;
	BYTE array[sizeof(struct uvc_streaming_control)];
} valuesArray = {
	.control = {
		.bmHint			= UVC_PROBE_BMHINT_dwFrameInterval,
		.bFormatIndex		= 1,
		.bFrameIndex		= 1,
		.dwFrameInterval	= fps[0], /* in 100ns */
		.wKeyFrameRate		= 0,
		.wPFrameRate		= 0,
		.wCompQuality		= 0,
		.wCompWindowSize	= 0,
		.wDelay				= 5, /* in ms */
		.dwMaxVideoFrameSize	= frameSize[0], // in bytes
		.dwMaxPayloadTransferSize = 1024, // in bytes
	},
};

const struct uvc_vs_control_data_15 stream_config_default = { // For GET_DEF
	.bFormatIndex		= 1,
	.bFrameIndex		= 1,
	.dwFrameInterval	= UVC_DESCRIPTOR.mjpeg_stream.frames[.bFrameIndex].dwFrameInterval[.bFrameIndex],
	.wDelay				= 5,
	.dwMaxVideoFrameSize = UVC_DESCRIPTORS.mjpeg_stream.frames[.bFormatIndex].dwMaxVideoFrameBufferSize,
	.dwMaxPayloadTransferSize = 1024,
	.bPreferedVersion	= UVC_PROBE_PAYLOAD_MJPEG_V11,
	.bMinVersion		= UVC_PROBE_PAYLOAD_MJPEG_V11,
	.bMaxVersion		= UVC_PROBE_PAYLOAD_MJPEG_V11,
};
#define stream_config_max	stream_config_default
const struct uvc_vs_control_data_15 stream_config_min = {
};

const struct uvc_vs_control_data_15 stream_config_step = {
	.bFormatIndex		= 1,
	.bFrameIndex		= 1,
};

struct uvc_vs_control_data_15 stream_config_probed;
struct uvc_vs_control_data_15 stream_config_commit;

/**
 * Copy any structure field from src into dst if dst value is zero.
 */
BOOL uvc_vs_control_data_populate(const BYTE uvc_bcd_version, const struct uvc_vs_control_data_15* src, struct uvc_vs_control_data_15* dst) {
	if (uvc_bcd_version != UVC_BCD_V10 && 
		uvc_bcd_version != UVC_BCD_V11 &&
		uvc_bcd_version != UVC_BCD_V15)
		return FALSE;

	if (dst->bFormatIndex == 0) {
		dst->bFormatIndex = src->bFormatIndex;
	}
	if (dst->bFrameIndex == 0) {
		dst->bFrameIndex = src->bFrameIndex;
	}
	if (dst->dwFrameInterval == 0) {
		dst->dwFrameInterval = src->dwFrameInterval;
	}
	if (dst->wKeyFrameRate == 0) {
		dst->wKeyFrameRate = src->wKeyFrameRate;
	}
	if (dst->wPFrameRate == 0) {
		dst->wPFrameRate = src->wPFrameRate;
	}
	if (dst->wCompQuality == 0) {
		dst->wCompQuality = src->wCompQuality;
	}
	if (dst->wCompWindowSize == 0) {
		dst->wCompWindowSize = src->wCompWindowSize;
	}
	if (dst->wDelay == 0) {
		dst->wDelay = src->wDelay;
	}
	if (dst->dwMaxVideoFrameSize == 0) {
		dst->dwMaxVideoFrameSize = src->dwMaxVideoFrameSize;
	}
	if (dst->dwMaxPayloadTransferSize == 0) {
		dst->dwMaxPayloadTransferSize = src->dwMaxPayloadTransferSize;
	}

	// UVC1.0 ends here
	if (uvc_bcd_version != UVC_BCD_V11 && 
		uvc_bcd_version != UVC_BCD_V15)
		return FALSE;

	if (dst->dwClockFrequency == 0) {
		dst->dwClockFrequency = src->dwClockFrequency;
	}
	if (dst->bmFramingInfo == 0) {
		dst->bmFramingInfo = src->bmFramingInfo;
	}
	if (dst->bPreferedVersion == 0) {
		dst->bPreferedVersion = src->bPreferedVersion;
	}
	if (dst->bMinVersion == 0) {
		dst->bMinVersion = src->bMinVersion;
	}
	if (dst->bMaxVersion == 0) {
		dst->bMaxVersion = src->bMaxVersion;
	}

	// UVC1.1 ends here
	if (uvc_bcd_version != UVC_BCD_V15)
		return TRUE;

	if (dst->bUsage == 0) {
		dst->bUsage = src->bUsage;
	}
	if (dst->bBitDepthLuma == 0) {
		dst->bBitDepthLuma = src->bBitDepthLuma;
	}
	if (dst->bmSettings == 0) {
		dst->bmSettings = src->bmSettings;
	}
	if (dst->bMaxNumberOfRefFramesPlus1 == 0) {
		dst->bMaxNumberOfRefFramesPlus1 = src->bMaxNumberOfRefFramesPlus1;
	}
	if (dst->bmRateControlModes == 0) {
		dst->bmRateControlModes = src->bmRateControlModes;
	}
	if (dst->bmLayoutPerStream == 0) {
		dst->bmLayoutPerStream = src->bmLayoutPerStream;
	}

	return TRUE;
}

// FIXME: Figure out how to make this __at(0xE6B8) be __at(&SETUPDAT)
__xdata __at(0xE6B8) volatile struct uvc_control_request uvc_ctrl_request;
// FIXME: Figure out how to make this __at(0xXXX) be __at(&EP0BUF)
//__xdata __at(0xE740) volatile struct uvc_??? uvc_clear_feature;

// capabilities = UVC_CONTROL_CAP_XXX | UVC_CONTROL_CAP_XXX
BOOL handle_uvc_control_get_info(BYTE control, BYTE capabilities);

#define UVC_DESCRIPTOR descriptors.highspeed.uvc

BOOL handleUVCCommand(BYTE cmd)
{
	// assert cmd == uvc_ctrl_request.bRequest

	int i;

	switch(cmd) {
	case CLEAR_FEATURE:
		// FIXME: WTF is 0x21 !?
		if (uvc_ctrl_request.bmRequestType != 0x21)
			return FALSE;

		// Write the CLEAR_FEATURE size into EP0?
		EP0BCH = 0;
		EP0BCL = UVC_XXX_SIZE;
		SYNCDELAY;
		// Wait?
		while (EP0CS & bmEPBUSY);
		while (EP0BCL != UVC_XXX_SIZE);

		// fps
		valuesArray.control.bFormatIndex = EP0BUF[2];
		valuesArray.control.dwFrameInterval = fps[valuesArray.control.bFrameIndex-1];

		// frame size
		valuesArray.control.bFrameIndex = EP0BUF[3];
		valuesArray.control.dwMaxVideoFrameSize = frameSize[valuesArray.control.bFormatIndex-1];

		EP0BCH = 0; // ACK
		EP0BCL = 0; // ACK
		return TRUE;


	// if ((uvc_ctrl_request.bmRequestType & UVC_REQUEST_TYPE_DIR) == UVC_REQUEST_TYPE_SET)
	//	assert uvc_ctrl_request.bRequest == UVC_SET_CUR || uvc_ctrl_request.bRequest == UVC_SET_CUR_ALL

	switch (uvc_ctrl_request.bmRequestType & UVC_REQUEST_TYPE_MASK) {
	// 4.2 VideoControl Requests
	case UVC_REQUEST_TYPE_CONTROL: {

		// Should be sending to the control interface
		if (uvc_ctrl_request.wIndex.bInterface != UVC_DESCRIPTOR.videocontrol.interface.bInterfaceNumber)
			return handle_uvc_control_set_error(ERROR_CODE_INVALID_UNIT);

		// Check sending to a valid UVC interface
		//if (uvc_ctrl_request.wIndex.bInterface > UVC_DESCRIPTOR.assoc_interface.bFirstInterface &&
		//	uvc_ctrl_request.wIndex.bInterface < UVC_DESCRIPTOR.assoc_interface.bFirstInterface + UVC_DESCRIPTOR.assoc_interface.bInterfaceCount)

		switch (uvc_ctrl_request.wValue.bControl) {
		// 4.2.1 Interface Control Requests
		// ==========================================
		// 4.2.1.1 Power Mode Control	
		case UVC_VC_VIDEO_POWER_MODE_CONTROL:
			// assert uvc_ctrl_request.wLength == 1
			if (uvc_ctrl_request.wIndex.bUnitId != 0)
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_UNIT);

			switch (uvc_ctrl_request.bRequest) {
			case SET_CUR:
				// FIXME: Implement
				// struct uvc_vc_data_power_mode_control;
				return TRUE;

			case GET_CUR:
				// FIXME: Implement
				// RETURN UVC_VC_POWER_MODE_CONTROL_POWERED_AC | UVC_VC_POWER_MODE_CONTROL_DATA_MODE_FULL_POWER
				// struct uvc_vc_data_power_mode_control;
				return TRUE;

			case GET_INFO:
				return handle_uvc_control_get_info(
					UVC_VC_VIDEO_POWER_MODE_CONTROL,
					UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

			// Unsupported request type
			default:
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_REQUEST);
			}

		// 4.2.1.2 Request Error Code Control
		case UVC_VC_REQUEST_ERROR_CODE_CONTROL:
			// assert uvc_ctrl_request.wLength == 1
			if (uvc_ctrl_request.wIndex.bUnitId != 0)
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_UNIT);

			switch (uvc_ctrl_request.bRequest) {
			case GET_CUR:
				// FIXME: Implement
				// struct uvc_vc_data_error_code_control
				return TRUE;

			case GET_INFO:
				return handle_uvc_control_get_info(
					UVC_VC_REQUEST_ERROR_CODE_CONTROL,
					UVC_CONTROL_CAP_GET);

			// Unsupported request type
			default:
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_REQUEST);
			}

		// 4.2.2 Unit and Terminal Control Requests
		// ==========================================
		// 4.2.2.1 Camera Terminal Control Requests
		// We don't implement most of the camera controls, not being a camera.
		//
		// Exposure related controls
		// -----------------------------------------------
		// 4.2.2.1.2 Auto-Exposure Mode Control
		// UVC_CT_AE_MODE_CONTROL
		// 4.2.2.1.3 Auto-Exposure Priority Control
		// UVC_CT_AE_PRIORITY_CONTROL
		// 4.2.2.1.4 Exposure Time (Absolute) Control
		// UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL
		// 4.2.2.1.5 Exposure Time (Relative) Control
		// UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL
		//
		// Focus related controls
		// -----------------------------------------------
		// 4.2.2.1.6 Focus (Absolute) Control
		// 4.2.2.1.7 Focus (Relative) Control
		// 4.2.2.1.8 Focus, Simple Range
		// 4.2.2.1.9 Focus, Auto Control
		//
		// Iris related controls
		// -----------------------------------------------
		// 4.2.2.1.10 Iris (Absolute) Control
		// 4.2.2.1.11 Iris (Relative) Control
		//
		// Pan, Tilt and Zoom Controls
		// -----------------------------------------------
		// 4.2.2.1.12 Zoom (Absolute) Control
		// 4.2.2.1.13 Zoom (Relative) Control
		// 4.2.2.1.14 PanTilt (Absolute) Control
		// 4.2.2.1.15 PanTilt (Relative) Control
		// 4.2.2.1.16 Roll (Absolute) Control
		// 4.2.2.1.17 Roll (Relative) Control

		// 4.2.2.1.18 Privacy Control
		// 4.2.2.1.19 Digital Window Control
		// FIXME: Could Digital Window control be useful?
		// 4.2.2.1.20 Digital Region of Interest (ROI) Control

		// 4.2.2.1.1 Scanning Mode Control
		// Interlaced / Progressive
		case UVC_CT_SCANNING_MODE_CONTROL:
			// assert uvc_ctrl_request.wLength == 1
			if (uvc_ctrl_request.wIndex.bUnitId != CAMERA_UNIT_ID)
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_UNIT);

			switch (uvc_ctrl_request.bRequest) {
			case SET_CUR:
				// FIXME: Implement
				// new_input = get_byte
				// if new_input != UVC_VC_SCANNING_MODE_CONTROL_PROGRESSIVE
				//  return handle_uvc_control_set_error(ERROR_CODE_INVALID_VALUE);
				return TRUE;

			case GET_DEF:
			case GET_CUR:
				return handle_uvc_control_return_byte(
					UVC_VC_SCANNING_MODE_CONTROL_PROGRESSIVE);

			case GET_INFO:
				return handle_uvc_control_return_info(
					UVC_CT_SCANNING_MODE_CONTROL,
					UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

			// Unsupported request type
			default:
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_REQUEST);
			}
			
		// 4.2.2.2 Selector Unit Control Requests
		case UVC_SU_INPUT_SELECT_CONTROL:
			// assert uvc_ctrl_request.wLength == 1

			// FIXME: Actually add a selector unit
			if (uvc_ctrl_request.wIndex.bUnitId != UNIT_SELECTOR_ENCODER)
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_UNIT);

			switch (uvc_ctrl_request.bRequest) {
			case SET_CUR:
				// FIXME: Implement
				// check that new_input > 0 and < descriptor.bNrInPins
				// selector_status[uvc_ctrl_request.wIndex.bUnitId] = new_input;
				return TRUE;

			case GET_DEF:
				// First input is the default
				return handle_uvc_control_return_byte(0);

			case GET_CUR:
				// FIXME: Implement
				// return handle_uvc_control_return_byte(selector_status[uvc_ctrl_request.wIndex.bUnitId]);
				return TRUE;

			case GET_INFO:
				// UVC_CONTROL_CAP_AUTOUPDATE
				// UVC_CONTROL_CAP_ASYNCHRONOUS
				return handle_uvc_control_return_info(
					UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

			// Unsupported request type
			default:
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_REQUEST);
			}


		// 4.2.2.3 Processing Unit Control Requests
		// We don't implement most of the processing unit controls, not being a
		// camera.
		// 4.2.2.3.1 Backlight Compensation Control
		// 4.2.2.3.2 Brightness Control
		// 4.2.2.3.3 Contrast Control
		// 4.2.2.3.4 Contrast, Auto Control
		// 4.2.2.3.5 Gain Control
		// 4.2.2.3.7 Hue Control
		// 4.2.2.3.8 Hue, Auto Control
		// 4.2.2.3.9 Saturation Control
		// 4.2.2.3.10 Sharpness Control
		// 4.2.2.3.11 Gamma Control
		// 4.2.2.3.12 White Balance Temperature Control
		// 4.2.2.3.13 White Balance Temperature, Auto Control
		// 4.2.2.3.14 White Balance Component Control
		// 4.2.2.3.15 White Balance Component, Auto Control
		// 4.2.2.3.16 Digital Multiplier Control -- DEPRECATED
		// 4.2.2.3.17 Digital Multiplier Limit Control
		// 4.2.2.3.18 Analog Video Standard Control

		// 4.2.2.3.6 Power Line Frequency Control
		// Support setting 60Hz or 50Hz modes
		case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
			// assert uvc_ctrl_request.wLength == 1
			if (uvc_ctrl_request.wIndex.bUnitId != PROCESSING_UNIT_ID)
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_UNIT);

			switch (uvc_ctrl_request.bRequest) {
			case SET_CUR:
				// FIXME: Implement
				// if (new_input != UVC_PU_POWER_LINE_FREQUENCY_CONTROL_50HZ || new_input != UVC_PU_POWER_LINE_FREQUENCY_CONTROL_60HZ)
				//	return handle_uvc_control_set_error(ERROR_CODE_INVALID_VALUE);
				return TRUE;

			case GET_DEF:
				return handle_uvc_control_return_byte(UVC_PU_POWER_LINE_FREQUENCY_CONTROL_50HZ);

			case GET_CUR:
				// FIXME: Implement
				// return handle_uvc_control_return_byte();
				return TRUE;

			case GET_INFO:
				// UVC_CONTROL_CAP_AUTOUPDATE
				// UVC_CONTROL_CAP_ASYNCHRONOUS
				return handle_uvc_control_return_info(
					UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

			// Unsupported request type
			default:
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_REQUEST);
			}

		// 4.2.2.3.19 Analog Video Lock Status Control
		// Support getting information about a video signal
		case UVC_PU_ANALOG_LOCK_STATUS_CONTROL:
			// assert uvc_ctrl_request.wLength == 1
			if (uvc_ctrl_request.wIndex.bUnitId != PROCESSING_UNIT_ID)
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_UNIT);

			switch (uvc_ctrl_request.bRequest) {
			case GET_CUR:
				// FIXME: Implement
				// return handle_uvc_control_return_byte();
				return TRUE;

			case GET_INFO:
				// UVC_CONTROL_CAP_AUTOUPDATE
				// UVC_CONTROL_CAP_ASYNCHRONOUS
				return handle_uvc_control_return_info(
					UVC_CONTROL_CAP_GET);

			// Unsupported request type
			default:
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_REQUEST);
			}

		// 4.2.2.4 Encoding Units
		// Doesn't seem to be supported under Linux?
		// Complicated!?

		// 4.2.2.5 Extension Unit Control Requests
		// Not sure what to do here...

		default:
			return handle_uvc_control_set_error(ERROR_CODE_INVALID_CONTROL);
		}

	// 4.3 VideoStreaming Requests
	case UVC_REQUEST_TYPE_STREAM: {

		// assert wIndex.bEntityId == 0;

		// Should be sending to the videostream interface
		if (uvc_ctrl_request.wIndex.bInterface != UVC_DESCRIPTOR.videostream.interface.bInterfaceNumber)
			return handle_uvc_control_set_error(ERROR_CODE_INVALID_UNIT);
		
		// 4.3.1 Interface Control Requests
		switch (uvc_ctrl_request.wValue.bControl) {

		// 4.3.1.1 Video Probe and Commit Controls
		case UVC_VS_PROBE_CONTROL:
			switch (uvc_ctrl_request.bRequest) {
			case SET_CUR:
				return TRUE;

			case GET_MIN:
				return TRUE;
			case GET_MAX;
				return TRUE;
			// Get "number of steps"
			case GET_RES:
				return TRUE;
			// Get "default value"
			case GET_DEF:

				return TRUE;

			case GET_LEN:
				// assert uvc_ctrl_request.wLength == 1
				switch (uvc_bcd_version) {
				case UVC_BCD_V10:
					return handle_uvc_control_return_byte(sizeof(uvc_vs_control_data_v10));
				case UVC_BCD_V11:
					return handle_uvc_control_return_byte(sizeof(uvc_vs_control_data_v11));
				case UVC_BCD_V15:
					return handle_uvc_control_return_byte(sizeof(uvc_vs_control_data_v11));
				default:
					return FALSE;
				}

			// Get current
			// Returns the current state of the streaming interface. All
			// supported fields set to zero will be returned with an acceptable
			// negotiated value. Prior to the initial SET_CUR operation, the
			// GET_CUR state is undefined. This request shall stall in case of
			// negotiation failure.
			case GET_CUR:
				return TRUE;

			// Queries the capabilities and status of the Control. The value
			// returned for this request shall have bits D0 and D1 each set to
			// one (1), and the remaining bits set to zero (0) (see section
			// 4.1.2, “Get Request”).
			case GET_INFO:
				// UVC_CONTROL_CAP_AUTOUPDATE
				// UVC_CONTROL_CAP_ASYNCHRONOUS
				return handle_uvc_control_return_info(
					UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

			// Unsupported request type
			default:
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_REQUEST);
			}

		case UVC_VS_COMMIT_CONTROL:
			switch (uvc_ctrl_request.bRequest) {
			// Sets the device state. This sets the active device state.
			case SET_CUR:
				return TRUE;

			case GET_LEN:
				return TRUE;

			// Get current
			// Returns the current state of the streaming interface.
			case GET_CUR:
				return TRUE;

			// Queries the capabilities and status of the Control. The value
			// returned for this request shall have bits D0 and D1 each set to
			// one (1), and the remaining bits set to zero (0) (see section
			// 4.1.2, “Get Request”).
			case GET_INFO:
				// UVC_CONTROL_CAP_AUTOUPDATE
				// UVC_CONTROL_CAP_ASYNCHRONOUS
				return handle_uvc_control_return_info(
					UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

			// Unsupported request type
			default:
				return handle_uvc_control_set_error(ERROR_CODE_INVALID_REQUEST);
			}
		
		// 4.3.1.2 Video Still Probe Control and Still Commit Control
		// VS_STILL_PROBE_CONTROL
		// VS_STILL_COMMIT_CONTROL

		// 4.3.1.3 Synch Delay Control
		// 4.3.1.4 Still Image Trigger Control
		// 4.3.1.5 Generate Key Frame Control
		// 4.3.1.6 Update Frame Segment Control
		// 4.3.1.7 Stream Error Code Control
		
		default:
			return handle_uvc_control_set_error(ERROR_CODE_INVALID_CONTROL);
		}

	default:
		return FALSE;
	}

	// Which Unit is this targeted at
	switch(uvc_ctrl_request.wIndex) {
	case ALL_UNIT_ID:
	case CAMERA_UNIT_ID:
	case PROCESSING_UNIT_ID:
	case EXTENSION_UNIT_ID:
	case OUTPUT_UNIT_ID:



		return TRUE;
	default:
		return FALSE;	
	}


	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
		// Disable the autoptr size??
		SUDPTRCTL = 0x01;

		// Copy the valuesArray into EP0BUF
		for (i = 0; i < UVC_XXX_SIZE; i++)
			EP0BUF[i] = valuesArray.array[i];

		// Set the size
		EP0BCH = 0x00;
		SYNCDELAY;
		EP0BCL = UVC_XXX_SIZE;
		return TRUE;

		// FIXME: What do these do????
		// case UVC_SET_CUR:
		// case UVC_GET_RES:
		// case UVC_GET_LEN:
		// case UVC_GET_INFO:

	// The device indicates hardware default values for Unit, Terminal and
	// Interface Controls through their GET_DEF values. These values may be
	// used by the host to restore a control to its default setting.
	case UVC_GET_DEF:
		// if (uvc_ctrl_request.bmRequestType != 0b10100001) return FALSE;

		//if (uvc_ctrl_request.wValue
	
// SETUPDAT[0] == bmRequestType?
// 


	default:
		return FALSE;
	}
}

BYTE Configuration;	  // Current configuration
BYTE handle_get_configuration()
{
	return Configuration;
}

BOOL handle_set_configuration(BYTE cfg)
{
	Configuration = SETUPDAT[2];   //cfg;
	return TRUE;
}

BYTE AlternateSetting = 0;   // Alternate settings
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc)
{

	*alt_ifc = AlternateSetting;
	//EP0BUF[0] = AlternateSetting;
	//EP0BCH = 0;
	//EP0BCL = 1;
	return TRUE;
}

BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc)
{
	AlternateSetting = SETUPDAT[2];

	if (ifc==0 && alt_ifc==0) {
		// SEE TRM 2.3.7
		// reset toggles
		CDC_H2D_RESET(TOGGLE);
		CDC_D2H_RESET(TOGGLE);
		// restore endpoints to default condition
		CDC_H2D_RESET(FIFO);
		CDC_H2D_EP(BCL)=0x80;
		SYNCDELAY;
		CDC_H2D_EP(BCL)=0X80;
		SYNCDELAY;
		CDC_D2H_RESET(FIFO);
	}

	if (AlternateSetting == 1) {
		// reset UVC fifo
		SYNCDELAY; FIFORESET = 0x80;
		SYNCDELAY; FIFORESET = 0x06;
		SYNCDELAY; FIFORESET = 0x00;
	}

	return TRUE;
}


BOOL handle_get_descriptor() {
	return FALSE;
}
