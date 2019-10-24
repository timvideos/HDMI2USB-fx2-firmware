/**
 * Copyright (C) 2009 Ubixum, Inc. 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **/
#include <stdbool.h>
#include <libfx2/firmware/library/include/fx2delay.h>
#include <libfx2/firmware/library/include/fx2regs.h>

#include "cdc-config.h"

#define LSB(word) ((uint8_t) ((word) & 0xff))
#define MSB(word) ((uint8_t) (((word) >> 8) & 0xff))
#define CPUCLKSPD ((CPUCS & (_CLKSPD0 | _CLKSPD1)) >> 3)  // clockl speed: 0b00 = 12MHz, 0b01 = 24MHz, 0b10 = 48MHz

// buffer counter for CDC transfers
extern volatile uint16_t cdc_queued_bytes;

void serial0_mode1_init(uint32_t baud_rate) __critical {
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
  
  // set Timer 2 as baud rate generator for tx and rx
  RCLK = 1;
  TCLK = 1;

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

  // start the timer
  TR2 = 1;

  // serial port mode 1 - full-duplex, asynchronious, 8-N-1
  SM0_0 = 0;
  SM1_0 = 1;
  SM2_0 = 0;  // recive flag RI0 activated only on valid stop bit
  REN_0 = 1;  // enable receiving
  PCON |= _SMOD0;  // enable baud rate doubler for serial port 0
  TI_0 = 1;  // clear transmit interrupt
  RI_0 = 0;  // clear receive interrupt
}

// extern void sio0_init( uint32_t baud_rate ) __critical;
bool cdcuser_set_line_rate(uint32_t baud_rate) {
  if (baud_rate > 115200 || baud_rate < 2400)
    baud_rate = 115200;
  serial0_mode1_init(baud_rate);
  return true;
}

void cdcuser_receive_data(uint8_t* data, uint16_t length) {
  uint16_t i;
  for (i = 0; i < length; ++i) {
    SBUF0 = data[i];
    while (TI_0)
      ;
  }
}

void uart_init() {
  cdcuser_set_line_rate(9600);

  // Used by the CDC serial port (EP2 == TX, EP4 == RX)
  SYNCDELAY; EP2CFG = _VALID | _TYPE1 | _BUF1;         // Activate, OUT Direction, BULK Type, 512  bytes Size, Double buffered
  SYNCDELAY; EP4CFG = _VALID | _DIR | _TYPE1 | _BUF1;  // Activate, IN  Direction, BULK Type, 512  bytes Size, Double buffered

  // arm ep2
  CDC_H2D_EP(BCL) = 0x80;  // write once
  SYNCDELAY;
  CDC_H2D_EP(BCL) = 0x80;  // do it again
  SYNCDELAY;

  // clear the cdc_queued_bytes
  cdc_queued_bytes = 0;

  ES0 = 1; // enable serial interrupts
  PS0 = 0; // set serial interrupts to low priority

  TI_0 = 1; // clear transmit interrupt
  RI_0 = 0; // clear receiver interrupt
}
