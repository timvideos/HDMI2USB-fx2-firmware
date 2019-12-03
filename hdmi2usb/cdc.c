#include "cdc.h"

#include <fx2lib.h>
#include <fx2delay.h>

#include <stdio.h>
#include <string.h>
#include <fx2debug.h>

volatile bool pending_ie0 = false;
static uint32_t scratch_buf_len = 0;

static void permute_data(uint8_t *data, uint16_t length);
static void init_uart_ext_int();

bool cdc_handle_usb_setup(__xdata struct usb_req_setup *req) {
  // We *very specifically* declare that we do not support, among others, SET_CONTROL_LINE_STATE
  // request, but Linux sends it anyway and this results in timeouts propagating to userspace.
  // Linux will send us other requests we explicitly declare to not support, but those just fail.
  if (req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
      req->bRequest == USB_CDC_PSTN_REQ_SET_CONTROL_LINE_STATE &&
      req->wIndex == 0 && req->wLength == 0)
  {
    ACK_EP0();
    return true;
  }

  // We *very specifically* declare that we do not support, among others, GET_LINE_CODING request,
  // but Windows sends it anyway and this results in errors propagating to userspace.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_IN) &&
     req->bRequest == USB_CDC_PSTN_REQ_GET_LINE_CODING &&
     req->wIndex == 0 && req->wLength == 7)
  {
    __xdata struct usb_cdc_req_line_coding *line_coding =
        (__xdata struct usb_cdc_req_line_coding *)EP0BUF;
    line_coding->dwDTERate = 115200;
    line_coding->bCharFormat = USB_CDC_REQ_LINE_CODING_STOP_BITS_1;
    line_coding->bParityType = USB_CDC_REQ_LINE_CODING_PARITY_NONE;
    line_coding->bDataBits = 8;
    SETUP_EP0_BUF(sizeof(struct usb_cdc_req_line_coding));
    return true;
  }

  // We *very specifically* declare that we do not support, among others, SET_LINE_CODING request,
  // but Windows sends it anyway and this results in errors propagating to userspace.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
     req->bRequest == USB_CDC_PSTN_REQ_SET_LINE_CODING &&
     req->wIndex == 0 && req->wLength == 7)
  {
    SETUP_EP0_BUF(0);
    return true;
  }

  return false;
}
