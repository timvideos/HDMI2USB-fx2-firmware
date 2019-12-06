#include "dna.h"

#include <fx2regs.h>
#include <fx2delay.h>
#include <fx2usb.h>
#include "usb_config.h"
#include "uart.h"

static char byte2hex(uint8_t byte);
static void copy_dna_to_serial_number(uint8_t *from_buf);

// usb strings defined with descriptors
extern usb_ascii_string_c usb_strings[];

bool try_read_fpga_dna_uart(uint16_t max_wait_ms) {
  uint8_t dna_buf[DNA_LENGTH];
  uint8_t dna_buf_i = 0;

  while (max_wait_ms > 0) {
    bool dna_started = false;
    uint8_t byte;

    // pop data until we find start byte
    if (!dna_started && uart_pop(&byte) && byte == DNA_START_CODE) {
      dna_started = true;
    }

    // now we are copying DNA to the buffer
    if (dna_started) {
      // read any missing data
      while (dna_buf_i < DNA_LENGTH && uart_pop(&byte)) {
        dna_buf[dna_buf_i++] = byte;
      }
      // if we have whole DNA, then set serial number and exit
      if (dna_buf_i == DNA_LENGTH) {
        copy_dna_to_serial_number(dna_buf);
        return true;
      }
    }

    // use simple inaccurate wait on delays as we don't really need too much precision
    delay_ms(DNA_WAIT_TIMEOUT_PRECISION_MS);
    // decrease remaining time, avoid overflow through zero
    if (max_wait_ms < DNA_WAIT_TIMEOUT_PRECISION_MS)
      max_wait_ms = 0;
    else
      max_wait_ms -= DNA_WAIT_TIMEOUT_PRECISION_MS;
  }

  return false; // DNA not sent
}

bool try_read_fpga_dna_ep(uint16_t max_wait_ms) {
  uint16_t dna_start_index = 0;  // to be able to find DNA packets in middle of buffer
  uint16_t ep_data_len;

  while (max_wait_ms > 0) {
    do {  // search for DNA
      ep_data_len = (EP_CDC_DEV2HOST(BCH) << 8) | EP_CDC_DEV2HOST(BCL);

      // if the first byte is not the start code, check next byte in next iteration
      if (EP_CDC_DEV2HOST(FIFOBUF)[dna_start_index] != DNA_START_CODE) {
        dna_start_index++;
      } else { // consider remaining data as DNA starting from next byte
        copy_dna_to_serial_number(EP_CDC_DEV2HOST(FIFOBUF) + dna_start_index + 1);
        return true;
      }
    // interate as long as there is space for a potential DNA packet (packet = start byte + DNA)
    } while (ep_data_len >= dna_start_index + 1 + DNA_LENGTH);

    // use simple inaccurate wait on delays as we don't really need too much precision
    delay_ms(DNA_WAIT_TIMEOUT_PRECISION_MS);
    // decrease remaining time, avoid overflow through zero
    if (max_wait_ms < DNA_WAIT_TIMEOUT_PRECISION_MS)
      max_wait_ms = 0;
    else
      max_wait_ms -= DNA_WAIT_TIMEOUT_PRECISION_MS;
  }

  return false; // DNA not sent
}

void copy_dna_to_serial_number(uint8_t *from_buf) {
  // save DNA to usb serial number string
  // on FX2 we can cast away __code const qualifier (see TRM 5.3)
  __xdata char *usb_serial_number = (__xdata char *) usb_strings[USB_STR_SERIAL_NUMBER - 1];

  uint8_t i;
  for (i = 0; i < DNA_LENGTH; ++i) {
    // convert each DNA byte to hex string (1 byte = 2 chars)
    char c_low = byte2hex(from_buf[i] & 0x0f);
    char c_high = byte2hex((from_buf[i] & 0xf0) >> 4);

    // save in reverse order - LSB goes as last element of the string
    usb_serial_number[DNA_USB_SERIAL_NUMBER_LENGTH - 1 - 2*i] = c_low;
    usb_serial_number[DNA_USB_SERIAL_NUMBER_LENGTH - 1 - (2*i + 1)] = c_high;
  }
}

// byte must be 4-bit value!
char byte2hex(uint8_t byte) {
  char c;
  if (byte <= 9) {
    c = byte + '0';
  } else {
    c = byte + 'a';
  }
  return c;
}
