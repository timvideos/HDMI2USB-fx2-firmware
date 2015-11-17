/*
 * Copyright 2013 Jahanzeb Ahmad
 * Copyright 2014 Tim Ansell <mithro@mithis.com>
 *
 * This file available under the GPL version 2 or later
 */

#include "cdc-config.h"

#include "uvc.h"
#include "uvclocal.h"
#include "descriptors.h"

#include <fx2regs.h>
#include <fx2macros.h>
#include <setupdat.h>
#include <eputils.h>
#include <delay.h>

#define SYNCDELAY SYNCDELAY4

// ==================================================================
// ==================================================================

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

// ==================================================================
// ==================================================================


//#define UVC_XXX_SIZE 26
#define UVC_XXX_SIZE (sizeof(struct uvc_vs_control_data_v11))

// FIXME: These are actually stored in the descriptor table!
const DWORD fps[] = {
	FRAME_INTERVAL_30FPS,
	FRAME_INTERVAL_7FPS,
};
const DWORD frameSize[] = {
	FRAME_SIZE_1024x768, // DVI
	FRAME_SIZE_1280x720, // HDMI
};

__xdata union uvc_streaming_control_array {
	struct uvc_vs_control_data_v11 control;
	BYTE array[sizeof(struct uvc_vs_control_data_v11)];
} valuesArray = {
	.control = {
		.bmHint			= UVC_PROBE_BMHINT_dwFrameInterval,
		.bFormatIndex		= 1,
		.bFrameIndex		= 1,
		.dwFrameInterval	= FRAME_INTERVAL_30FPS, /* in 100ns */
		.wKeyFrameRate		= 0,
		.wPFrameRate		= 0,
		.wCompQuality		= 0,
		.wCompWindowSize	= 0,
		.wDelay			= 5, /* in ms */
		.dwMaxVideoFrameSize	= FRAME_SIZE_1024x768, // in bytes
		.dwMaxPayloadTransferSize = 1024, // in bytes
	},
};

#define DEFAULT_bFormatIndex 1
#define DEFAULT_bFrameIndex 1

__code const struct uvc_vs_control_data_v11 stream_config_default = { // For UVC_GET_DEF
	.bFormatIndex		= DEFAULT_bFormatIndex,
	.bFrameIndex		= DEFAULT_bFrameIndex,
	.dwFrameInterval	= FRAME_INTERVAL_30FPS, //UVC_DESCRIPTOR.mjpeg_stream.frames[DEFAULT_bFormatIndex].dwFrameInterval[DEFAULT_bFrameIndex],
	.wDelay				= 5,
	.dwMaxVideoFrameSize = FRAME_SIZE_1024x768, //UVC_DESCRIPTORS.mjpeg_stream.frames[DEFAULT_bFormatIndex].dwMaxVideoFrameBufferSize,
	.dwMaxPayloadTransferSize = 1024,
	.bPreferedVersion	= UVC_PROBE_PAYLOAD_MJPEG_V11,
	.bMinVersion		= UVC_PROBE_PAYLOAD_MJPEG_V11,
	.bMaxVersion		= UVC_PROBE_PAYLOAD_MJPEG_V11,
};
#define stream_config_max	stream_config_default
__code const struct uvc_vs_control_data_v11 stream_config_min = {
	.bFormatIndex		= 1,
	.bFrameIndex		= 1,
};

__code const struct uvc_vs_control_data_v11 stream_config_step = {
	.bFormatIndex		= 1,
	.bFrameIndex		= 1,
};

__xdata struct uvc_vs_control_data_v11 stream_config_probed;
__xdata struct uvc_vs_control_data_v11 stream_config_commit;

// FIXME: Look at the descriptors?
const WORD uvc_bcd_version = UVC_BCD_V11;

/**
 * Copy any structure field from src into dst if dst value is zero.
 */
BOOL uvc_vs_control_data_populate(const struct uvc_vs_control_data_v15* src, struct uvc_vs_control_data_v15* dst) {
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

#define UVC_DESCRIPTOR descriptors.highspeed.uvc

BOOL handleUVCCommand(BYTE cmd)
{
	// assert cmd == uvc_ctrl_request.bRequest
	int i;
	/*
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
	} */

	// if ((uvc_ctrl_request.bmRequestType & UVC_REQUEST_TYPE_DIR) == UVC_REQUEST_TYPE_SET)
	//	assert uvc_ctrl_request.bRequest == UVC_SET_CUR || uvc_ctrl_request.bRequest == UVC_SET_CUR_ALL

	switch (uvc_ctrl_request.bmRequestType & UVC_REQUEST_TYPE_MASK) {

	// 4.2 VideoControl Requests
	case UVC_REQUEST_TYPE_CONTROL:
		// Should be sending to the control interface
		if (uvc_ctrl_request.wIndex.bInterface != UVC_DESCRIPTOR.videocontrol.interface.bInterfaceNumber)
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

		// Check sending to a valid UVC interface
		//if (uvc_ctrl_request.wIndex.bInterface > UVC_DESCRIPTOR.assoc_interface.bFirstInterface &&
		//	uvc_ctrl_request.wIndex.bInterface < UVC_DESCRIPTOR.assoc_interface.bFirstInterface + UVC_DESCRIPTOR.assoc_interface.bInterfaceCount)

		switch (uvc_ctrl_request.wIndex.bUnitId) {
		case UNIT_ID_NONE:
			return uvc_interface_control_request();

		case UNIT_ID_CAMERA:
			return uvc_camera_control_request();

		case UNIT_ID_PROCESSING:
			return uvc_processing_control_request();

		case UNIT_ID_EXTENSION:
			return uvc_extension_control_request();

		case UNIT_ID_OUTPUT:
			// Output terminals don't have any controls.
			return uvc_output_control_request();

		// For the future
		/*
		case UNIT_ID_SELECTOR_4_ENCODER:
			return uvc_selector_control_request();
		case UNIT_ID_ENCODER:
			return uvc_encoder_control_request();
		*/
		default:
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);
		}

	// 4.3 VideoStreaming Requests
	case UVC_REQUEST_TYPE_STREAM:
		// assert wIndex.bEntityId == 0;

		// Should be sending to the videostream interface
		if (uvc_ctrl_request.wIndex.bInterface != UVC_DESCRIPTOR.videostream.interface.bInterfaceNumber)
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);
		
		// 4.3.1 Interface Control Requests
		switch (uvc_ctrl_request.wValue.bControl) {

		// 4.3.1.1 Video Probe and Commit Controls
		case UVC_VS_PROBE_CONTROL:
			return uvc_stream_probe_request();
		case UVC_VS_COMMIT_CONTROL:
			return uvc_stream_commit_request();
		
		// 4.3.1.2 Video Still Probe Control and Still Commit Control
		// VS_STILL_PROBE_CONTROL
		// VS_STILL_COMMIT_CONTROL
		// 4.3.1.3 Synch Delay Control
		// 4.3.1.4 Still Image Trigger Control
		// 4.3.1.5 Generate Key Frame Control
		// 4.3.1.6 Update Frame Segment Control

		// 4.3.1.7 Stream Error Code Control
		case UVC_VS_STREAM_ERROR_CODE_CONTROL:
			return uvc_stream_commit_request();
		
		default:
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
		}

	default:
		return FALSE;
	}
}


// capabilities = UVC_CONTROL_CAP_XXX | UVC_CONTROL_CAP_XXX
BOOL uvc_control_get_info(BYTE control, BYTE capabilities) {
	// assert uvc_ctrl_request.wLength == 1

	// Do something...

	return TRUE;
}

BOOL uvc_control_return_byte(BYTE data) {
	// Send the single byte response....

	return TRUE;
}

// 4.2.1 Interface Control Requests
// ==========================================
inline BOOL uvc_interface_control_request() {
	// assert uvc_ctrl_request.wLength == 1
	if (uvc_ctrl_request.wIndex.bUnitId != 0)
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

	switch (uvc_ctrl_request.wValue.bControl) {
	// 4.2.1.1 Power Mode Control	
	case UVC_VC_VIDEO_POWER_MODE_CONTROL:
		switch (uvc_ctrl_request.bRequest) {
		case UVC_SET_CUR:
			// FIXME: Implement
			// struct uvc_vc_data_power_mode_control;
			return TRUE;

		case UVC_GET_CUR:
			// FIXME: Implement
			// RETURN UVC_VC_POWER_MODE_CONTROL_POWERED_AC | UVC_VC_POWER_MODE_CONTROL_DATA_MODE_FULL_POWER
			// struct uvc_vc_data_power_mode_control;
			return TRUE;

		case UVC_GET_INFO:
			return uvc_control_get_info(
				UVC_VC_VIDEO_POWER_MODE_CONTROL,
				UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

		// Unsupported request type
		default:
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
		}

	// 4.2.1.2 Request Error Code Control
	case UVC_VC_REQUEST_ERROR_CODE_CONTROL:
		switch (uvc_ctrl_request.bRequest) {
		case UVC_GET_CUR:
			// FIXME: Implement
			// struct uvc_vc_data_error_code_control
			return TRUE;

		case UVC_GET_INFO:
			return uvc_control_get_info(
				UVC_VC_REQUEST_ERROR_CODE_CONTROL,
				UVC_CONTROL_CAP_GET);

		// Unsupported request type
		default:
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
		}

	default:
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
	}
}

inline BOOL uvc_control_set_error(enum bmRequestStreamErrorCodeType code) {
	// Set the error value

	// Stall the endpoint
	return TRUE;
}


// 4.2.2 Unit and Terminal Control Requests
// ==========================================
// 4.2.2.1 Camera Terminal Control Requests
// We don't implement most of the camera controls, not being a camera.
inline BOOL uvc_camera_control_request() {
	// assert uvc_ctrl_request.wLength == 1
	if (uvc_ctrl_request.wIndex.bUnitId != UNIT_ID_CAMERA)
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

	switch (uvc_ctrl_request.wValue.bControl) {
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

		switch (uvc_ctrl_request.bRequest) {
		case UVC_SET_CUR:
			// FIXME: Implement
			// new_input = get_byte
			// if new_input != UVC_VC_SCANNING_MODE_CONTROL_PROGRESSIVE
			//  return uvc_control_set_error(ERROR_CODE_INVALID_VALUE);
			return TRUE;

		case UVC_GET_DEF:
		case UVC_GET_CUR:
			return uvc_control_return_byte(
				UVC_VC_SCANNING_MODE_CONTROL_PROGRESSIVE);

		case UVC_GET_INFO:
			return uvc_control_get_info(
				UVC_CT_SCANNING_MODE_CONTROL,
				UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

		// Unsupported request type
		default:
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
		}

	default:
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
	}
}

// 4.2.2.2 Selector Unit Control Requests
inline BOOL uvc_selector_control_request() {
	// assert uvc_ctrl_request.wLength == 1

	// FIXME: Actually add a selector unit
	if (uvc_ctrl_request.wIndex.bUnitId != UNIT_ID_SELECTOR_4_ENCODER)
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

	switch (uvc_ctrl_request.wValue.bControl) {
	case UVC_SU_INPUT_SELECT_CONTROL:
		switch (uvc_ctrl_request.bRequest) {
		case UVC_SET_CUR:
			// FIXME: Implement
			// check that new_input > 0 and < descriptor.bNrInPins
			// selector_status[uvc_ctrl_request.wIndex.bUnitId] = new_input;
			return TRUE;

		case UVC_GET_DEF:
			// First input is the default
			return uvc_control_return_byte(0);

		case UVC_GET_CUR:
			// FIXME: Implement
			// return uvc_control_return_byte(selector_status[uvc_ctrl_request.wIndex.bUnitId]);
			return TRUE;

		case UVC_GET_INFO:
			// UVC_CONTROL_CAP_AUTOUPDATE
			// UVC_CONTROL_CAP_ASYNCHRONOUS
			return uvc_control_get_info(
				UVC_SU_INPUT_SELECT_CONTROL,
				UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

		// Unsupported request type
		default:
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
		}

	default:
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
	}
}

// 4.2.2.3 Processing Unit Control Requests
// We don't implement most of the processing unit controls, not being a
// camera.
inline BOOL uvc_processing_control_request() {
	// assert uvc_ctrl_request.wLength == 1
	if (uvc_ctrl_request.wIndex.bUnitId != UNIT_ID_PROCESSING)
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

	switch (uvc_ctrl_request.wValue.bControl) {
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

		switch (uvc_ctrl_request.bRequest) {
		case UVC_SET_CUR:
			// FIXME: Implement
			// if (new_input != UVC_PU_POWER_LINE_FREQUENCY_CONTROL_50HZ || new_input != UVC_PU_POWER_LINE_FREQUENCY_CONTROL_60HZ)
			//	return uvc_control_set_error(ERROR_CODE_INVALID_VALUE);
			return TRUE;

		case UVC_GET_DEF:
			return uvc_control_return_byte(UVC_PU_POWER_LINE_FREQUENCY_CONTROL_50HZ);

		case UVC_GET_CUR:
			// FIXME: Implement
			// return uvc_control_return_byte();
			return TRUE;

		case UVC_GET_INFO:
			// UVC_CONTROL_CAP_AUTOUPDATE
			// UVC_CONTROL_CAP_ASYNCHRONOUS
			return uvc_control_get_info(
				UVC_PU_POWER_LINE_FREQUENCY_CONTROL,
				UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

		// Unsupported request type
		default:
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
		}

	// 4.2.2.3.19 Analog Video Lock Status Control
	// Support getting information about a video signal
	case UVC_PU_ANALOG_LOCK_STATUS_CONTROL:
		// assert uvc_ctrl_request.wLength == 1
		if (uvc_ctrl_request.wIndex.bUnitId != UNIT_ID_PROCESSING)
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

		switch (uvc_ctrl_request.bRequest) {
		case UVC_GET_CUR:
			// FIXME: Implement
			// return uvc_control_return_byte();
			return TRUE;

		case UVC_GET_INFO:
			// UVC_CONTROL_CAP_AUTOUPDATE
			// UVC_CONTROL_CAP_ASYNCHRONOUS
			return uvc_control_get_info(
				UVC_PU_ANALOG_LOCK_STATUS_CONTROL,
				UVC_CONTROL_CAP_GET);

		// Unsupported request type
		default:
			return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
		}

	default:
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
	}
}

// 4.2.2.4 Encoding Units
// Doesn't seem to be supported under Linux?
// Complicated!?
inline BOOL uvc_encoding_control_request() {
	// assert uvc_ctrl_request.wLength == 1
	if (uvc_ctrl_request.wIndex.bUnitId != UNIT_ID_ENCODER)
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

	return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
}

// 4.2.2.5 Extension Unit Control Requests
// Not sure what to do here...
inline BOOL uvc_extension_control_request() {
	if (uvc_ctrl_request.wIndex.bUnitId != UNIT_ID_EXTENSION)
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

	return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
}

// 4.2.2.?? Output Terminal Control Requests
// Output Terminal's don't have any control.
inline BOOL uvc_output_control_request() {
	if (uvc_ctrl_request.wIndex.bUnitId != UNIT_ID_OUTPUT)
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

	return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
}

// 4.3.1 Interface Control Requests
// ==========================================
// 4.3.1.1 Video Probe control
inline BOOL uvc_stream_probe_request() {
	// assert uvc_ctrl_request.bRequest == UVC_VS_PROBE_CONTROL

	switch (uvc_ctrl_request.bRequest) {
	case UVC_SET_CUR:
		return TRUE;

	case UVC_GET_MIN:
		return TRUE;
	case UVC_GET_MAX:
		return TRUE;
	// Get "number of steps"
	case UVC_GET_RES:
		return TRUE;
	// Get "default value"
	case UVC_GET_DEF:

		return TRUE;

	case UVC_GET_LEN:
		// assert uvc_ctrl_request.wLength == 1
		switch (uvc_bcd_version) {
		case UVC_BCD_V10:
			return uvc_control_return_byte(sizeof(struct uvc_vs_control_data_v10));
		case UVC_BCD_V11:
			return uvc_control_return_byte(sizeof(struct uvc_vs_control_data_v11));
		case UVC_BCD_V15:
			return uvc_control_return_byte(sizeof(struct uvc_vs_control_data_v15));
		default:
			return FALSE;
		}

	// Get current
	// Returns the current state of the streaming interface. All
	// supported fields set to zero will be returned with an acceptable
	// negotiated value. Prior to the initial UVC_SET_CUR operation, the
	// UVC_GET_CUR state is undefined. This request shall stall in case of
	// negotiation failure.
	case UVC_GET_CUR:
		return TRUE;

	// Queries the capabilities and status of the Control. The value
	// returned for this request shall have bits D0 and D1 each set to
	// one (1), and the remaining bits set to zero (0) (see section
	// 4.1.2, “Get Request”).
	case UVC_GET_INFO:
		// UVC_CONTROL_CAP_AUTOUPDATE
		// UVC_CONTROL_CAP_ASYNCHRONOUS
		return uvc_control_get_info(
			UVC_VS_PROBE_CONTROL,
			UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

	// Unsupported request type
	default:
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
	}
}

// 4.3.1.1 Video Commit control
inline BOOL uvc_stream_commit_request() {
	// assert uvc_ctrl_request.bRequest == UVC_VS_COMMIT_CONTROL

	switch (uvc_ctrl_request.bRequest) {
	// Sets the device state. This sets the active device state.
	case UVC_SET_CUR:
		return TRUE;

	case UVC_GET_LEN:
		return TRUE;

	// Get current
	// Returns the current state of the streaming interface.
	case UVC_GET_CUR:
		return TRUE;

	// Queries the capabilities and status of the Control. The value
	// returned for this request shall have bits D0 and D1 each set to
	// one (1), and the remaining bits set to zero (0) (see section
	// 4.1.2, “Get Request”).
	case UVC_GET_INFO:
		// UVC_CONTROL_CAP_AUTOUPDATE
		// UVC_CONTROL_CAP_ASYNCHRONOUS
		return uvc_control_get_info(
			UVC_VS_COMMIT_CONTROL,
			UVC_CONTROL_CAP_GET | UVC_CONTROL_CAP_SET);

	// Unsupported request type
	default:
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
	}
}

// 4.3.1.7 Stream Error Code Control
inline BOOL uvc_stream_error_request() {
	// assert uvc_ctrl_request.bRequest == UVC_VC_REQUEST_ERROR_CODE_CONTROL
	// 4.2.1.2 Request Error Code Control
	switch (uvc_ctrl_request.bRequest) {
	case UVC_GET_CUR:
		// FIXME: Implement
		// struct uvc_vc_data_error_code_control
		return TRUE;

	case UVC_GET_INFO:
		return uvc_control_get_info(
			UVC_VC_REQUEST_ERROR_CODE_CONTROL,
			UVC_CONTROL_CAP_GET);

	// Unsupported request type
	default:
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_REQUEST);
	}
}

inline BOOL uvc_stream_set_error(enum bmRequestStreamErrorCodeType code) {
	// Set the error value

	// Stall the endpoint
	return TRUE;
}
