#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>
#include <usbcdc.h>

#include "usb_config.h"
#include "cdc.h"
#include "uac.h"
#include "uvc.h"
#include "uart.h"

static void fx2_usb_config();

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Use newest chip features.
  REVCTL = _ENH_PKT|_DYN_OUT;

  __critical { // disable interrupts during configuration phase
    // configure usb endpoints and fifos
    fx2_usb_config();

    uart_init(9600);
  }

  EA = 1; // enable interrupts

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);


  // UART test
  uint32_t test_counter = 0;  // to slowly perform consequent steps
  int test_step = 0;
  const char *string = "N";

  while (1) {
    // slave fifos configured in auto mode

    // clears endpoint with received data
    cdc_poll();

    // performed in "human" time
    if (test_counter++ > 10000UL * 5) {
      test_counter = 0;

      switch (test_step++) {
        case 0:
          cdc_printf("rec=%d, byte=0x%02x '%c', ovf=%d\r\n", 
                    uart.received_flag, uart.rx_buf, uart.rx_buf, uart.overflow_flag);
          if (uart.received_flag) {
            uart.received_flag = false;
          }
          break;
        case 1:
          cdc_printf("sending 0x%02x '%c' ...\r\n", string[0], string[0]);
          uart_queue_send(string[0]);
          break;
        default:
          cdc_printf(" [ tick ]\r\n");
          test_step = 0;
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

#define MSB(word) (((word) & 0xff00) >> 8)
#define LSB(word) ((word) & 0xff)

void fx2_usb_config() {
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
  // SYNCDELAY; IFCONFIG = _IFCLKSRC|_3048MHZ|_IFCLKOE|0|0|0|_IFCFG1|_IFCFG1;
  SYNCDELAY; IFCONFIG = _IFCLKSRC|_3048MHZ|_IFCLKOE|0|0|0|0|0; // FIXME: change uart pins, now they collide with FIFO interface

  // CDC interrupt endpoint
  EP1INCFG = _VALID|_TYPE1|_TYPE0; // INTERRUPT IN.
  EP1OUTCFG &= ~_VALID; // EP1OUT not used

  // CDC 512-byte double buffed BULK OUT.
  EP_CDC_HOST2DEV(CFG) = _VALID|_TYPE1|_BUF1;
  EP_CDC_HOST2DEV(CS) = 0;
  SYNCDELAY; EP_CDC_HOST2DEV(FIFOCFG) = _AUTOIN|_ZEROLENIN;
  SYNCDELAY; EP_CDC_HOST2DEV(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_CDC_HOST2DEV(AUTOINLENL) = LSB(512);

  // CDC 512-byte double buffed BULK IN.
  EP_CDC_DEV2HOST(CFG) = _VALID|_DIR|_TYPE1|_BUF1;
  EP_CDC_DEV2HOST(CS) = 0;
  SYNCDELAY; EP_CDC_DEV2HOST(FIFOCFG) = _AUTOIN|_ZEROLENIN;
  SYNCDELAY; EP_CDC_DEV2HOST(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_CDC_DEV2HOST(AUTOINLENL) = LSB(512);

  // UVC 512-byte double buffered ISOCHRONOUS IN
  EP_UVC(CFG) = _VALID|_DIR|_TYPE0|_SIZE|_BUF1;
  // FIFO: auto commit IN packets, set length of 512
  SYNCDELAY; EP_UVC(FIFOCFG) = _AUTOIN|_ZEROLENIN;
  SYNCDELAY; EP_UVC(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_UVC(AUTOINLENL) = LSB(512);

  // UAC 512-byte double buffered ISOCHRONOUS IN
  EP_UAC(CFG) = _VALID|_DIR|_TYPE0|_SIZE|_BUF1;
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

