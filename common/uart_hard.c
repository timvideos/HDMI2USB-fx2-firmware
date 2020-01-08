#include "uart.h"

void uart_init(uint32_t baudrate, enum UARTMode mode) {
  (void) mode;

  // use Timer 2 as baud rate generator for Serial Port 0
  RCLK = 1;
  TCLK = 1;

  // configure baud rate generator timer
  // in baud rate generator mode CLKOUT/2 is used as time base, instead of
  // CLKOUT/4 assumed by set_tim2_frequency(), so set_tim2_frequency() will
  // actually configure 2 times higher baud rate then passed as argument
  TR2 = 0; // stop timer 2
  set_tim2_frequency(baudrate / 2); // /2 to compensate for different time base

  // start the timer
  TR2 = 1;

  // setup hardware serial port in mode 1 (full-duplex 8N1)
	SM0_0 = 0;
  SM1_0 = 1;
	SM2_0 = 0; // TODO: is it that when =0, RI will fire even with invalid stop bit?
	REN_0 = 1; // enable reception

  // do not set baud rate doubler: // PCON |= _SMOD0;

  // clear interrupt flags
  TI_0 = 0;
  RI_0 = 0;

  // enable serial interrupts with low priority
  ES0 = 1;
  PS0 = 0;
}

bool uart_push(uint8_t byte) {
  if (QUEUE_FULL(uart_tx_queue))
    return false;

  QUEUE_PUT(uart_tx_queue, byte);
  return true;
}

bool uart_pop(uint8_t *byte) {
  if (QUEUE_EMPTY(uart_rx_queue))
    return false;

  QUEUE_GET(uart_rx_queue, *byte);
  return true;
}

// serial port 0 interrupt
void isr_RI_TI_0() __interrupt(_INT_RI_TI_0) {
  if (RI_0) {
    RI_0 = 0;
    // copy the byte to rx queue if we have space
    if (!QUEUE_FULL(uart_rx_queue)) {
      uint8_t byte = SBUF0;
      QUEUE_PUT(uart_rx_queue, byte);
    }
  }

  if (TI_0) {
    TI_0 = 0;
    // send next byte if there is something in the queue
    if (!QUEUE_EMPTY(uart_tx_queue)) {
      uint8_t byte;
      QUEUE_GET(uart_tx_queue, byte);
      SBUF0 = byte;
    }
  }
}
