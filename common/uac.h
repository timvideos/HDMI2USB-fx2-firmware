#ifndef UAC_H
#define UAC_H

#include <fx2usb.h>
#include "uac_defs.h"

// control and streaming interface
#define UAC_NUM_INTERFACES 2

// UAC descriptors
extern usb_desc_interface_c                  usb_uac_std_ac_interface;
extern usb_desc_uac_input_terminal_c         usb_uac_input_terminal;
extern usb_desc_uac1_output_terminal_c       usb_uac_output_terminal;
extern usb_desc_uac1_ac_header_c             usb_uac_ac_header;
extern usb_desc_interface_c                  usb_uac_std_streaming_interface_alt0;
extern usb_desc_interface_c                  usb_uac_std_streaming_interface_alt1;
extern usb_desc_uac1_as_header_c             usb_uac_as_header;
extern usb_desc_uac_format_type_i_discrete_c usb_uac_format;
extern usb_desc_audio_endpoint_c             usb_uac_audio_endpoint;
extern usb_desc_uac_iso_endpoint_c           usb_uac_iso_endpoint;

// TODO: { .generic   = (struct usb_desc_generic *) &usb_uac_if_assoc              },
// Marco for adding descriptors to usb_configuration_c
#define UAC_DESCRIPTORS_LIST \
    { .interface =                             &usb_uac_std_ac_interface             }, \
    { .generic   = (struct usb_desc_generic *) &usb_uac_ac_header                    }, \
    { .generic   = (struct usb_desc_generic *) &usb_uac_input_terminal               }, \
    { .generic   = (struct usb_desc_generic *) &usb_uac_output_terminal              }, \
    { .interface =                             &usb_uac_std_streaming_interface_alt0 }, \
    { .interface =                             &usb_uac_std_streaming_interface_alt1 }, \
    { .generic   = (struct usb_desc_generic *) &usb_uac_as_header                    }, \
    { .generic   = (struct usb_desc_generic *) &usb_uac_format                       }, \
    { .generic   = (struct usb_desc_generic *) &usb_uac_audio_endpoint               }, \
    { .generic   = (struct usb_desc_generic *) &usb_uac_iso_endpoint                 },

struct uac_configuration {
  uint8_t if_num_ctrl;
  // if_num_streaming = if_num_ctrl + 1
  uint8_t ep_addr_streaming;
  uint8_t i_str_channel_left; // index of left channel string
  // i_str_channel_right = i_str_channel_left + 1
};

void uac_config(struct uac_configuration *config);
bool uac_handle_usb_set_interface(uint8_t interface, uint8_t alt_setting);
bool uac_handle_usb_get_interface(uint8_t interface);

#endif /* UAC_H */
