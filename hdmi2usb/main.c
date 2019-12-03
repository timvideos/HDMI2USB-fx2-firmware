#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>
#include <usbcdc.h>

#include "usb_config.h"
#include "cdc.h"
#include "dna.h"
#include "uac.h"
#include "uvc.h"
#include "uart.h"

#define MSB(word) (((word) & 0xff00) >> 8)
#define LSB(word) ((word) & 0xff)

static void fx2_usb_config();

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // configure UART for CDC data transmissions
  uart_init(9600);

  // Configure usb endpoints and fifos
  fx2_usb_config();

  // Wait until FPGA sends DNA over CDC IN endpoint or timeout (EP can't be in auto IN FIFO mode!)
  try_read_fpga_dna(200);

  // Reconfigure CDC IN endpoint in auto IN mode
  SYNCDELAY; INPKTEND = USB_CFG_EP_CDC_DEV2HOST|_SKIP;
  SYNCDELAY; FIFORESET = _NAKALL;
  SYNCDELAY; FIFORESET = USB_CFG_EP_CDC_DEV2HOST|_NAKALL;
  SYNCDELAY; EP_CDC_DEV2HOST(FIFOCFG) = _AUTOIN|_ZEROLENIN;
  SYNCDELAY; EP_CDC_DEV2HOST(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_CDC_DEV2HOST(AUTOINLENL) = LSB(512);
  SYNCDELAY; FIFORESET = 0;

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  EA = 1; // enable interrupts

  // start the state machine
  uart_start();

  while (1) {
    // slave fifos configured in auto mode

    // CDC endpoints are in manual mode

    // TODO: now we assume UART queues are never full!

    // get data from CDC OUT endpoint and send it through UART
    if (!(EP_CDC_HOST2DEV(CS) & _EMPTY)) {
      uint16_t i;
      uint16_t length = (EP_CDC_HOST2DEV(BCH) << 8) | EP_CDC_HOST2DEV(BCL);

      for (i = 0; i < length; ++i) {
        uint8_t byte = EP_CDC_HOST2DEV(FIFOBUF)[i];
        QUEUE_PUT(uart_tx_queue, byte);
      }

      // clear the endpoint
      EP_CDC_HOST2DEV(BCL) = 0;
    }

    // get data from UART RX queue and commit it to CDC IN endpoint
    {
      uint16_t cdc_in_length = 0;

      while (!QUEUE_EMPTY(uart_rx_queue) && !(EP_CDC_DEV2HOST(CS) & _FULL)) {
        uint8_t byte;
        QUEUE_GET(uart_rx_queue, byte);
        EP_CDC_DEV2HOST(FIFOBUF)[cdc_in_length++] = byte;
      }

      // if we have written anything, then commit the packet
      if (cdc_in_length > 0) {
        EP_CDC_DEV2HOST(BCH) = MSB(cdc_in_length);
        EP_CDC_DEV2HOST(BCL) = LSB(cdc_in_length);
      }
    }

  }
}

/*** Reimplemented libfx2 USB handlers ****************************************/

// USB setup commands
void handle_usb_setup(__xdata struct usb_req_setup *req) {
  if (cdc_handle_usb_setup(req))
    return;
  if (uvc_handle_usb_setup(req))
    return;
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

/*** USB registers configuration **********************************************/

void fx2_usb_config() {
  // NAK all transfers.
  SYNCDELAY;
  FIFORESET = _NAKALL;

  // Use newest chip features required for auto slave FIFO operation (TRM 9.3.1)
  REVCTL = _ENH_PKT|_DYN_OUT;

  // first all as invalid
  EP2CFG &= ~_VALID;
  EP4CFG &= ~_VALID;
  EP6CFG &= ~_VALID;
  EP8CFG &= ~_VALID;

  // Return FIFO setings back to default just in case previous firmware messed with them
  // Configure flags:
  // FLAGA = EP2 ~EF, FLAGB = EP4 ~EF, FLAGC = EP6 ~FF, FLAGD = EP8 ~FF
  // Configure PA7 as FLAGD
  SYNCDELAY; PINFLAGSAB = 0b10011000;
  SYNCDELAY; PINFLAGSCD = 0b11111110;
  SYNCDELAY; PORTACFG |= _FLAGD;
  SYNCDELAY; FIFOPINPOLAR = 0x00;

  // configure FIFO interface
  // internal clock|48MHz|output to pin|normla polarity|syncronious mode|no gstate|slave FIFO interface mode [1:0]
  SYNCDELAY; IFCONFIG = _IFCLKSRC|_3048MHZ|_IFCLKOE|0|0|0|_IFCFG1|_IFCFG1;

  // CDC interrupt endpoint
  EP1INCFG = _VALID|_TYPE1|_TYPE0; // INTERRUPT IN.
  EP1OUTCFG &= ~_VALID; // EP1OUT not used

  // CDC 512-byte double buffed BULK OUT.
  EP_CDC_HOST2DEV(CFG) = _VALID|_TYPE1|_BUF1;
  EP_CDC_HOST2DEV(CS) = 0;
  SYNCDELAY; EP_CDC_HOST2DEV(FIFOCFG) = /* _AUTOOUT */ 0;
  SYNCDELAY; EP_CDC_HOST2DEV(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_CDC_HOST2DEV(AUTOINLENL) = LSB(512);

  // CDC 512-byte double buffed BULK IN.
  EP_CDC_DEV2HOST(CFG) = _VALID|_DIR|_TYPE1|_BUF1;
  EP_CDC_DEV2HOST(CS) = 0;
  SYNCDELAY; EP_CDC_HOST2DEV(FIFOCFG) = 0;
  // no automatic committing as we need to first check for DNA

  // UVC 512-byte double buffered ISOCHRONOUS IN
  EP_UVC(CFG) = _VALID|_DIR|_TYPE0|_BUF1;
  // FIFO: auto commit IN packets, set length of 512
  SYNCDELAY; EP_UVC(FIFOCFG) = _AUTOIN|_ZEROLENIN;
  SYNCDELAY; EP_UVC(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_UVC(AUTOINLENL) = LSB(512);

  // UAC 512-byte double buffered ISOCHRONOUS IN
  EP_UAC(CFG) = _VALID|_DIR|_TYPE0|_BUF1;
  SYNCDELAY; EP_UAC(FIFOCFG) = _AUTOIN|_ZEROLENIN;
  SYNCDELAY; EP_UAC(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_UAC(AUTOINLENL) = LSB(512);

  // reset (and skip) endpoints
  SYNCDELAY; FIFORESET = USB_CFG_EP_CDC_HOST2DEV|_NAKALL;
  SYNCDELAY; OUTPKTEND = USB_CFG_EP_CDC_HOST2DEV|_SKIP;
  SYNCDELAY; OUTPKTEND = USB_CFG_EP_CDC_HOST2DEV|_SKIP;
  SYNCDELAY; FIFORESET = USB_CFG_EP_CDC_DEV2HOST|_NAKALL;
  SYNCDELAY; FIFORESET = USB_CFG_EP_UVC|_NAKALL;
  SYNCDELAY; FIFORESET = USB_CFG_EP_UAC|_NAKALL;

  // restore normal operation
  SYNCDELAY; FIFORESET = 0;
}
