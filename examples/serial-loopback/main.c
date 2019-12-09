#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>

#include "cdc.h"

#define MSB(word) (((word) & 0xff00) >> 8)
#define LSB(word) ((word) & 0xff)

static void permute_data(uint8_t *data, uint16_t length);

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Configure descriptors
  {
    __xdata struct cdc_configuration config = {
      .if_num_comm = 0,
      .ep_addr_comm = 1,
      .ep_addr_data_host2dev = 2,
      .ep_addr_data_dev2host = 6,
    };
    cdc_config(&config);
  }

  // Control endpoint
  EP1INCFG = _VALID|_TYPE1|_TYPE0; // INTERRUPT IN.
  EP1OUTCFG &= ~_VALID; // EP1OUT not used

  // Host->Device endpoint: 512-byte, double buffered, BULK OUT
  SYNCDELAY; EP2CFG = _VALID|_TYPE1|_BUF1;
  // Device->Host endpoint: 512-byte, double buffered, BULK IN
  SYNCDELAY; EP6CFG = _VALID|_DIR|_TYPE1|_BUF1;

  // reset endpoint fifos
  SYNCDELAY; FIFORESET = _NAKALL;
  SYNCDELAY; FIFORESET = 2;
  SYNCDELAY; FIFORESET = 6;
  // clean buffers for OUT endpoint (double buffered)
  SYNCDELAY; OUTPKTEND = _SKIP|2;
  SYNCDELAY; OUTPKTEND = _SKIP|2;
  // restore normal operation
  SYNCDELAY; FIFORESET = 0;

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  while (1) {

    if (!(EP2CS & _EMPTY)) {
      // if we received any data copy it to dev2host endpoint buffer
      uint16_t length = (EP2BCH << 8) | EP2BCL;
      xmemcpy(EP6FIFOBUF, EP2FIFOBUF, length);

      // signalize we are ready for new data
      EP2BCL = 0;

      // modify it before resending
      permute_data(EP6FIFOBUF, length);

      // send the data to host
      EP6BCH = MSB(length);
      SYNCDELAY;
      EP6BCL = LSB(length);
    }

  }
}

/*** Reimplemented libfx2 USB handlers ****************************************/

// USB setup commands
void handle_usb_setup(__xdata struct usb_req_setup *req) {
  if (cdc_handle_usb_setup(req))
    return;
  STALL_EP0(); // not handled
}


void permute_data(uint8_t *data, uint16_t length) {
  uint16_t i;
  for (i = 0; i < length; ++i) {
    // translate a-zA-Z by N characters
    if ((data[i] >= 'a' && data[i] <= 'z') || (data[i] >= 'A' && data[i] <= 'Z')) {
      data[i] += 1;
    }
  }
}
