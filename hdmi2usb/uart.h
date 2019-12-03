#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <fx2lib.h>
#include <fx2ints.h>
#include <fx2regs.h>
#include <fx2queue.h>

#define UART_TX_PIN PB0
#define UART_RX_PIN PA0

// UART state machine is clocked by a timer at twice the baudrate,
// when not sending nor receiving for that many ticks, the timer gets
// disabled and is restarted on RX external interrupt (or on TX)
#define UART_IDLE_TICKS 6

enum UARTState {
  IDLE = 0,
  START_BIT,
  DATA,
  END_BIT
};

enum UARTClkPhase {
  CLK_TX,
  CLK_RX
};

struct UARTStateMachine {
  enum UARTState state;
  uint8_t data;
  uint8_t bit_n;
};

struct BitbangUART {
  struct UARTStateMachine tx;
  struct UARTStateMachine rx;
  enum UARTClkPhase clk_phase;
  uint8_t rx_buf;
  bool received_flag;
  bool overflow_flag;
  uint8_t idle_counter;
};

// structure used by the interrupt for bitbang uart logic
extern __xdata volatile struct BitbangUART uart;

void uart_init(uint32_t baudrate);
void uart_start();
void uart_send(uint8_t byte);


#endif /* UART_H */
