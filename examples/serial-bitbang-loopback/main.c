#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>

#include "cdc.h"
#include "uart.h"

#define MSB(word) (((word) & 0xff00) >> 8)
#define LSB(word) ((word) & 0xff)

static void permute_data(uint8_t *data, uint16_t length);

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
  uart_init(9600, UART_MODE_RX_TX);

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  {
    uint16_t length = 0;
    uint16_t uart_n_sent = 0;
    uint16_t uart_n_rec = 0;

    while (1) {
      // check for data received from usb host
      if (length == 0 && !(EP2CS & _EMPTY)) {
        // copy received data to scratch buffer and save
        length = (EP2BCH << 8) | EP2BCL;
        xmemcpy(scratch, EP2FIFOBUF, length);

        // signalize we are ready for new data
        EP2BCL = 0;

        // modify the data
        permute_data(scratch, length);
      }

      // send bytes over uart until all have been sent
      if (uart_n_sent < length) {
        // we can fail if queue is full
        bool tx_ok = uart_push(scratch[uart_n_sent]);
        if (tx_ok) {
          uart_n_sent++;
        }
      }

      // when everything has been sent, restart receiving over USB CDC-ACM
      if (uart_n_sent == length) {
        length = 0;
        uart_n_sent = 0;
      }

      // copy any data received over UART to EP6 buffer
      // uart_pop returns false when there is no data available
      while (uart_pop(&EP6FIFOBUF[uart_n_rec])) {
        uart_n_rec++;
      }

      // when received anything, send it over usb
      if (uart_n_rec > 0) {
        // commit the packet
        EP6BCH = MSB(uart_n_rec);
        SYNCDELAY;
        EP6BCL = LSB(uart_n_rec);
        // restart receving
        uart_n_rec = 0;
      }

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
