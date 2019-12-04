#ifndef UAC_H
#define UAC_H

#include <fx2usb.h>
#include "usb_config.h"

bool uac_handle_usb_set_interface(uint8_t interface, uint8_t alt_setting); 
bool uac_handle_usb_get_interface(uint8_t interface);

#endif /* UAC_H */
