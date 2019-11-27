#ifndef UVC_H
#define UVC_H

#include <fx2delay.h>
#include <fx2usb.h>

// UVC definitions
#define USB_UVC_SET_CUR  (0x01)
#define USB_UVC_GET_CUR  (0x81)
#define USB_UVC_GET_MIN  (0x82)
#define USB_UVC_GET_MAX  (0x83)
#define USB_UVC_GET_RES  (0x84)
#define USB_UVC_GET_LEN  (0x85)
#define USB_UVC_GET_INFO (0x86)
#define USB_UVC_GET_DEF  (0x87)

bool uvc_handle_usb_setup(__xdata struct usb_req_setup *req);

#endif /* UVC_H */
