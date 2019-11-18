#include "cdc.h"

#include <fx2lib.h>
#include <fx2delay.h>

#include <fx2debug.h>
DEFINE_DEBUG_FN(bitbang_uart_send_byte, PB0, 115200)

volatile bool pending_ep6_in = false;
uint32_t scratch_buf_len = 0;

static void permute_data(uint8_t *data, uint16_t length) {
  uint16_t i;
  for (i = 0; i < length; ++i) {
    // translate a-zA-Z by N characters
    if ((data[i] >= 'a' && data[i] <= 'z') || (data[i] >= 'A' && data[i] <= 'Z')) {
      data[i] += 1;
    }
  }
}


void cdc_init() {

}

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

void cdc_poll() {

  if(!(EP2CS & _EMPTY)) {
    uint16_t length = (EP2BCH << 8) | EP2BCL;

    // if scratch buffer is full, ignore subsequent data
    if (scratch_buf_len < ARRAYSIZE(scratch)) {
      // store in data up to available scratch length, ignore rest
      if (length + scratch_buf_len > ARRAYSIZE(scratch)) {
        length = ARRAYSIZE(scratch) - scratch_buf_len;
      }

      // length bytes from EP2 buf to scratch+scratch_buf_len
      xmemcpy(scratch + scratch_buf_len, EP2FIFOBUF, length);
      scratch_buf_len += length;
    }

    // signalize we are ready for new data
    EP2BCL = 0;
  }

  // send data to EP6 if it is not full
  if (scratch_buf_len != 0 && !(EP6CS & _FULL)) {
    permute_data(scratch, scratch_buf_len);

    xmemcpy(EP6FIFOBUF, scratch, scratch_buf_len);
    EP6BCH = scratch_buf_len >> 8;
    SYNCDELAY;
    EP6BCL = scratch_buf_len;

    // simultaneously send the data over uart
    {
      uint16_t i;
      for (i = 0; i < scratch_buf_len; ++i) {
        __critical {
          bitbang_uart_send_byte(scratch[i]);
        }
      }
    }

    scratch_buf_len = 0;
  }
}
