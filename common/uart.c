#include "uart.h"

// State of byte transmission
enum UARTState {
  IDLE = 0,
  START_BIT,
  DATA,
  END_BIT
};

// Sampling RX data is done in between changes to TX line
enum UARTClkPhase {
  CLK_TX,
  CLK_RX
};

// State required for single direction of UART
struct UARTStateMachine {
  enum UARTState state;
  uint8_t data;
  uint8_t bit_n;
};

// State of the whole bitbang UART
struct BitbangUART {
  enum UARTMode mode;
  struct UARTStateMachine tx;
  struct UARTStateMachine rx;
  enum UARTClkPhase clk_phase;
  uint8_t overflow_flag;
  uint8_t idle_counter;
};

static void set_tim2_frequency(uint32_t baud_rate);
static void uart_set_idle();
static void uart_set_running();


// state and queue buffers
__xdata volatile struct BitbangUART uart;
DEFINE_QUEUE(uart_tx_queue, __xdata volatile uint8_t, UART_TX_QUEUE_SIZE)
DEFINE_QUEUE(uart_rx_queue, __xdata volatile uint8_t, UART_RX_QUEUE_SIZE)


void uart_init(uint32_t baudrate, enum UARTMode mode) {
  xmemclr(&uart, sizeof(uart));
  uart.mode = mode;

  if (mode != UART_MODE_RX) { // configure TX pin as output
    // FIXME: remove hardcoded pin numbers
    PB0 = 1; // uart is initially high
    OEB |= (1 << 0);
  }

  if (mode != UART_MODE_TX) {
    // FIXME: remove hardcoded pin numbers
    // configure PA0 as alternate function INT0
    PORTACFG |= 1;
    // configure external interrupt on INT0 for uart rx, negative edge (start bit)
    IT0 = 1;
    IP |= 1; // high priority
  }

  // configure interrupt timer
  TR2 = 0; // stop timer 2
  set_tim2_frequency(baudrate * 2); // we need 2 times faster clock than the actual baudrate
  T2CON = 0x00; // configure timer 2 in default auto-reload mode
  ET2 = 1; // enable timer 2 interrupt
  PT2 = 1; // set high priority

  // enable external interrupt if RX is enabled
  uart_set_idle();
}

bool uart_push(uint8_t byte) {
  if (uart.mode == UART_MODE_RX)
    return false;

  if (QUEUE_FULL(uart_tx_queue))
    return false;

  QUEUE_PUT(uart_tx_queue, byte);
  uart.tx.state = START_BIT;

  // if timer is not running, than we are in idle state so we have to start timer now
  if (!TR2) {
    uart_set_running();
  }

  return true;
}

bool uart_pop(uint8_t *byte) {
  if (uart.mode == UART_MODE_TX)
    return false;

  if (QUEUE_EMPTY(uart_rx_queue))
    return false;

  QUEUE_GET(uart_rx_queue, *byte);
  return true;
}

// timer 2 interrupt for software uart
void isr_TF2() __interrupt(_INT_TF2) {
  TF2 = 0; // clear timer overflow flag

  // transmit in first clock tick, receive in the second one
  // TODO: make clock two times slower when not in full-duplex mode
  if (uart.clk_phase == CLK_TX) {
    uart.clk_phase = CLK_RX;

    if (uart.mode != UART_MODE_RX) {
      // transmit state machine
      switch (uart.tx.state) {
        case IDLE:
          if (QUEUE_EMPTY(uart_tx_queue)) {
            break;
          }
          // no break if there is something else in the queue!
          // jump to START_BIT directly
          // uart.tx.state = START_BIT;
        case START_BIT:
          UART_TX_PIN = 0;
          uart.tx.bit_n = 0;
          uart.tx.state = DATA;
          // load next byte from queue
          QUEUE_GET(uart_tx_queue, uart.tx.data);
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
    }
  } else { // receive phase
    uart.clk_phase = CLK_TX;

    if (uart.mode != UART_MODE_TX) {
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
            if (QUEUE_FULL(uart_rx_queue)) {
              uart.overflow_flag = 1;
            } else {
              QUEUE_PUT(uart_rx_queue, uart.rx.data);
            }
          }
          break;
        default:
          break;
      }
    }
  }

  // check if we are in idle state
  // we want to enable external interrupt synchronization only if we are not sending nor receiving
  // this external interrupt will be disabled in the handler when it occurs (and will reenable this interrupt)
  if (uart.tx.state == IDLE && uart.rx.state == IDLE) {
    uart.idle_counter++;
    if (uart.idle_counter > UART_IDLE_TICKS) {
      uart.idle_counter = 0;
      uart_set_idle();
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
  uart_set_running();
}


// disables the timer and moves uart state machine into standby
// in RX mode we enable external interrupt, TX is started with uart_set_running
void uart_set_idle() {
  TR2 = 0; // disable timer
  if (uart.mode != UART_MODE_TX) {
    IE |= 1; // enable external interrupt
  }
}

// synchronizes uart timer at given moment at the TX phase
void uart_set_running() {
  if (uart.mode != UART_MODE_TX) { // if RX is enabled
    // first, disable external interrupt
    IE &= ~1;
  }
  // set correct phase
  uart.clk_phase = CLK_TX;
  // set timer counter to max value, so that it fires in next tick
  // FIXME: we could set it to reload value and force interrupt
  TH2 = 0xff;
  TL2 = 0xff;
  // start the timer
  TR2 = 1;
}


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
