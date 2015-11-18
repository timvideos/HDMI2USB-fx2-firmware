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

// sdcc
#include <string.h>
#include <stdio.h>

// fx2lib
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

//FIXME: The typedef doesn't seem to pick up the __xdata/__code definitions!?
//typedef (__code const struct uvc_vs_control_data_v15) const_control_data_t;
//typedef ((__code const struct uvc_vs_control_data_v15) * const) const_control_data_ptr_t;
//typedef (__xdata struct uvc_vs_control_data_v15) control_data_t;
//typedef ((__xdata struct uvc_vs_control_data_v15) * const) control_data_ptr_t;
#define const_control_data_t \
	__code const struct uvc_vs_control_data_v15
#define const_control_data_ptr_t \
	__code const struct uvc_vs_control_data_v15 * const
#define control_data_t \
	__xdata struct uvc_vs_control_data_v15
#define control_data_ptr_t \
	__xdata struct uvc_vs_control_data_v15 * const


// FIXME: Move these somewhere.
inline void writeep0_byte(BYTE byte) {
	printf("bwriteep0\n");
	SUDPTRCTL = 0; // bmSDPAUTO;
	EP0BUF[0] = byte;
	// Set how much to transfer
	EP0BCH = 0;
	EP0BCL = 1; // Transfer starts with this write.
}

inline void writeep0_auto_code(const_control_data_ptr_t src, WORD len) {
	printf("cwriteep0\n");
	// Turn off "auto read length mode"
	SUDPTRCTL = 0; // bmSDPAUTO;
	// Set how much to transfer
	EP0BCH = MSB(len);
	EP0BCL = LSB(len);
	// Set the pointer to the data
	SUDPTRH = MSB((WORD)src);
	SUDPTRL = LSB((WORD)src); // Transfer starts with this write.
}
inline void writeep0_auto_xdata(control_data_ptr_t src, WORD len) {
	printf("xwriteep0\n");
	// Turn off "auto read length mode"
	SUDPTRCTL = 0; // bmSDPAUTO;
	// Set how much to transfer
	EP0BCH = MSB(len);
	EP0BCL = LSB(len); // Transfer starts with this write.
	// Set the pointer to the data
	SUDPTRH = MSB((WORD)src);
	SUDPTRL = LSB((WORD)src);
}

// ==================================================================
// ==================================================================


// FIXME: Look at the descriptors?
const WORD uvc_bcd_version = UVC_BCD_V11;


// FIXME: Figure out how to make this __at(0xE6B8) be __at(&SETUPDAT)
__xdata __at(0xE6B8) volatile struct uvc_control_request uvc_ctrl_request;
// FIXME: Figure out how to make this __at(0xXXX) be __at(&EP0BUF)
__xdata __at(0xE740) volatile struct uvc_vs_control_data_v15 ep0buffer;

#define UVC_DESCRIPTOR descriptors.highspeed.uvc

#define UVC_XXX_SIZE (sizeof(struct uvc_streaming_control))

BOOL handleUVCCommand(BYTE cmd)
{
	BOOL r = FALSE;
	// assert cmd == uvc_ctrl_request.bRequest
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

		EP0BCH = 0; // ACK
		EP0BCL = 0; // ACK
		return TRUE;
	}

	// if ((uvc_ctrl_request.bmRequestType & UVC_REQUEST_TYPE_DIR) == UVC_REQUEST_TYPE_SET)
	//	assert uvc_ctrl_request.bRequest == UVC_SET_CUR || uvc_ctrl_request.bRequest == UVC_SET_CUR_ALL
	switch (uvc_ctrl_request.bmRequestType & UVC_REQUEST_TYPE_MASK) {

	// 4.2 VideoControl Requests
	case UVC_REQUEST_TYPE_CONTROL:
		//r = uvc_control_request();
		break;

	// 4.3 VideoStreaming Requests
	case UVC_REQUEST_TYPE_STREAM:
		//r = uvc_stream_request();
		break;
	}

	if (r)
		uvc_control_clear_error();

	return r;
}

// Return a response to a GET_INFO request
// capabilities = UVC_CONTROL_CAP_XXX | UVC_CONTROL_CAP_XXX
inline BOOL uvc_control_get_info(BYTE control, BYTE capabilities) {
	// assert uvc_ctrl_request.wLength == 1
	uvc_control_return_byte(capabilities);
	return TRUE;
}

inline BOOL uvc_control_return_byte(BYTE data) {
	// Send the single byte response....
	writeep0_byte(data);
	return TRUE;
}

enum bmRequestControlErrorCodeType uvc_control_error_last = 0;
inline BOOL uvc_control_set_error(enum bmRequestControlErrorCodeType code) {
	// Set the error value
	uvc_control_error_last = 0;

	// Stall the endpoint
	STALLEP0();

	return TRUE;
}

inline void uvc_control_clear_error() {
	uvc_control_error_last = CONTROL_ERROR_CODE_NONE;
}


/*

// 4.2 VideoControl Requests
// ==========================================
inline BOOL uvc_control_request() {
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
	//case UNIT_ID_SELECTOR_4_ENCODER:
	//	return uvc_selector_control_request();
	//case UNIT_ID_ENCODER:
	//	return uvc_encoder_control_request();
	default:
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);
	}
}

// 4.2.1 Interface Control Requests
// ------------------------------------------

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
			return uvc_control_return_byte(UVC_VC_POWER_MODE_CONTROL_POWERED_AC | UVC_VC_POWER_MODE_CONTROL_DATA_MODE_FULL_POWER);

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
			return uvc_control_return_byte(uvc_control_error_last);

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

// 4.2.2 Unit and Terminal Control Requests
// ------------------------------------------
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
// ------------------------------------------
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
// ------------------------------------------
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
			return uvc_control_return_byte(UVC_PU_POWER_LINE_FREQUENCY_CONTROL_50HZ);

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
			return uvc_control_return_byte(UVC_PU_ANALOG_LOCK_STATUS_CONTROL_SIGNAL);

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
// ------------------------------------------
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
// ------------------------------------------
// Output Terminal's don't have any control.
inline BOOL uvc_output_control_request() {
	if (uvc_ctrl_request.wIndex.bUnitId != UNIT_ID_OUTPUT)
		return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_UNIT);

	return uvc_control_set_error(CONTROL_ERROR_CODE_INVALID_CONTROL);
}


// 4.3 VideoStreaming Requests
// ==========================================
inline BOOL uvc_stream_request() {
	// assert UVC_REQUEST_TYPE_STREAM:
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
}

// 4.3.1 Interface Control Requests
// ------------------------------------------
// 4.3.1.1 Video Probe and Commit control

BOOL uvc_stream_common_request(control_data_ptr_t dst);
void uvc_vs_control_data_populate(control_data_ptr_t src, control_data_ptr_t dst);

control_data_t stream_config_probed;
inline BOOL uvc_stream_probe_request() {
	// assert uvc_ctrl_request.bRequest == UVC_VS_PROBE_CONTROL
	return uvc_stream_common_request(&stream_config_probed);
}
control_data_t stream_config_committed;
inline BOOL uvc_stream_commit_request() {
	// assert uvc_ctrl_request.bRequest == UVC_VS_COMMIT_CONTROL
	return uvc_stream_common_request(&stream_config_committed);
}

#define DEFAULT_bFormatIndex 1
#define DEFAULT_bFrameIndex 1

// Default, min, max and step configurations
const_control_data_t stream_config_default = {
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
const_control_data_t stream_config_min = {
	.bFormatIndex		= 1,
	.bFrameIndex		= 1,
};
const_control_data_t stream_config_step = {
	.bFormatIndex		= 1,
	.bFrameIndex		= 1,
};


inline BYTE uvc_control_size() {
	switch (uvc_bcd_version) {
	case UVC_BCD_V10:
		return sizeof(struct uvc_vs_control_data_v10);
	case UVC_BCD_V11:
		return sizeof(struct uvc_vs_control_data_v11);
	case UVC_BCD_V15:
		return sizeof(struct uvc_vs_control_data_v15);
	default:
		return 0;
	}
}

BOOL uvc_stream_common_request(control_data_ptr_t dst) {
	// assert uvc_ctrl_request.bRequest == UVC_VS_PROBE_CONTROL
	BYTE size = uvc_control_size();

	switch (uvc_ctrl_request.bRequest) {

	case UVC_SET_CUR:
		// assert MAKEWORD(EP0BCH, EP0BCL) == size
		uvc_vs_control_data_populate(&ep0buffer, dst);
		return TRUE;

	// Get current
	// Returns the current state of the streaming interface.
	case UVC_GET_CUR:
		writeep0_auto_xdata(dst, size);
		return TRUE;

	case UVC_GET_MIN:
		writeep0_auto_code(&stream_config_min, size);
		return TRUE;
	case UVC_GET_MAX:
		writeep0_auto_code(&stream_config_max, size);
		return TRUE;
	// Get "number of steps"
	case UVC_GET_RES:
		writeep0_auto_code(&stream_config_step, size);
		return TRUE;
	// Get "default value"
	case UVC_GET_DEF:
		writeep0_auto_code(&stream_config_default, size);
		return TRUE;

	case UVC_GET_LEN:
		// assert uvc_ctrl_request.wLength == 1
		return uvc_control_return_byte(size);

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

// Copy any structure field from src into dst if dst value is zero.
// 
// FIXME: sdcc generates pretty crappy code for this. The dual auto pointers on
// the FX2 would be very useful for doing this.
// 
// D0: dwFrameInterval
// D1: wKeyFrameRate
// D2: wPFrameRate
// D3: wCompQuality
// D4: wCompWindowSize
// 
// Unsupported fields shall be set to zero by the device. 
// 
// Fields left for streaming parameters negotiation shall be set to zero by the host. 
// 
// For example, after a SET_CUR request initializing the FormatIndex and
// FrameIndex, the device will return the new negotiated field values for the
// supported fields when retrieving the Probe control GET_CUR attribute.
// 
// In order to avoid negotiation loops, the device shall always return streaming parameters with
// decreasing data rate requirements. 
// 
// Unsupported streaming parameters shall be reset by the streaming interface to
// supported values according to the negotiation loop avoidance rules. 
// 
// This convention allows the host to cycle through supported values of a field.
void uvc_vs_control_data_populate(control_data_ptr_t src, control_data_ptr_t dst) {
	if (uvc_bcd_version != UVC_BCD_V10 && 
		uvc_bcd_version != UVC_BCD_V11 &&
		uvc_bcd_version != UVC_BCD_V15)
		return;

	// This field is set by the host.
	if (dst->bFormatIndex == 0) {
		dst->bFormatIndex = src->bFormatIndex;
	}

	// This field is set by the host.
	if (dst->bFrameIndex == 0) {
		dst->bFrameIndex = src->bFrameIndex;
	}

	// When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase. The value must be from the range of values supported by the device.
	// When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
	if (dst->dwFrameInterval == 0) {
		dst->dwFrameInterval = src->dwFrameInterval;
	}

	// Use of this control is at the discretion of the device, and is indicated in the VS Input or Output Header descriptor.
	// When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase. The value must be from the range of values supported by the device.
	// When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
	if (dst->wKeyFrameRate == 0) {
		dst->wKeyFrameRate = src->wKeyFrameRate;
	}

	// Use of this control is at the discretion of the device, and is indicated in the VS Input or Output Header descriptor.
	// When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase. The value must be from the range of values supported by the device.
	// When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
	if (dst->wPFrameRate == 0) {
		dst->wPFrameRate = src->wPFrameRate;
	}

	// The resolution reported by this control will determine the number of discrete quality settings that it can support.
	// When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase. The value must be from the range of values supported by the device.
	// When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
	if (dst->wCompQuality == 0) {
		dst->wCompQuality = src->wCompQuality;
	}

	// When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase. The value must be from the range of values supported by the device.
	// When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
	if (dst->wCompWindowSize == 0) {
		dst->wCompWindowSize = src->wCompWindowSize;
	}

	// When used in conjunction with an IN endpoint, this field is set by the device and read only from the host.
	// When used in conjunction with an OUT endpoint, this field is set by the host and read only from the device.
	if (dst->wDelay == 0) {
		dst->wDelay = src->wDelay;
	}

	// For frame-based formats, this field indicates the maximum size of a single video frame.
	// When used in conjunction with an IN endpoint, this field is set by the device and read only from the host.
	// When used in conjunction with an OUT endpoint, this field is set by the host and read only from the device.
	if (dst->dwMaxVideoFrameSize == 0) {
		dst->dwMaxVideoFrameSize = src->dwMaxVideoFrameSize;
	}

	// This field is set by the device and read only from the host. Some host implementations restrict the maximum value permitted for this field.
	if (dst->dwMaxPayloadTransferSize == 0) {
		dst->dwMaxPayloadTransferSize = src->dwMaxPayloadTransferSize;
	}

	// UVC1.0 ends here
	if (uvc_bcd_version != UVC_BCD_V11 && 
		uvc_bcd_version != UVC_BCD_V15)
		return;

	// This parameter is set by the device and read only from the host.
	if (dst->dwClockFrequency == 0) {
		dst->dwClockFrequency = src->dwClockFrequency;
	}

	// Ignored for frame based formats (MJPEG + Uncompressed)
	if (dst->bmFramingInfo == 0) {
		dst->bmFramingInfo = src->bmFramingInfo;
	}

	// The host initializes bPreferedVersion and the following bMinVersion and bMaxVersion fields to zero on the first Probe Set. 
	// Upon Probe Get, the device shall return its preferred version, plus the minimum and maximum versions supported by the device (see bMinVersion and bMaxVersion below).
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
		return;

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
}

// 4.3.1.7 Stream Error Code Control
// ------------------------------------------
enum bmRequestStreamErrorCodeType uvc_stream_error_last = 0;

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
	uvc_stream_error_last = code;

	// Stall the endpoint
	STALLEP0();

	return TRUE;
}

inline void uvc_stream_clear_error() {
	uvc_stream_error_last = STREAM_ERROR_CODE_NONE;
}
*/
