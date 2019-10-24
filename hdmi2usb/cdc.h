
#ifndef CDC_H
#define CDC_H

#include <stdbool.h>
#include <libfx2/firmware/library/include/fx2delay.h>
#include <libfx2/firmware/library/include/fx2regs.h>
#include <libfx2/firmware/library/include/usbcdc.h>

#include "cdc-config.h"

void cdcuser_receive_data(uint8_t* data, uint16_t length);
bool cdcuser_set_line_rate(uint32_t baud_rate);

// Handles the "vendor commands" for a CDC device.
bool cdc_handle_command(uint8_t cmd);

//void cdc_setup();

// Send the CDC interrupt poll thingy.
void cdc_receive_poll();

// You are able to send data.
//bool cdc_can_send();
#define cdc_can_send() \
  !(EP2468STAT & bmCDC_D2H_EP(F))

extern volatile uint16_t cdc_queued_bytes;
// Queue a byte in the output CDC data queue.
//void cdc_queue_data(uint8_t data);
#define cdc_queue_data(data) \
  CDC_D2H_EP(FIFOBUF)        \
  [cdc_queued_bytes++] = data;
// Send all queue bytes from the output CDC data queue to the host.
//void cdc_send_queued_data();
#define cdc_send_queued_data()             \
  CDC_D2H_EP(BCH) = MSB(cdc_queued_bytes); \
  SYNCDELAY;                               \
  CDC_D2H_EP(BCL) = LSB(cdc_queued_bytes); \
  SYNCDELAY;                               \
  cdc_queued_bytes = 0;

#endif  // CDC_H
