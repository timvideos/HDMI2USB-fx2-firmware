#ifndef UVC_H
#define UVC_H

#include <fx2delay.h>
#include <fx2usb.h>

#include "uvc_defs.h"

bool uvc_handle_usb_setup(__xdata struct usb_req_setup *req);

#endif /* UVC_H */
