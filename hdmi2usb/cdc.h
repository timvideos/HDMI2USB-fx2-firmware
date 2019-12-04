#ifndef CDC_H
#define CDC_H

#include <fx2usb.h>
#include <usbcdc.h>

#include "usb_config.h"

#include <stdio.h>
#define cdc_printf(...)                                         \
    do {                                                        \
      int len = sprintf(EP_CDC_DEV2HOST(FIFOBUF), __VA_ARGS__); \
      EP_CDC_DEV2HOST(BCH) = (len & 0xff00) >> 8;               \
      SYNCDELAY;                                                \
      EP_CDC_DEV2HOST(BCL) = len & 0xff;                        \
    } while (0);                                                \

// void cdc_init();
bool cdc_handle_usb_setup(__xdata struct usb_req_setup *req);
// void cdc_handle_IBN();
void cdc_poll();

#endif /* CDC_H */
