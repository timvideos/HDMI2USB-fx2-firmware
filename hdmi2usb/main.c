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
  uart_init(9600, UART_MODE_RX);

  // Descriptors
  {
    struct cdc_configuration config = {
      .if_num_comm = USB_CFG_IF_CDC_COMMUNICATION,
      .ep_addr_comm = USB_CFG_EP_CDC_COMMUNICATION,
      .ep_addr_data_host2dev = USB_CFG_EP_CDC_HOST2DEV,
      .ep_addr_data_dev2host = USB_CFG_EP_CDC_DEV2HOST,
    };
    cdc_config(&config);
  }
  {
    struct uvc_configuration config = {
      .if_num_ctrl = USB_CFG_IF_UVC_VIDEO_CONTROL,
      .if_num_streaming = USB_CFG_IF_UVC_VIDEO_STREAMING,
      .ep_addr_streaming = USB_CFG_EP_UVC,
    };
    uvc_config(&config);
  }

  // Configure usb endpoints and fifos
  fx2_usb_config();

  // Wait until FPGA sends DNA over serial
  try_read_fpga_dna_uart(200);

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  EA = 1; // enable interrupts

  while (1) {
    // UVC, UAC and CDC OUT endpoints configured with slave fifos in auto mode - nothing to do
    // CDC IN endpoint in manual mode, we read CDC data using UART

    // get data from UART RX queue and commit it to CDC IN endpoint
    {
      uint16_t cdc_in_length = 0;
      uint8_t byte = 0;

      // order matters! first check if EP is full, then pop from queue
      while (!(EP_CDC_DEV2HOST(CS) & _FULL) && uart_pop(&byte)) {
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
  SYNCDELAY; IFCONFIG = _IFCLKSRC|_3048MHZ|_IFCLKOE|0|0|0|_IFCFG1|_IFCFG0;

  // CDC interrupt endpoint
  EP1INCFG = _VALID|_TYPE1|_TYPE0; // INTERRUPT IN.
  EP1OUTCFG &= ~_VALID; // EP1OUT not used

  // The "big" endpoints for interfacing to FPGA through slave FIFO interface.
  // FX2 Crossbar supports 2 OUT FIFOs and 2 IN FIFOs, so:
  // * we use OUT FIFOs for audio and video,
  // * we use one IN FIFO for CDC OUT endpoint (HOST->FX2->FPGA),
  // * CDC IN endpoint data (FPGA->FX2->HOST) is passed using UART, so FIFO
  //   is not configured here.

  // CDC 512-byte double buffered BULK OUT
  EP_CDC_HOST2DEV(CFG) = _VALID|_TYPE1|_BUF1;
  EP_CDC_HOST2DEV(CS) = 0;
  SYNCDELAY; EP_CDC_HOST2DEV(FIFOCFG) = _AUTOOUT;
  SYNCDELAY; EP_CDC_HOST2DEV(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_CDC_HOST2DEV(AUTOINLENL) = LSB(512);

  // CDC 512-byte double buffed BULK IN.
  EP_CDC_DEV2HOST(CFG) = _VALID|_DIR|_TYPE1|_BUF1;
  EP_CDC_DEV2HOST(CS) = 0;
  SYNCDELAY; EP_CDC_DEV2HOST(FIFOCFG) = 0;
  // manual mode, data passed to FPGA through UART

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
