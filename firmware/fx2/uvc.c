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

__xdata union uvc_streaming_control_array {
	struct uvc_streaming_control control;
	BYTE array[sizeof(struct uvc_streaming_control)];
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


// FIXME: Figure out how to make this __at(0xE6B8) be __at(&SETUPDAT)
__xdata __at(0xE6B8) volatile struct uvc_control_request uvc_ctrl_request;
// FIXME: Figure out how to make this __at(0xXXX) be __at(&EP0BUF)
//__xdata __at(0xE740) volatile struct uvc_??? uvc_clear_feature;

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


	// 4.2 VideoControl Requests
	// 4.2.1 Interface Control Requests

		// 4.2.1.1 Power Mode Control	
		UVC_VC_VIDEO_POWER_MODE_CONTROL	

		// 4.2.1.2 Request Error Code Control
		UVC_VC_REQUEST_ERROR_CODE_CONTROL

	// 4.2.2 Unit and Terminal Control Requests

		// 4.2.2.1 Camera Terminal Control Requests
			// We don't implement most of the camera controls, not being a camera.
			// 4.2.2.1.1 Scanning Mode Control
			UVC_CT_SCANNING_MODE_CONTROL

			// Exposure related controls
			// 4.2.2.1.2 Auto-Exposure Mode Control
			// UVC_CT_AE_MODE_CONTROL
			// 4.2.2.1.3 Auto-Exposure Priority Control
			// UVC_CT_AE_PRIORITY_CONTROL
			// 4.2.2.1.4 Exposure Time (Absolute) Control
			// UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL
			// 4.2.2.1.5 Exposure Time (Relative) Control
			// UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL

			// Focus related controls
			// 4.2.2.1.6 Focus (Absolute) Control
			// 4.2.2.1.7 Focus (Relative) Control
			// 4.2.2.1.8 Focus, Simple Range
			// 4.2.2.1.9 Focus, Auto Control
			// Iris related controls
			// 4.2.2.1.10 Iris (Absolute) Control
			// 4.2.2.1.11 Iris (Relative) Control
			// Pan, Tilt and Zoom Controls
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


		// 4.2.2.2 Selector Unit Control Requests
			// SU_INPUT_SELECT_CONTROL

		// 4.2.2.3 Processing Unit Control Requests
			// 4.2.2.3.1 Backlight Compensation Control
			// 4.2.2.3.2 Brightness Control
			// 4.2.2.3.3 Contrast Control
			// 4.2.2.3.4 Contrast, Auto Control
			// 4.2.2.3.5 Gain Control

			// 4.2.2.3.6 Power Line Frequency Control

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

			// 4.2.2.3.19 Analog Video Lock Status Control

		// 4.2.2.4 Encoding Units
			// Doesn't seem to be supported under Linux?
			// Complicated!?

		// 4.2.2.5 Extension Unit Control Requests

	// 4.3 VideoStreaming Requests
	// 4.3.1 Interface Control Requests

		// 4.3.1.1 Video Probe and Commit Controls
		// UVC_VS_PROBE_CONTROL
		// UVC_VS_COMMIT_CONTROL

		// 4.3.1.2 Video Still Probe Control and Still Commit Control
		// VS_STILL_PROBE_CONTROL
		// VS_STILL_COMMIT_CONTROL

		// 4.3.1.3 Synch Delay Control
		// 4.3.1.4 Still Image Trigger Control
		


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
