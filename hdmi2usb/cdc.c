#include "cdc.h"

#include <fx2lib.h>
#include <fx2delay.h>

#include <stdio.h>
#include <string.h>
#include <fx2debug.h>

volatile bool pending_ie0 = false;
static uint32_t scratch_buf_len = 0;

static void permute_data(uint8_t *data, uint16_t length);
static void init_uart_ext_int();


// blocking software uart
/* DEFINE_DEBUG_FN(bitbang_uart_send_byte, PB0, BAUDRATE) */

/* #include <stdio.h> */
/* DEFINE_DEBUG_PUTCHAR_FN(PB1, BAUDRATE) */


// external interrupt on pin INT0
void isr_IE0() __interrupt(_INT_IE0) {
  // IE0 is automatically cleared by hardware
  pending_ie0 = true;
}



void cdc_init() {

}

void cdc_poll() {
  if(!(EP_CDC_HOST2DEV(CS) & _EMPTY)) {
    // signalize we are ready for new data
    EP_CDC_HOST2DEV(BCL) = 0;
  }
}

bool cdc_handle_usb_setup(__xdata struct usb_req_setup *req) {
  // We *very specifically* declare that we do not support, among others, SET_CONTROL_LINE_STATE
  // request, but Linux sends it anyway and this results in timeouts propagating to userspace.
  // Linux will send us other requests we explicitly declare to not support, but those just fail.
  if (req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
      req->bRequest == USB_CDC_PSTN_REQ_SET_CONTROL_LINE_STATE &&
      req->wIndex == 0 && req->wLength == 0)
  {
    ACK_EP0();
    return true;
  }

  // We *very specifically* declare that we do not support, among others, GET_LINE_CODING request,
  // but Windows sends it anyway and this results in errors propagating to userspace.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_IN) &&
     req->bRequest == USB_CDC_PSTN_REQ_GET_LINE_CODING &&
     req->wIndex == 0 && req->wLength == 7)
  {
    __xdata struct usb_cdc_req_line_coding *line_coding =
        (__xdata struct usb_cdc_req_line_coding *)EP0BUF;
    line_coding->dwDTERate = 115200;
    line_coding->bCharFormat = USB_CDC_REQ_LINE_CODING_STOP_BITS_1;
    line_coding->bParityType = USB_CDC_REQ_LINE_CODING_PARITY_NONE;
    line_coding->bDataBits = 8;
    SETUP_EP0_BUF(sizeof(struct usb_cdc_req_line_coding));
    return true;
  }

  // We *very specifically* declare that we do not support, among others, SET_LINE_CODING request,
  // but Windows sends it anyway and this results in errors propagating to userspace.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
     req->bRequest == USB_CDC_PSTN_REQ_SET_LINE_CODING &&
     req->wIndex == 0 && req->wLength == 7)
  {
    SETUP_EP0_BUF(0);
    return true;
  }

  return false;
}

#if 0
void cdc_print(const char *string) {
  uint16_t i;
  uint16_t len = strlen(string);
  if (len > 512)
    len = 512;

  // __critical {
  //   printf("string = %s\n", string);
  // }

  for (i = 0; i < len; ++i) {
    EP_CDC_DEV2HOST(FIFOBUF)[i] = ((uint8_t *) string)[i];
  }
  EP_CDC_DEV2HOST(BCH) = len >> 8;
  SYNCDELAY;
  EP_CDC_DEV2HOST(BCL) = len;
}


// void cdc_poll_loopback() {
// 
//   // receive CDC-ACM data
//   if(!(EP_CDC_HOST2DEV(CS) & _EMPTY)) {
//     uint16_t length = (EP_CDC_HOST2DEV(BCH) << 8) | EP_CDC_HOST2DEV(BCL);
// 
//     // if scratch buffer is full, ignore subsequent data
//     if (scratch_buf_len < ARRAYSIZE(scratch)) {
//       // store in data up to available scratch length, ignore rest
//       if (length + scratch_buf_len > ARRAYSIZE(scratch)) {
//         length = ARRAYSIZE(scratch) - scratch_buf_len;
//       }
// 
//       // length bytes from EP buf to scratch+scratch_buf_len
//       xmemcpy(scratch + scratch_buf_len, EP_CDC_HOST2DEV(FIFOBUF), length);
//       scratch_buf_len += length;
//     }
// 
//     // signalize we are ready for new data
//     EP_CDC_HOST2DEV(BCL) = 0;
//   }
// 
//   // send data to EP if it is not full
//   if (scratch_buf_len != 0 && !(EP_CDC_DEV2HOST(CS) & _FULL)) {
//     permute_data(scratch, scratch_buf_len);
// 
//     xmemcpy(EP_CDC_DEV2HOST(FIFOBUF), scratch, scratch_buf_len);
//     EP_CDC_DEV2HOST(BCH) = scratch_buf_len >> 8;
//     SYNCDELAY;
//     EP_CDC_DEV2HOST(BCL) = scratch_buf_len;
// 
//     // // simultaneously send the data over uart
//     // {
//     //   uint16_t i;
//     //   for (i = 0; i < scratch_buf_len; ++i) {
//     //     __critical {
//     //       bitbang_uart_send_byte(scratch[i]);
//     //     }
//     //   }
//     // }
// 
//     scratch_buf_len = 0;
//   }
// 
// }

void cdc_poll() {
  static uint32_t counter = 0;
  static uint8_t state_tick = 0;
  
  if (counter++ > 10000UL * 8) {
    counter = 0;
    
    switch (state_tick) {
      case 0:
        cdc_print("tick...\r\n");
        break;
      case 1:
        cdc_print("sending byte...\r\n");
        uart_queue_send('N');

        {
          int len = 0;
          int s1 = uart.tx.state, s2 = uart.tx.state;
          len += sprintf(&EP_CDC_DEV2HOST(FIFOBUF)[len], "tx.d=0x%02x, rx.d=0x%02x, rx=0x%02x, PB0=%d, PA0=%d\r\n",
                            uart.tx.data, uart.rx.data, uart.last_received, PB0, PA0);
          len += sprintf(&EP_CDC_DEV2HOST(FIFOBUF)[len], "rxs=%d, txs=%d, tic=%ld\r\n",
                            s1, s2, uart.timer_interrupt_count);
          EP_CDC_DEV2HOST(BCH) = (len & 0xff00) >> 8;
          SYNCDELAY;
          EP_CDC_DEV2HOST(BCL) = len & 0xff;
        }
        break;
      case 2:
        
        {
          int len = 0;
          len += sprintf(&EP_CDC_DEV2HOST(FIFOBUF)[len], "tx.d=0x%02x, rx.d=0x%02x, rx=0x%02x, PB0=%d, PA0=%d\r\n",
                            uart.tx.data, uart.rx.data, uart.last_received, PB0, PA0);
          EP_CDC_DEV2HOST(BCH) = (len & 0xff00) >> 8;
          SYNCDELAY;
          EP_CDC_DEV2HOST(BCL) = len & 0xff;
        }
        {
          int len = 0;
          // len += sprintf(&EP_CDC_DEV2HOST(FIFOBUF)[len], "rx=%d, ovf=%d, rxb=0x%02x\r\n",
          //                   uart.received_flag, uart.overflow_flag, uart.last_received);
          // len += sprintf(&EP_CDC_DEV2HOST(FIFOBUF)[len], "tx.d=0x%02x, rx.d=0x%02x\r\n",
          //                   uart.tx.data, uart.rx.data);
          len += sprintf(&EP_CDC_DEV2HOST(FIFOBUF)[len], "rxs=%d, txs=%d, tic=%ld, states=0x%02x\r\n",
                            uart.rx.state, uart.tx.state, uart.timer_interrupt_count, uart.rx.states_reached);
          
          EP_CDC_DEV2HOST(BCH) = (len & 0xff00) >> 8;
          SYNCDELAY;
          EP_CDC_DEV2HOST(BCL) = len & 0xff;
        }

        if (uart.received_flag) {
          uart.received_flag = false;
        }
        break;
      default:
        state_tick = 0;
        break;
    }
    state_tick++;
    if (state_tick > 2)
      state_tick = 0;

  }


  // cdc_poll_loopback();
  // or:
  // we need to receive, even when we don't care
  if(!(EP_CDC_HOST2DEV(CS) & _EMPTY)) {
    // signalize we are ready for new data
    EP_CDC_HOST2DEV(BCL) = 0;
  }

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

void init_uart_ext_int() {
  // configure PA0 as alternate function INT0
  PORTACFG |= 1;

  // configure external interrupt on INT0 for uart rx, negative edge (start bit)
  IT0 = 1;
  IP |= 1; // high priority
  IE |= 1; // enable interrupt
}
#endif
