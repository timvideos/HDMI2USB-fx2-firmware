#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <fx2lib.h>
#include <fx2ints.h>
#include <fx2regs.h>
#include <fx2queue.h>

#if defined(UART_SOFTWARE_BITBANG) && UART_SOFTWARE_BITBANG == 1
// TODO: now PB0 and PA0 are still hard-coded in parts of implementation
// this is the most problematic with RX as we need external interrupt
#  if !defined(UART_TX_PIN)
#    error "UART_TX_PIN must be defined (e.g. as PB0)"
#  endif
#  if !defined(UART_RX_PIN)
#    error "UART_RX_PIN must be defined (e.g. as PA0)"
#  endif
#endif

// Data pushed/poped is queued in TX/RX queues of given size
#if !defined(UART_TX_QUEUE_SIZE)
#  error "UART_TX_QUEUE_SIZE must be defined"
#endif
#if !defined(UART_RX_QUEUE_SIZE)
#  error "UART_RX_QUEUE_SIZE must be defined"
#endif

// Bitbang UART state machine is clocked by a timer at twice the baudrate,
// when not sending nor receiving for that many ticks, the timer gets
// disabled and is restarted on RX external interrupt (or on TX)
#define UART_IDLE_TICKS 6

// Mode, only relevant when UART_SOFTWARE_BITBANG == 1
enum UARTMode {
  UART_MODE_TX,
  UART_MODE_RX,
  UART_MODE_RX_TX,
};

/**
 * Configures UART for given baudrate and starts it.
 *
 * When using hardware UART, 'mode' argument is not used.
 * UART is configured in 8N1 full-duplex mode.
 *
 * When using software bitbanged UART, it can either work in full
 * duplex mode, or only one direction can be enabled, in which case
 * the second one is not configured and the pin can be used for
 * other purpose.
 *
 * In either mode, Timer 2 is used for baudrate generation.
 * If RX is enabled in bitbanged mode, INT0 is used for clock
 * synchronization.
 */
void uart_init(uint32_t baudrate, enum UARTMode mode);

/**
 * Send byte using bitbang UART. Returns false if TX queue is full,
 * or if TX mode is not enabled (bitbang only).
 */
bool uart_push(uint8_t byte);

/**
 * Get byte from UART RX queue. Returns false if RX queue is empty,
 * or if RX mode is not enabled (bitbang only).
 */
bool uart_pop(uint8_t *byte);


#endif /* UART_H */
