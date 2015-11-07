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
	FRAME_INTERVAL_15FPS,
	FRAME_INTERVAL_7FPS,
};
const DWORD frameSize[] = {
	FRAME_SIZE_1024x768, // DVI
	FRAME_SIZE_1280x720, // HDMI
};

union uvc_streaming_control_array {
	struct uvc_streaming_control control;
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
		.wDelay			= 5, /* in ms */
		.dwMaxVideoFrameSize	= frameSize[0], // in bytes
		.dwMaxPayloadTransferSize = 1024, // in bytes
	},
};


BOOL handleUVCCommand(BYTE cmd)
{
    int i;

    switch(cmd) {
    case CLEAR_FEATURE:
        // FIXME: WTF is 0x21 !?
        if (SETUPDAT[0] != 0x21)
            return FALSE;

        EP0BCH = 0;
        EP0BCL = UVC_XXX_SIZE;
        SYNCDELAY;
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

    case UVC_GET_CUR:
    case UVC_GET_MIN:
    case UVC_GET_MAX:
        SUDPTRCTL = 0x01;

        for (i = 0; i < UVC_XXX_SIZE; i++)
            EP0BUF[i] = valuesArray.array[i];

        EP0BCH = 0x00;
        SYNCDELAY;
        EP0BCL = UVC_XXX_SIZE;
        return TRUE;

        // FIXME: What do these do????
        // case UVC_SET_CUR:
        // case UVC_GET_RES:
        // case UVC_GET_LEN:
        // case UVC_GET_INFO:

        // case UVC_GET_DEF:
        // FIXME: Missing this case causes the following errors
        // uvcvideo: UVC non compliance - GET_DEF(PROBE) not supported. Enabling workaround.
        // Unhandled Vendor Command: 87

    default:
        return FALSE;
    }
}

BYTE   Configuration;      // Current configuration
BYTE handle_get_configuration()
{
    return Configuration;
}

BOOL handle_set_configuration(BYTE cfg)
{
    Configuration = SETUPDAT[2];   //cfg;
    return TRUE;
}

BYTE   AlternateSetting = 0;   // Alternate settings
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
