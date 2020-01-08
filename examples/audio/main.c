#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>

#include "uac.h"

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Configure descriptors
  {
    __xdata struct uac_configuration config = {
      .if_num_ctrl = 0,  // interface numbers start from 0
      .ep_addr_streaming = 2,  // use EP2 for streaming interface
      .i_str_channel_left = 3,
    };
    uac_config(&config);
  }

  // Configure UAC endpoint: 512-byte, double buffered, ISOCHRONOUS IN
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
  STALL_EP0(); // not handled
}

// Set active interface _alternate setting_
bool handle_usb_set_interface(uint8_t interface, uint8_t alt_setting) {
  if (uac_handle_usb_set_interface(interface, alt_setting))
    return true;
  return false; // not handled
}

// Set current interface _alternate setting_
void handle_usb_get_interface(uint8_t interface) {
  if (uac_handle_usb_get_interface(interface))
    return;
  STALL_EP0(); // not handled
}
