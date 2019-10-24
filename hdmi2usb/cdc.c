#include "cdc.h"

#include "macros.h"

volatile uint16_t cdc_queued_bytes = 0;

struct usb_cdc_line_coding cdc_current_line_coding = {
    .bDTERate0 = LSB(2400),
    .bDTERate1 = MSB(2400),
    .bDTERate2 = 0,
    .bDTERate3 = 0,
    .bCharFormat = USB_CDC_1_STOP_BITS,
    .bParityType = USB_CDC_NO_PARITY,
    .bDataBits = 8};

void cdc_receive_poll() {
  if (!(EP2468STAT & bmCDC_H2D_EP(E))) {
    uint16_t bytes = MAKEWORD(CDC_H2D_EP(BCH), CDC_H2D_EP(BCL));
    cdcuser_receive_data(CDC_H2D_EP(FIFOBUF), bytes);
    CDC_H2D_EP(BCL) = 0x80;  // Mark us ready to receive again.
  }
  // FIXME: Send the interrupt thingy
}

bool cdc_handle_command(uint8_t cmd) {
  int i;
  uint8_t* line_coding = (uint8_t*)&cdc_current_line_coding;
  uint32_t baud_rate = 0;

  switch (cmd) {
    case USB_CDC_REQ_SET_LINE_CODING:
      EUSB = 0;
      SUDPTRCTL = 0x01;
      EP0BCL = 0x00;
      SUDPTRCTL = 0x00;
      EUSB = 1;

      while (EP0BCL != 7)
        ;
      SYNCDELAY;

      for (i = 0; i < 7; i++)
        line_coding[i] = EP0BUF[i];

      // FIXME: Make this following line work rather then the if statement chain!
      //                baud_rate = MAKEDWORD(
      //			MAKEWORD(cdc_current_line_coding.bDTERate3, cdc_current_line_coding.bDTERate2),
      //			MAKEWORD(cdc_current_line_coding.bDTERate1, cdc_current_line_coding.bDTERate0));
      baud_rate = MAKEDWORD(
          MAKEWORD(line_coding[3], line_coding[2]),
          MAKEWORD(line_coding[1], line_coding[0]));

      if (!cdcuser_set_line_rate(baud_rate))
        ;  //EP0STALL();

      return true;

    case USB_CDC_REQ_GET_LINE_CODING:
      SUDPTRCTL = 0x01;

      for (i = 0; i < 7; i++)
        EP0BUF[i] = line_coding[i];

      EP0BCH = 0x00;
      SYNCDELAY;
      EP0BCL = 7;
      SYNCDELAY;
      while (EP0CS & 0x02)
        ;
      SUDPTRCTL = 0x00;

      return true;

    case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
      return true;

    default:
      return false;
  }
}
