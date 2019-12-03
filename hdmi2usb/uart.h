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


/**
 * Configures UART for given baudrate and starts listening for incoming data
 */
void uart_init(uint32_t baudrate);

/**
 * Send byte using bitbang UART. Returns false if TX queue is full.
 */
bool uart_push(uint8_t byte);

/**
 * Get byte from UART RX queue. Returns false if RX queue is empty.
 */
bool uart_pop(uint8_t *byte);


#endif /* UART_H */
