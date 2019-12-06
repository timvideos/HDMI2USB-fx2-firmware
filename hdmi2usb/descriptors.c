#include <fx2lib.h>
#include <fx2usb.h>
#include <usbcdc.h>

#include "usb_config.h"
#include "cdc.h"
#include "uvc.h"
#include "uac.h"

usb_ascii_string_c usb_strings[] = {
  [USB_STR_MANUFACTURER   - 1] = "TimVideos.us",
  [USB_STR_PRODUCT        - 1] = "HDMI2USB.tv - Numato Opsis Board",
  [USB_STR_SERIAL_NUMBER  - 1] = "0123456789abcdef", // must have length 16 to set it to FPGA DNA
  [USB_STR_CHANNEL_NAME_1 - 1] = "Left",
  [USB_STR_CHANNEL_NAME_2 - 1] = "Right",
};

usb_desc_device_c usb_device = {
  .bLength              = sizeof(struct usb_desc_device),
  .bDescriptorType      = USB_DESC_DEVICE,
  .bcdUSB               = 0x0200,
  .bDeviceClass         = USB_DEV_CLASS_MISCELLANEOUS,
  .bDeviceSubClass      = USB_DEV_SUBCLASS_COMMON,
  .bDeviceProtocol      = USB_DEV_PROTOCOL_INTERFACE_ASSOCIATION_DESCRIPTOR,
  .bMaxPacketSize0      = 64,
  .idVendor             = VID,  // VID, PID, DID must be defined as compiler flags
  .idProduct            = PID,
  .bcdDevice            = DID,
  .iManufacturer        = USB_STR_MANUFACTURER,
  .iProduct             = USB_STR_PRODUCT,
  .iSerialNumber        = USB_STR_SERIAL_NUMBER,
  .bNumConfigurations   = 1,
};

usb_configuration_c usb_config = {
  {
    .bLength              = sizeof(struct usb_desc_configuration),
    .bDescriptorType      = USB_DESC_CONFIGURATION,
    .bNumInterfaces       = USBConfigInterface_COUNT,
    .bConfigurationValue  = 1,
    .iConfiguration       = 0,
    .bmAttributes         = USB_ATTR_RESERVED_1,
    .bMaxPower            = 250, // 500mA
  },
  {
    UVC_DESCRIPTORS_LIST
    CDC_DESCRIPTORS_LIST
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

