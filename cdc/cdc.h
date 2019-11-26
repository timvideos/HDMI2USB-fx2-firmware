#ifndef CDC_H
#define CDC_H

#include <fx2usb.h>
#include <usbcdc.h>

#include "endpoints.h"

void cdc_init();
bool cdc_handle_usb_setup(__xdata struct usb_req_setup *req);
void cdc_handle_IBN();
void cdc_poll();

#endif /* CDC_H */
