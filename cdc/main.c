#include <fx2lib.h>
#include <fx2delay.h>
#include <fx2usb.h>
#include <usbcdc.h>
#include <ctype.h>

void handle_usb_setup(__xdata struct usb_req_setup *req) {
  // We *very specifically* declare that we do not support, among others, SET_CONTROL_LINE_STATE
  // request, but Linux sends it anyway and this results in timeouts propagating to userspace.
  // Linux will send us other requests we explicitly declare to not support, but those just fail.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
     req->bRequest == USB_CDC_PSTN_REQ_SET_CONTROL_LINE_STATE &&
     req->wIndex == 0 && req->wLength == 0) {
    ACK_EP0();
    return;
  }

  // We *very specifically* declare that we do not support, among others, GET_LINE_CODING request,
  // but Windows sends it anyway and this results in errors propagating to userspace.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_IN) &&
     req->bRequest == USB_CDC_PSTN_REQ_GET_LINE_CODING &&
     req->wIndex == 0 && req->wLength == 7) {
    __xdata struct usb_cdc_req_line_coding *line_coding =
      (__xdata struct usb_cdc_req_line_coding *)EP0BUF;
    line_coding->dwDTERate = 115200;
    line_coding->bCharFormat = USB_CDC_REQ_LINE_CODING_STOP_BITS_1;
    line_coding->bParityType = USB_CDC_REQ_LINE_CODING_PARITY_NONE;
    line_coding->bDataBits = 8;
    SETUP_EP0_BUF(sizeof(struct usb_cdc_req_line_coding));
    return;
  }

  // We *very specifically* declare that we do not support, among others, SET_LINE_CODING request,
  // but Windows sends it anyway and this results in errors propagating to userspace.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
     req->bRequest == USB_CDC_PSTN_REQ_SET_LINE_CODING &&
     req->wIndex == 0 && req->wLength == 7) {
    SETUP_EP0_BUF(0);
    return;
  }

  STALL_EP0();
}

volatile bool pending_ep6_in;

void isr_IBN() __interrupt {
  pending_ep6_in = true;
  CLEAR_USB_IRQ();
  NAKIRQ = _IBN;
  IBNIRQ = _IBNI_EP6;
}

int main() {
  // Run core at 48 MHz fCLK.
  CPUCS = _CLKSPD1;

  // Use newest chip features.
  REVCTL = _ENH_PKT|_DYN_OUT;

  // NAK all transfers.
  SYNCDELAY;
  FIFORESET = _NAKALL;

  // EP1IN is configured as INTERRUPT IN.
  EP1INCFG = _VALID|_TYPE1|_TYPE0;
  // EP1OUT is not used.
  EP1OUTCFG &= ~_VALID;

  // EP2 is configured as 512-byte double buffed BULK OUT.
  EP2CFG  =  _VALID|_TYPE1|_BUF1;
  EP2CS   = 0;
  // EP6 is configured as 512-byte double buffed BULK IN.
  EP6CFG  =  _VALID|_DIR|_TYPE1|_BUF1;
  EP6CS   = 0;
  // EP4/8 are not used.
  EP4CFG &= ~_VALID;
  EP8CFG &= ~_VALID;

  // Enable IN-BULK-NAK interrupt for EP6.
  IBNIE = _IBNI_EP6;
  NAKIE = _IBN;

  // Reset and prime EP2, and reset EP6.
  SYNCDELAY;
  FIFORESET = _NAKALL|2;
  SYNCDELAY;
  OUTPKTEND = _SKIP|2;
  SYNCDELAY;
  OUTPKTEND = _SKIP|2;
  SYNCDELAY;
  FIFORESET = _NAKALL|6;
  SYNCDELAY;
  FIFORESET = 0;

  // Re-enumerate, to make sure our descriptors are picked up correctly.
  usb_init(/*disconnect=*/true);

  {
    uint16_t length = 0;
    while(1) {
      if(length == 0 && !(EP2CS & _EMPTY)) {
        length = (EP2BCH << 8) | EP2BCL;
        xmemcpy(scratch, EP2FIFOBUF, length);
        EP2BCL = 0;

        // Permute the buffer in an amusing way.
        {
          uint16_t i;
          for(i = 0; i < length; i++)
          {
            char c = scratch[i];
                 if(isupper(c)) c = tolower(c);
            else if(islower(c)) c = toupper(c);
            scratch[i] = c;
          }
        }
      }

      if(length != 0 && pending_ep6_in) {
        xmemcpy(EP6FIFOBUF, scratch, length);
        EP6BCH = length >> 8;
        SYNCDELAY;
        EP6BCL = length;

        length = 0;
        pending_ep6_in = false;
      }
    }
  }
}
