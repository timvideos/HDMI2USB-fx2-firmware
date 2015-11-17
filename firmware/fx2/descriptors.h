
#include <stddef.h>

#include "date.h"
#include "uvclocal.h"
#include "descriptors_strings.h"

#include <linux/ch9.h>
#include <linux/ch9-extra.h>
#include <linux/video.h>
#include <linux/video-extra.h>
#include <linux/uvcvideo.h>

#include <linux/cdc.h>
#include <linux/cdc-extra.h>

#ifndef DESCRIPTORS_H_
#define DESCRIPTORS_H_

DECLARE_UVC_HEADER_DESCRIPTOR(1);
DECLARE_UVC_HEADER_DESCRIPTOR(2);

struct usb_section {
	struct usb_config_descriptor config;
	/* ;;;;;;;;;;;;;;;;;;;;;;;;;; UVC ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; */
	struct usb_uvc {
                /* Interface association descriptor */
		struct usb_interface_assoc_descriptor assoc_interface;
		struct usb_control {
                        /* Standard video control interface descriptor */
			struct usb_interface_descriptor interface;
                        /* Class specific VC interface header descriptor */
			struct UVC_HEADER_DESCRIPTOR(1) header;
			/* Input (camera) terminal descriptor */
			struct uvc_camera_terminal_descriptor camera;
			/* Processing unit descriptor */
			DECLARE_UVC_PROCESSING_UNIT_DESCRIPTOR(3) processing;
			/* Extension unit descriptor */
			DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(1, 3) extension;
			/* Output terminal descriptor */
			struct uvc_output_terminal_descriptor output;
		} videocontrol;
		struct usb_stream {
			/* Standard video streaming interface descriptor (alternate setting 0) */
			struct usb_interface_descriptor interface;
			/* Class-specific video streaming input header descriptor */
			DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(2, 1) header;
			/* ;;;;;;;;;;;;;; MJPEG ;;;;;;;;;;;;; */
			struct usb_stream_mjpeg {
				/* Class specific VS format descriptor */
				struct uvc_format_mjpeg format;
				/* Class specific VS frame descriptor 1 + 2 */
				DECLARE_UVC_FRAME_MJPEG(1) frames[2];
				/* VS Color Matching Descriptor Descriptor */
				struct uvc_color_matching_descriptor color;
			} mjpeg_stream;
			/* ;;;;;;;;;;;;;;;;;;;; YUY2 ;;;;;;;;;;;;;;;;;;;;;;;; */
			struct usb_stream_yuy2 {
				/* Class specific VS format descriptor */
				struct uvc_format_uncompressed format;
				/* Frame descriptors 1 + 2 */
				DECLARE_UVC_FRAME_UNCOMPRESSED(1) frames[2];
				/* VS Color Matching Descriptor Descriptor */
				struct uvc_color_matching_descriptor color;
			} yuy2_stream;
			/* Standard video streaming interface descriptor (alternate setting 1) */
			struct usb_interface_descriptor interface_alt;
		} videostream;
		/* Endpoint descriptor for streaming video data */
		struct usb_endpoint_descriptor endpoints[1];
	} uvc;
	/* ;;;;;;;;;;;;;;;;;;;;;;;;;; CDC ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; */
	struct usb_cdc {
		struct usb_interface_assoc_descriptor assoc_interface;

		struct usb_cdc_interface1 {
			struct usb_interface_descriptor interface;
			/* Header Functional Descriptor */
			struct usb_cdc_header_desc header;
			/* Union Functional Descriptor */
			struct usb_cdc_union_desc union_;
			/* CM Functional Descriptor */
			struct usb_cdc_call_mgmt_descriptor cm;
			/* ACM Functional Descriptor */
			struct usb_cdc_acm_descriptor acm;
			/* EP1 Descriptor */
			struct usb_endpoint_descriptor endpoints[1];
		} interface1;

		struct usb_cdc_interface2 {
			/* Virtual COM Port Data Interface Descriptor */
			struct usb_interface_descriptor interface;
			/* EP2OUT Descriptor + EP4 Descriptor */
			struct usb_endpoint_descriptor endpoints[2];
		} interface2;
	} cdc;
};

struct usb_descriptors {
	struct usb_device_descriptor device;
	struct usb_section highspeed;
	struct usb_qualifier_descriptor qualifier;
	WORD fullspeed;
	struct usb_descriptors_strings strings;
};

__xdata __at(DSCR_AREA) struct usb_descriptors descriptors;

__code __at(DSCR_AREA+offsetof(struct usb_descriptors, device)) WORD dev_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, qualifier)) WORD dev_qual_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, highspeed)) WORD highspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, fullspeed)) WORD fullspd_dscr;
__code __at(DSCR_AREA+offsetof(struct usb_descriptors, strings)) WORD dev_strings;

#endif // DESCRIPTORS_H_
