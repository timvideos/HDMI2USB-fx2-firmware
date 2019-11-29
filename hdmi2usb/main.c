#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>
#include <usbcdc.h>

#include "usb_config.h"
#include "cdc.h"
#include "uac.h"
#include "uvc.h"
#include "uart.h"

#define DNA_START_CODE 0xfe

static char byte2hex(uint8_t byte);
static void fx2_usb_config();

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Use newest chip features.
  REVCTL = _ENH_PKT|_DYN_OUT;

  // configure usb endpoints and fifos
  fx2_usb_config();

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  EA = 1; // enable interrupts

  // store DNA received from FPGA and use a custom usb serial number to publish DNA
  uint8_t dna[8];
  char usb_serial_number[16];

  while (1) {
    // slave fifos configured in auto mode
    
    if (!(EP_CDC_DEV2HOST(CS) & _EMPTY)) {
      // inspect data in buffer to check if DNA is being passed
      if (EP_CDC_DEV2HOST(FIFOBUF)[0] == DNA_START_CODE) {
        const uint16_t dna_start_code_length = 1;
        uint16_t buf_length;
        uint8_t i;
    
        // wait until we have whole DNA
        // we can block as the rest of endpoints is handled by hardware
        do {
          buf_length = (EP_CDC_DEV2HOST(BCH) << 8) | EP_CDC_DEV2HOST(BCL);
        } while (buf_length < dna_start_code_length + ARRAYSIZE(dna));
    
        // save dna to USB serial number
        for (i = 0; i < ARRAYSIZE(dna); ++i) { // two chars for one byte
          uint8_t byte = EP_CDC_DEV2HOST(FIFOBUF)[dna_start_code_length + i];
          char c_low = byte2hex(byte & 0x0f);
          char c_high = byte2hex((byte & 0xf0) >> 4);
          // save in reverse order - LSB goes as last element of the string
          usb_serial_number[ARRAYSIZE(usb_serial_number) - 2 * i] = c_low;
          usb_serial_number[ARRAYSIZE(usb_serial_number) - 2 * i + 1] = c_high;
        }

        // make usb use modified DNA
        usb_user_strings[USB_STR_SERIAL_NUMBER - 1] = usb_serial_number;
    
        // re-enumerate to pick up new serial number
        usb_init(/*disconnect=*/true);
    
        // ignore endpoint data
        INPKTEND = USB_CFG_EP_CDC_DEV2HOST|_SKIP;
      } else { // commit regular data arming an IN tranfser
        INPKTEND = USB_CFG_EP_CDC_DEV2HOST;
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
  SYNCDELAY; EP_CDC_HOST2DEV(FIFOCFG) = _AUTOOUT|_ZEROLENIN;
  SYNCDELAY; EP_CDC_HOST2DEV(AUTOINLENH) = MSB(512);
  SYNCDELAY; EP_CDC_HOST2DEV(AUTOINLENL) = LSB(512);

  // CDC 512-byte double buffed BULK IN.
  EP_CDC_DEV2HOST(CFG) = _VALID|_DIR|_TYPE1|_BUF1;
  EP_CDC_DEV2HOST(CS) = 0;
  // no automatic committing as we need to first check for DNA

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

// byte must be 4-bit value!
char byte2hex(uint8_t byte) {
  char c;
  if (byte <= 9) {
    c = byte + '0';
  } else {
    c = byte + 'a';
  }
  return c;
}

