#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>
#include <usbcdc.h>

#include "cdc.h"
#include "uvc.h"
#include "endpoints.h"

static void endpoints_config();

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Use newest chip features.
  REVCTL = _ENH_PKT|_DYN_OUT;

  // configure usb endpoints and fifos
  endpoints_config();

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  cdc_init();

  while (1) {
    cdc_poll();
  }
}

void handle_usb_setup(__xdata struct usb_req_setup *req) {
  if (cdc_handle_usb_setup(req))
    return;
  if (uvc_handle_usb_setup(req))
    return;
  STALL_EP0();
}

#define MSB(word) (((word) & 0xff00) >> 8)
#define LSB(word) ((word) & 0xff)

void endpoints_config() {
  // NAK all transfers.
  SYNCDELAY;
  FIFORESET = _NAKALL;

  // first all as invalid
  EP2CFG &= ~_VALID;
  EP4CFG &= ~_VALID;
  EP6CFG &= ~_VALID;
  EP8CFG &= ~_VALID;

  // Return FIFO setings back to default just in case previous firmware messed with them
  SYNCDELAY; PINFLAGSAB = 0x00;
  SYNCDELAY; PINFLAGSCD = 0x00;
  SYNCDELAY; FIFOPINPOLAR = 0x00;

  // configure FIFO interface
  // internal clock|48MHz|output to pin|normla polarity|syncronious mode|no gstate|slave FIFO interface mode [1:0]
  SYNCDELAY; IFCONFIG = _IFCLKSRC|_3048MHZ|_IFCLKOE|0|0|0|_IFCFG1|_IFCFG1;

  // CDC interrupt endpoint
  EP1INCFG = _VALID|_TYPE1|_TYPE0; // INTERRUPT IN.
  EP1OUTCFG &= ~_VALID; // EP1OUT not used

  // CDC 512-byte double buffed BULK OUT.
  EP_CDC_HOST2DEV_(CFG) = _VALID|_TYPE1|_BUF1;
  EP_CDC_HOST2DEV_(CS) = 0;

  // CDC 512-byte double buffed BULK IN.
  EP_CDC_DEV2HOST_(CFG) = _VALID|_DIR|_TYPE1|_BUF1;
  EP_CDC_DEV2HOST_(CS) = 0;

  // UVC 512-byte double buffered ISOCHRONOUS IN
  EP_UVC_(CFG) = _VALID|_DIR|_TYPE0|_SIZE|_BUF1;
  // FIFO: auto commit IN packets, set length of 512
  SYNCDELAY; EP_UVC_(FIFOCFG) = _AUTOIN|_ZEROLENIN;
  SYNCDELAY; EP_UVC_(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_UVC_(AUTOINLENL) = MSB(512);

  // UAC 512-byte double buffered ISOCHRONOUS IN
  EP_UAC_(CFG) = _VALID|_DIR|_TYPE0|_SIZE|_BUF1;
  SYNCDELAY; EP_UAC_(FIFOCFG) = _AUTOIN|_ZEROLENIN;
  SYNCDELAY; EP_UAC_(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_UAC_(AUTOINLENL) = MSB(512);

  // reset (and skip) endpoints
  SYNCDELAY; FIFORESET = EP_CDC_HOST2DEV|_NAKALL;
  SYNCDELAY; OUTPKTEND = EP_CDC_HOST2DEV|_SKIP;
  SYNCDELAY; OUTPKTEND = EP_CDC_HOST2DEV|_SKIP;
  SYNCDELAY; FIFORESET = EP_CDC_DEV2HOST|_NAKALL;
  SYNCDELAY; FIFORESET = EP_UVC|_NAKALL;
  SYNCDELAY; FIFORESET = EP_UAC|_NAKALL;

  // restore normal operation
  SYNCDELAY; FIFORESET = 0;
}

