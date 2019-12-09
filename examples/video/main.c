#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>

#include "uvc.h"

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Configure descriptors
  {
    __xdata struct uvc_configuration config = {
      .if_num_ctrl = 0,  // interface numbers start from 0
      .ep_addr_streaming = 2,  // use EP2 for streaming interface
    };
    uvc_config(&config);
  }

  // Configure UVC endpoint: 512-byte, double buffered, ISOCHRONOUS IN
  SYNCDELAY;
  EP2CFG = _VALID|_DIR|_TYPE0|_BUF1;

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  while (1) {

  }
}

/*** Reimplemented libfx2 USB handlers ****************************************/

// USB setup commands
void handle_usb_setup(__xdata struct usb_req_setup *req) {
  if (uvc_handle_usb_setup(req))
    return;
  STALL_EP0(); // not handled
}
