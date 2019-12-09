#include <fx2lib.h>
#include <fx2usb.h>

#include "uac.h"

usb_ascii_string_c usb_strings[] = {
  "TimVideos.us",
  "USB Audio Class example",
  "Left",
  "Right",
};

usb_desc_device_c usb_device = {
  .bLength              = sizeof(struct usb_desc_device),
  .bDescriptorType      = USB_DESC_DEVICE,
  .bcdUSB               = 0x0200,
  .bDeviceClass         = USB_DEV_CLASS_MISCELLANEOUS,
  .bDeviceSubClass      = USB_DEV_SUBCLASS_COMMON,
  .bDeviceProtocol      = USB_DEV_PROTOCOL_INTERFACE_ASSOCIATION_DESCRIPTOR,
  .bMaxPacketSize0      = 64,
  .idVendor             = VID,  // VID, PID, DID defined by compiler flags depending on BOARD
  .idProduct            = PID,
  .bcdDevice            = DID,
  .iManufacturer        = 1,
  .iProduct             = 2,
  .iSerialNumber        = 0,
  .bNumConfigurations   = 1,
};

usb_configuration_c usb_config = {
  {
    .bLength              = sizeof(struct usb_desc_configuration),
    .bDescriptorType      = USB_DESC_CONFIGURATION,
    .bNumInterfaces       = UAC_NUM_INTERFACES,
    .bConfigurationValue  = 1,
    .iConfiguration       = 0,
    .bmAttributes         = USB_ATTR_RESERVED_1,
    .bMaxPower            = 250, // 500mA
  },
  {
    UAC_DESCRIPTORS_LIST
    { 0 }
  }
};

usb_configuration_set_c usb_configs[] = {
  &usb_config,
};

usb_descriptor_set_c usb_descriptor_set = {
  .device           = &usb_device,
  .config_count     = ARRAYSIZE(usb_configs),
  .configs          = usb_configs,
  .string_count     = ARRAYSIZE(usb_strings),
  .strings          = usb_strings,
};
