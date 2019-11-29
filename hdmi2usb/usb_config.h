#ifndef USB_CONFIG_H
#define USB_CONFIG_H

/*** USB interface numbers configuration **************************************/

enum USBConfigInterface {
  USB_CFG_IF_CDC_COMMUNICATION = 0,
  USB_CFG_IF_CDC_DATA,
  USB_CFG_IF_UVC_VIDEO_CONTROL, 
  USB_CFG_IF_UVC_VIDEO_STREAMING,
  USB_CFG_IF_UAC_AUDIO_CONTROL,
  USB_CFG_IF_UAC_AUDIO_STREAMING,
  // number of enum elements
  USBConfigInterface_COUNT
};

/*** USB endpoints configuration **********************************************/

// Configuration of large endpoints (2, 4, 6, 8).
// EP1 and EP0 are different than those so there is no sense to define them this way.
// This approach still has some drawbacks, as these endpoints still have some differences,
// e.g. only EP2 and EP6 can have size of 1024 bytes. Even so it may be easier to define
// endpoints configuration in one place.

// endpoint numbers
#define USB_CFG_EP_CDC_COMMUNICATION   1 // communication interface, using small endpoint
#define USB_CFG_EP_CDC_HOST2DEV        2 // data interface OUT
#define USB_CFG_EP_UAC                 4
#define USB_CFG_EP_UVC                 6
#define USB_CFG_EP_CDC_DEV2HOST        8 // data interface IN

// helpers for constructing register names
// https://stackoverflow.com/a/1489985
#define MACRO_PASTER(x, y)    x ## y
#define MACRO_EVALUATOR(x, y) MACRO_PASTER(x, y)

// macros for endpoint registers access
// looks scary, but the usage is:
//   `EP_CDC_HOST2DEV(CFG)` -> `EP2CFG`
#define EP_CDC_HOST2DEV(tail) MACRO_EVALUATOR(MACRO_EVALUATOR(EP, USB_CFG_EP_CDC_HOST2DEV), tail)
#define EP_CDC_DEV2HOST(tail) MACRO_EVALUATOR(MACRO_EVALUATOR(EP, USB_CFG_EP_CDC_DEV2HOST), tail)
#define EP_UVC(tail)          MACRO_EVALUATOR(MACRO_EVALUATOR(EP, USB_CFG_EP_UVC), tail)
#define EP_UAC(tail)          MACRO_EVALUATOR(MACRO_EVALUATOR(EP, USB_CFG_EP_UAC), tail)

#endif /* USB_CONFIG_H */
