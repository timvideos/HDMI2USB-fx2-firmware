#ifndef CDC_H
#define CDC_H

#include <fx2usb.h>
#include <usbcdc.h>

#include "usb_defs.h"

// CDC-ACM descritors
extern usb_desc_if_assoc_c              usb_cdc_if_assoc;
extern usb_cdc_desc_functional_header_c usb_cdc_func_cic_header;
extern usb_cdc_desc_functional_acm_c    usb_cdc_func_cic_acm;
extern usb_cdc_desc_functional_union_c  usb_cdc_func_cic_union;
extern usb_desc_interface_c             usb_cdc_if_cic;
extern usb_desc_interface_c             usb_cdc_if_dic;
extern usb_desc_endpoint_c              usb_cdc_ep_comm;
extern usb_desc_endpoint_c              usb_cdc_ep_data_out;
extern usb_desc_endpoint_c              usb_cdc_ep_data_in;

// Marco for adding descriptors to usb_configuration_c
#define CDC_DESCRIPTORS_LIST \
    { .generic   = (usb_desc_generic_c *) &usb_cdc_if_assoc         }, \
    { .interface =                        &usb_cdc_if_cic           }, \
    { .generic   = (usb_desc_generic_c *) &usb_cdc_func_cic_header  }, \
    { .generic   = (usb_desc_generic_c *) &usb_cdc_func_cic_acm     }, \
    { .generic   = (usb_desc_generic_c *) &usb_cdc_func_cic_union   }, \
    { .endpoint  =                        &usb_cdc_ep_comm          }, \
    { .interface =                        &usb_cdc_if_dic           }, \
    { .endpoint  =                        &usb_cdc_ep_data_out      }, \
    { .endpoint  =                        &usb_cdc_ep_data_in       },

// This is for debug purpose only. It is generally unsafe to use if anything else
// uses that endpoint. The endpoint is assumed to be configured properly.
#ifdef DEBUG
#include <stdio.h>
#define cdc_printf(...)                                         \
    do {                                                        \
      int len = sprintf(EP_CDC_DEV2HOST(FIFOBUF), __VA_ARGS__); \
      EP_CDC_DEV2HOST(BCH) = (len & 0xff00) >> 8;               \
      SYNCDELAY;                                                \
      EP_CDC_DEV2HOST(BCL) = len & 0xff;                        \
    } while (0);
#else
#define cdc_printf(...)
#endif

/**
 * Used to configure descriptors to avoid conflits with the rest of application
 * configuration.
 * NOTE: CDC data interface always has number of communication interface + 1!
 */
struct cdc_configuration {
  uint8_t if_num_comm; // data interface number = if_num_comm + 1
  uint8_t ep_addr_comm;
  uint8_t ep_addr_data_host2dev;
  uint8_t ep_addr_data_dev2host;
};

/**
 * Modifies descriptors to use requested configuration
 */
void cdc_config(struct cdc_configuration *config);

/**
 * Handle CDC-specific USB setup requests. Return true if request has been handled.
 */
bool cdc_handle_usb_setup(__xdata struct usb_req_setup *req);

#endif /* CDC_H */
