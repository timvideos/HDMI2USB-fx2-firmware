#include "uac.h"

extern usb_descriptor_set_c usb_descriptor_set;

// keeps track of current alternate setting of streaming interface
uint8_t uac_as_alt_setting = 0;

bool uac_handle_usb_set_interface(uint8_t interface, uint8_t alt_setting) {
  if (interface == USB_CFG_IF_UAC_AUDIO_CONTROL && alt_setting == 0) {
    usb_reset_data_toggles(&usb_descriptor_set, interface, alt_setting);
    return true;
  }
  if (interface == USB_CFG_IF_UAC_AUDIO_STREAMING && (alt_setting == 0 || alt_setting == 1)) {
    // I belive we do not need to reset our endpoint configuration regsiters, as host should know 
    // that in alt_setting 0 there are no endpoints associated with this interface, so host will 
    // not send any IN requests
    uac_as_alt_setting = alt_setting;
    usb_reset_data_toggles(&usb_descriptor_set, interface, alt_setting);
    return true;
  }
  return false; // not handled
}

bool uac_handle_usb_get_interface(uint8_t interface) {
  if (interface == USB_CFG_IF_UAC_AUDIO_CONTROL) {
    EP0BUF[0] = 0; // only 1 alternate setting
    SETUP_EP0_BUF(1);
  }
  if (interface == USB_CFG_IF_UAC_AUDIO_CONTROL) {
    EP0BUF[0] = uac_as_alt_setting;
    SETUP_EP0_BUF(1);
  }
  return false; // not handled
}
