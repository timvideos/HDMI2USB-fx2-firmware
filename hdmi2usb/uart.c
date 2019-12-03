#include "uart.h"

static void set_tim2_frequency(uint32_t baud_rate);

__xdata volatile struct BitbangUART uart;

void uart_init(uint32_t baudrate) {
  // configure PB0 as TX, PA0 as RX // FIXME: in interrupt we use #define
  PB0 = 1; // uart is initially high
  OEB |= (1 << 0); // PB0 as output

  // configure PA0 as alternate function INT0
  PORTACFG |= 1;

  // configure external interrupt on INT0 for uart rx, negative edge (start bit)
  IT0 = 1;
  IP |= 1; // high priority

  // configure interrupt timer
  TR2 = 0; // top timer 2
  set_tim2_frequency(baudrate * 2); // we need 2 times faster clock than the actual baudrate
  T2CON = 0x00; // configure timer 2 in default auto-reload mode
  ET2 = 1; // enable timer 2 interrupt

  PT2 = 1; // set high priority
  // PT2 = 0; // set low priority
}

// disables the timer and moves uart state machine into standby
// until ext_interrupt_sync gets called (either by starting TX or on RX external interrupt)
void ext_interrupt_await() {
  TR2 = 0; // disable timer
  IE |= 1; // enable interrupt
}

// synchronizes uart timer at given moment at the TX phase
void ext_interrupt_sync() {
  // first, disable external interrupt
  IE &= ~1;
  // set correct phase
  uart.clk_phase = CLK_TX;
  // set timer counter to max value, so that it fires in next tick
  // FIXME: we could set it to reload value and force interrupt
  TH2 = 0xff;
  TL2 = 0xff;
  // start the timer
  TR2 = 1;
}


void uart_start() {
  TR2 = 1; // start timer
}

void uart_send(uint8_t byte) {
  // TODO: add actual queueing logic
  uart.tx.data = byte;
  uart.tx.bit_n = 0;
  uart.tx.state = START_BIT;

  // if timer is not running, than we are in awaiting state so we have to start timer now
  // if (!TR2) {
    ext_interrupt_sync();
  // }
}

// timer 2 interrupt for software uart
void isr_TF2() __interrupt(_INT_TF2) {
  TF2 = 0; // clear timer overflow flag

  // transmit in first clock tick, receive in the second one
  if (uart.clk_phase == CLK_TX) {
    uart.clk_phase = CLK_RX;

    // transmit state machine
    switch (uart.tx.state) {
      case START_BIT:
        UART_TX_PIN = 0;
        uart.tx.state = DATA;
        break;
      case DATA:
        UART_TX_PIN = uart.tx.data & 0x01;
        uart.tx.data >>= 1;
        uart.tx.bit_n++;
        if (uart.tx.bit_n > 7) {
          uart.tx.state = END_BIT;
        }
        break;
      case END_BIT:
        UART_TX_PIN = 1;
        uart.tx.state = IDLE;
        break;
      default:
        break;
    }
  } else { // receive phase
    uart.clk_phase = CLK_TX;

    // receive state machine
    switch (uart.rx.state) {
      case IDLE:
        // when detected low transition, go to directly to data state, as this was start bit
        if (UART_RX_PIN == 0) {
          // we are in START_BIT state, NO BREAK!
        } else {
          break; // only if not start bit
        }
      case START_BIT:
        uart.rx.data = 0;
        uart.rx.bit_n = 0;
        uart.rx.state = DATA;
        break;
      case DATA:
        uart.rx.data |= (UART_RX_PIN & 0x1) << uart.rx.bit_n;
        uart.rx.bit_n++;
        if (uart.rx.bit_n > 7) {
          uart.rx.state = END_BIT;
        }
        break;
      case END_BIT:
        uart.rx.state = IDLE;
        if (UART_RX_PIN != 0) {
          if (uart.received_flag) { // consumer didn't read byte - overflow
            uart.overflow_flag = true;
          }
          uart.rx_buf = uart.rx.data;
          uart.received_flag = true;
        }
        break;
      default:
        break;
    }
  }

  // check if we are in idle state
  // we want to enable external interrupt synchronization only if we are not sending nor receiving
  // this external interrupt will be disabled in the handler when it occurs (and will reenable this interrupt)
  if (uart.tx.state == IDLE && uart.rx.state == IDLE) {
    uart.idle_counter++;
    if (uart.idle_counter > UART_IDLE_TICKS) {
      uart.idle_counter = 0;
      ext_interrupt_await();
    }
  } else {
    uart.idle_counter = 0;
  }

}

// external interrupt on pin INT0
void isr_IE0() __interrupt(_INT_IE0) {
  // IE0 is automatically cleared by hardware

  // this gets fired when we there is a falling edge (we assume it is the START BIT)
  // we want to synchronize Timer 2 to that moment by starting it with clk_phase = TX,
  // as the falling edge is the phase when TX happens, so RX will happen 180 deg later
  ext_interrupt_sync();
}


// clockl speed: 0b00 = 12MHz, 0b01 = 24MHz, 0b10 = 48MHz
#define CPUCLKSPD ((CPUCS & (_CLKSPD0 | _CLKSPD1)) >> 3)

#define MSB(word) (((word) & 0xff00) >> 8)
#define LSB(word) ((word) & 0xff)

void set_tim2_frequency(uint32_t baud_rate) {
  // both Timer 1 or 2 could be used for serial port 0, but serial port 1 can only use Timer 1
  // so we use Timer 2
  uint16_t rcap2hl;    // value of RCAP2HL before splitting into H and L
  uint8_t clk_multip;  // multiplier depeding on processor clock speed
  uint32_t tmp;        // for better accuracy in calculations

  // CPU clock speed multiplier:
  // 00 - 12MHz: x1
  // 01 - 24MHz: x2
  // 10 - 48MHz: x4
  // so 2^CLKSPD
  clk_multip = 1 << CPUCLKSPD;

  // calculate baud rate
  // RCAP2HL = 65536 * CLKOUT / (32 * BaudRate)
  tmp = clk_multip * 375000L * 2;
  tmp /= baud_rate;
  tmp += 1;
  tmp /= 2;

  // save the baudrate
  rcap2hl = 0xFFFF - (uint16_t) tmp;
  RCAP2H= MSB(rcap2hl);
  // seems that the 24/48mhz calculations are always one less than suggested values (TRM table 14-16)
  RCAP2L= LSB(rcap2hl) + (clk_multip > 0 ? 1 : 0);
}

