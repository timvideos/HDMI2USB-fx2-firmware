#include <fx2lib.h>
#include <fx2usb.h>
#include <usbcdc.h>

usb_ascii_string_c usb_strings[] = {
  [0] = "TimVideos.us",  // manufacturer
  [1] = "HDMI2USB.tv - Numato Opsis Board",  // product
  [2] = "0123456789abcdef",  // serial number
};

usb_desc_device_c usb_device = {
  .bLength              = sizeof(struct usb_desc_device),
  .bDescriptorType      = USB_DESC_DEVICE,
  .bcdUSB               = 0x0200,
  // It would make more sense for this to be USB_DEV_CLASS_PER_INTERFACE, such that the device
  // could be a composite device and include non-CDC interfaces. However, this does not work under
  // Windows; it enumerates a broken unknown device and a broken serial port instead. It is likely
  // that the following Microsoft document describes a way to make it work, but I have not verified
  // it: https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-common-class-generic-parent-driver
  .bDeviceClass         = USB_DEV_CLASS_PER_INTERFACE,
  .bDeviceSubClass      = USB_DEV_SUBCLASS_PER_INTERFACE,
  .bDeviceProtocol      = USB_DEV_PROTOCOL_PER_INTERFACE,
  .bMaxPacketSize0      = 64,
  .idVendor             = VID,  // VID, PID, DID must be defined as compiler flags
  .idProduct            = PID,
  .bcdDevice            = DID,
  .iManufacturer        = 1,
  .iProduct             = 2,
  .iSerialNumber        = 3,
  .bNumConfigurations   = 1,
};

usb_desc_interface_c usb_iface_cic = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 0,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_IFACE_CLASS_CIC,
  .bInterfaceSubClass   = USB_IFACE_SUBCLASS_CDC_CIC_ACM,
  .bInterfaceProtocol   = USB_IFACE_PROTOCOL_CDC_CIC_NONE,
  .iInterface           = 0,
};

usb_desc_endpoint_c usb_endpoint_ep1_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 1|USB_DIR_IN,
  .bmAttributes         = USB_XFER_INTERRUPT,
  .wMaxPacketSize       = 8,
  .bInterval            = 10,
};

usb_cdc_desc_functional_header_c usb_func_cic_header = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_header),
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_HEADER,
  .bcdCDC               = 0x0120,
};

usb_cdc_desc_functional_acm_c usb_func_cic_acm = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_acm),
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_ACM,
  .bmCapabilities       = 0,
};

usb_cdc_desc_functional_union_c usb_func_cic_union = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_union) +
                          sizeof(uint8_t) * 1,
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_UNION,
  .bControlInterface    = 0,
  .bSubordinateInterface = { 1 },
};

usb_desc_interface_c usb_iface_dic = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 1,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 2,
  .bInterfaceClass      = USB_IFACE_CLASS_DIC,
  .bInterfaceSubClass   = USB_IFACE_SUBCLASS_CDC_DIC,
  .bInterfaceProtocol   = USB_IFACE_PROTOCOL_CDC_DIC_NONE,
  .iInterface           = 0,
};

usb_desc_endpoint_c usb_endpoint_ep2_out = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 2,
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};

usb_desc_endpoint_c usb_endpoint_ep6_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 6|USB_DIR_IN,
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};

usb_configuration_c usb_config = {
  {
    .bLength              = sizeof(struct usb_desc_configuration),
    .bDescriptorType      = USB_DESC_CONFIGURATION,
    .bNumInterfaces       = 2,
    .bConfigurationValue  = 1,
    .iConfiguration       = 0,
    .bmAttributes         = USB_ATTR_RESERVED_1,
    .bMaxPower            = 250,
  },
  {
    { .interface = &usb_iface_cic },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_header },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_acm },
    { .generic   = (struct usb_desc_generic *) &usb_func_cic_union },
    { .endpoint  = &usb_endpoint_ep1_in },
    { .interface = &usb_iface_dic },
    { .endpoint  = &usb_endpoint_ep2_out },
    { .endpoint  = &usb_endpoint_ep6_in },
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

