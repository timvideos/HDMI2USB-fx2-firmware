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

#define UART_TX_QUEUE_SIZE 50
#define UART_RX_QUEUE_SIZE 50

enum UARTMode {
  UART_MODE_TX,
  UART_MODE_RX,
  UART_MODE_RX_TX,
};

/**
 * Configures UART for given baudrate and starts it.
 * UART can either work in full duplex mode, or only one direction
 * can be enabled, in which case the second one is not configured
 * and the pin can be used for other purpose.
 * In either mode, Timer 2 is used for baudrate generation.
 * If RX is enabled, INT0 is used for clock synchronization.
 */
void uart_init(uint32_t baudrate, enum UARTMode mode);

/**
 * Send byte using bitbang UART. Returns false if TX queue is full,
 * or if TX mode is not enabled.
 */
bool uart_push(uint8_t byte);

/**
 * Get byte from UART RX queue. Returns false if RX queue is empty,
 * or if RX mode is not enabled.
 */
bool uart_pop(uint8_t *byte);


#endif /* UART_H */
