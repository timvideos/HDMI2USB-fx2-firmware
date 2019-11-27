#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>
#include <usbcdc.h>

#include "cdc.h"
#include "uvc.h"
#include "endpoints.h"

void handle_usb_setup(__xdata struct usb_req_setup *req) {
  if (cdc_handle_usb_setup(req))
    return;
  if (uvc_handle_usb_setup(req))
    return;
  STALL_EP0();
}

void configure_endpoints() {
  // NAK all transfers.
  SYNCDELAY;
  FIFORESET = _NAKALL;

  // first all as invalid
  EP2CFG &= ~_VALID;
  EP4CFG &= ~_VALID;
  EP6CFG &= ~_VALID;
  EP8CFG &= ~_VALID;

  // CDC interrupt endpoint
  EP1INCFG = _VALID|_TYPE1|_TYPE0; // INTERRUPT IN.
  EP1OUTCFG &= ~_VALID; // EP1OUT not used

  // CDC 512-byte double buffed BULK OUT.
  EP_CDC_HOST2DEV_(CFG) = _VALID|_TYPE1|_BUF1;
  EP_CDC_HOST2DEV_(CS) = 0;

  // CDC 512-byte double buffed BULK IN.
  EP_CDC_DEV2HOST_(CFG) = _VALID|_DIR|_TYPE1|_BUF1;
  EP_CDC_DEV2HOST_(CS) = 0;

  // UVC 1024-byte (EP2 or EP6) double buffered ISOCHRONOUS IN
  EP_UVC_(CFG) = _VALID|_DIR|_TYPE0|_SIZE|_BUF1;

  // reset (and skip) endpoints
  SYNCDELAY; FIFORESET = EP_CDC_HOST2DEV|_NAKALL;
  SYNCDELAY; OUTPKTEND = EP_CDC_HOST2DEV|_SKIP;
  SYNCDELAY; OUTPKTEND = EP_CDC_HOST2DEV|_SKIP;

  SYNCDELAY; FIFORESET = EP_CDC_DEV2HOST|_NAKALL;

  SYNCDELAY; FIFORESET = EP_UVC|_NAKALL;

  // restore normal operation
  SYNCDELAY; FIFORESET = 0;
}

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Use newest chip features.
  REVCTL = _ENH_PKT|_DYN_OUT;

  configure_endpoints();

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  cdc_init();

  while (1) {
    cdc_poll();
  }
}
