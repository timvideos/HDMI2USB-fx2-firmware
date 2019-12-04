#ifndef DNA_H
#define DNA_H

#include <fx2lib.h>
#include <stdbool.h>

#define DNA_START_CODE 0xfe  // non-ASCII, starts DNA packet
#define DNA_WAIT_TIMEOUT_PRECISION_MS 10
#define DNA_LENGTH 8  // 64-bit value
#define DNA_USB_SERIAL_NUMBER_LENGTH (2 * DNA_LENGTH)  // required length of USB serial number string

/**
 * Tries reading CDC IN endpoint to find DNA packet sent from FPGA.
 * Timeouts after approximatelly max_wait_ms (more as simple delay is used).
 * The timeout 
 */
bool try_read_fpga_dna(uint16_t max_wait_ms);

#endif /* DNA_H */
