#include "uart.h"

static void set_tim2_frequency(uint32_t baud_rate);

// queue buffers
DEFINE_QUEUE(uart_tx_queue, __xdata volatile uint8_t, UART_TX_QUEUE_SIZE)
DEFINE_QUEUE(uart_rx_queue, __xdata volatile uint8_t, UART_RX_QUEUE_SIZE)

// add software or hardware implementation depending on configuration
#if defined(UART_SOFTWARE_BITBANG) && UART_SOFTWARE_BITBANG == 1
#  include "uart_soft.c"
#else
#  include "uart_hard.c"
#endif

// clock speed: 0b00 = 12MHz, 0b01 = 24MHz, 0b10 = 48MHz
#define CPUCLKSPD ((CPUCS & (_CLKSPD0 | _CLKSPD1)) >> 3)

#define MSB(word) (((word) & 0xff00) >> 8)
#define LSB(word) ((word) & 0xff)

void set_tim2_frequency(uint32_t baud_rate) {
  // both Timer 1 or 2 could be used for serial port 0, but serial port 1 can only use Timer 1
  // so we use Timer 2
  uint16_t rcap2hl;    // value of RCAP2HL before splitting into H and L
  uint8_t clk_multip;  // multiplier depeding on processor clock speed
  uint32_t n_clocks;
  uint32_t clk_out;

  // CPU clock speed multiplier:
  // 00 - 12MHz: x1
  // 01 - 24MHz: x2
  // 10 - 48MHz: x4
  // so 2^CLKSPD
  clk_multip = 1 << CPUCLKSPD;

  // use CLKOUT/4
  CKCON |= _T2M;

  clk_out = 12000000ul / 4 * clk_multip;

  // to get desired baud rate f_baud
  // f_baud = clk_out / n_clocks
  // n_clocks = clk_out / f_baud
  // because the timer counts from RCAP2HL up to 0xffff it can be calculated as:
  // RCAP2HL = 0xffff - (n_clocks - 1)
  n_clocks = clk_out / baud_rate;
  rcap2hl = 0xffff - (uint16_t) (n_clocks - 1);

  // save the baudrate
  RCAP2H = MSB(rcap2hl);
  RCAP2L = LSB(rcap2hl);
}
