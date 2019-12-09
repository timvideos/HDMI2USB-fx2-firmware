#ifndef USB_DEFS_H
#define USB_DEFS_H

#include <fx2lib.h>
#include <fx2usb.h>

// Macro to easily define typedefs for descriptor structures
#define USB_DESC_CONST_CODE_TYPEDEF(desc) \
    typedef __code const struct desc \
    desc ## _c;

enum {
  // Interface association descriptor, requires proper device descriptor
  // see: USB Interface Association Descriptor Device Class Code and Use Model, 1.0
  //      https://www.usb.org/sites/default/files/iadclasscode_r10.pdf
  USB_DEV_CLASS_MISCELLANEOUS = 0xef,
  USB_DEV_SUBCLASS_COMMON = 0x02,
  USB_DEV_PROTOCOL_INTERFACE_ASSOCIATION_DESCRIPTOR = 0x01,
  USB_DESC_IF_ASSOC = 0x0b,
};

struct usb_desc_if_assoc {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bFirstInterface;
  uint8_t bInterfaceCount;
  uint8_t bFunctionClass;
  uint8_t bFunctionSubClass;
  uint8_t bFunctionProtocol;
  uint8_t iFunction;
};

USB_DESC_CONST_CODE_TYPEDEF(usb_desc_if_assoc)


#endif /* USB_DEFS_H */
