#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>

#include "cdc.h"
#include "uart.h"

#define MSB(word) (((word) & 0xff00) >> 8)
#define LSB(word) ((word) & 0xff)

const char * const msg = "Hello world!\r\n";

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Use newest chip features
  REVCTL = _ENH_PKT|_DYN_OUT;

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

  // configure and start bitbang uart
  uart_init(BAUDRATE, UART_MODE_RX_TX);

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  while (1) {
    int n = 0;
    const char *c = msg;
    uint8_t byte;

    // copy the message to uart queue
    while (*c != 0) {
      if (uart_push(*c)) {
        c++;
        n++;
      }
    }

    // wait for the whole message
    {
      int i = 0;
      while (i < n) {
        if (uart_pop(&byte)) {
          EP6FIFOBUF[i] = byte;
          i++;
        }
      }
    }

    EP6BCH = MSB(n);
    SYNCDELAY;
    EP6BCL = LSB(n);
    SYNCDELAY;

    delay_ms(100);

    // skip any data received over cdc-acm
    OUTPKTEND = _SKIP|2;
  }
}

/*** Reimplemented libfx2 USB handlers ****************************************/

// USB setup commands
void handle_usb_setup(__xdata struct usb_req_setup *req) {
  if (cdc_handle_usb_setup(req))
    return;
  STALL_EP0(); // not handled
}
