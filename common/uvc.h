#ifndef UVC_H
#define UVC_H

#include <fx2delay.h>
#include <fx2usb.h>

#include "uvc_defs.h"

// control interface and streaming interface
#define UVC_NUM_INTERFACES 2

// UVC descritors
extern usb_desc_if_assoc_c                   usb_uvc_if_assoc;
extern usb_desc_interface_c                  usb_uvc_std_ctrl_iface;
extern usb_desc_uvc_camera_terminal_c        usb_uvc_camera;
extern usb_desc_uvc_processing_unit_c        usb_uvc_processing_unit;
extern usb_desc_uvc_extension_unit_c         usb_uvc_extension_unit;
extern usb_desc_uvc_output_terminal_c        usb_uvc_output_terminal;
extern usb_desc_vc_if_header_c               usb_uvc_vc_if_header;
extern usb_desc_interface_c                  usb_uvc_std_streaming_iface_0;
extern usb_desc_uvc_vs_format_mjpeg_c        usb_uvc_mjpeg_vs_format;
extern usb_desc_uvc_vs_frame_c               usb_uvc_mjpeg_vs_frame_1;
extern usb_desc_uvc_vs_frame_c               usb_uvc_mjpeg_vs_frame_2;
extern usb_desc_uvc_color_matching_c         usb_uvc_mjpeg_color_matching;
extern usb_desc_uvc_vs_format_uncompressed_c usb_uvc_yuy2_vs_format;
extern usb_desc_uvc_vs_frame_c               usb_uvc_yuy2_vs_frame_1;
extern usb_desc_uvc_vs_frame_c               usb_uvc_yuy2_vs_frame_2;
extern usb_desc_uvc_color_matching_c         usb_uvc_yuy2_color_matching;
extern usb_desc_vs_if_in_header_c            usb_uvc_vs_if_in_header;
extern usb_desc_interface_c                  usb_uvc_std_streaming_iface_1;
extern usb_desc_endpoint_c                   usb_uvc_ep_in;

// Marco for adding descriptors to usb_configuration_c
#define UVC_DESCRIPTORS_LIST \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_if_assoc                 }, \
    { .interface =                             &usb_uvc_std_ctrl_iface           }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_vc_if_header             }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_camera                   }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_processing_unit          }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_extension_unit           }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_output_terminal          }, \
    { .interface =                             &usb_uvc_std_streaming_iface_0    }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_vs_if_in_header          }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_format          }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_frame_1         }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_vs_frame_2         }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_mjpeg_color_matching     }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_format           }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_frame_1          }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_vs_frame_2          }, \
    { .generic   = (struct usb_desc_generic *) &usb_uvc_yuy2_color_matching      }, \
    { .interface =                             &usb_uvc_std_streaming_iface_1    }, \
    { .endpoint  =                             &usb_uvc_ep_in                    },

struct uvc_configuration {
  uint8_t if_num_ctrl;
  // if_num_streaming = if_num_ctrl + 1
  uint8_t ep_addr_streaming;
};

/**
 * Modifies descriptors to use requested configuration
 */
void uvc_config(struct uvc_configuration *config);

/**
 * Handle UVC-specific USB setup requests. Return true if request has been handled.
 */
bool uvc_handle_usb_setup(__xdata struct usb_req_setup *req);

#endif /* UVC_H */
