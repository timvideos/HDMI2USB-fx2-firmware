#include <fx2lib.h>
#include <fx2usb.h>
#include <usbcdc.h>
#include "usb_config.h"
#include "uac_defs.h"
#include "cdc.h"
#include "uvc.h"

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

/*** UAC **********************************************************************/

usb_desc_interface_c usb_uac_std_ac_interface = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = USB_CFG_IF_UAC_AUDIO_CONTROL,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOCONTROL,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

#define LENGTH_uac_input_terminal (sizeof(struct usb_desc_uac_input_terminal))
usb_desc_uac_input_terminal_c uac_input_terminal = {
  .bLength            = LENGTH_uac_input_terminal,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_INPUT_TERMINAL,
  .bTerminalID        = 1,
  .wTerminalType      = UAC_INPUT_TERMINAL_MICROPHONE,
  .bAssocTerminal     = 0,
  .bNrChannels        = 2, // stereo
  .wChannelConfig     = (UAC_CHANNEL_LEFT | UAC_CHANNEL_RIGHT),
  .iChannelNames      = USB_STR_CHANNEL_NAME_1, // first channel name, other channels must have consequetive indices
  .iTerminal          = 0,
};

#define LENGTH_uac_output_terminal (sizeof(struct usb_desc_uac1_output_terminal))
usb_desc_uac1_output_terminal_c uac_output_terminal = {
  .bLength            = LENGTH_uac_output_terminal,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_OUTPUT_TERMINAL,
  .bTerminalID        = 2,
  .wTerminalType      = UAC_OUTPUT_TERMINAL_STREAMING,
  .bAssocTerminal     = 0,
  .bSourceID          = 1, // connected to input terminal
  .iTerminal          = 0,
};

#define LENGTH_uac_ac_header (sizeof(struct usb_desc_uac1_ac_header) + 1)
usb_desc_uac1_ac_header_c uac_ac_header = {
  .bLength            = LENGTH_uac_ac_header,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_MS_HEADER,
  .bcdADC             = UAC_BCD_V10,
  .wTotalLength       = // this descriptor + all unit and terminal descriptors
      LENGTH_uac_ac_header +
      LENGTH_uac_input_terminal +
      LENGTH_uac_output_terminal,
  .bInCollection      = 1, // one interface in collection
  .baInterfaceNr      = {5}, // streaing interface 0
};

// Setting 0 of streaming interface, no endpoints which means this is a zero-bandwidth
// setting to allow host to temporarily disable audio in case of bandiwdth problems
usb_desc_interface_c uac_std_streaming_interface_alt0 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = USB_CFG_IF_UAC_AUDIO_STREAMING,
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

// Setting 1 of streaming interface, regular operation
usb_desc_interface_c uac_std_streaming_interface_alt1 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = USB_CFG_IF_UAC_AUDIO_STREAMING,
  .bAlternateSetting    = 1,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

usb_desc_uac1_as_header_c uac_as_header = {
  .bLength            = sizeof(struct usb_desc_uac1_as_header),
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_AS_GENERAL,
  .bTerminalLink      = 2, // connected to output terminal
  .bDelay             = 1,
  .wFormatTag         = UAC_FORMAT_TYPE_I_PCM,
};

usb_desc_uac_format_type_i_discrete_c uac_format = {
  .bLength            = sizeof(struct usb_desc_uac_format_type_i_discrete) + 3,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_FORMAT_TYPE,
  .bFormatType        = UAC_FORMAT_TYPE_I,
  .bNrChannels        = 2,
  .bSubframeSize      = 2,
  .bBitResolution     = 16,
  .bSamFreqType       = 1,
  .tSamFreq           = {{0x40, 0x1F, 0x00}}, // 8000Hz, little endian
};

/*** UAC: endpoints ***********************************************************/

usb_desc_audio_endpoint_c uac_audio_endpoint = {
  .bLength          = sizeof(struct usb_desc_audio_endpoint),
  .bDescriptorType  = USB_DESC_ENDPOINT,
  .bEndpointAddress = USB_CFG_EP_UAC|USB_DIR_IN,
  .bmAttributes     = USB_XFER_ISOCHRONOUS,
  .wMaxPacketSize   = 512,
  .bInterval        = 4,
  .bRefresh         = 0,
  .bSynchAddress    = 0,
};

usb_desc_uac_iso_endpoint_c uac_iso_endpoint = {
  .bLength            = sizeof(struct usb_desc_uac_iso_endpoint),
  .bDescriptorType    = USB_DT_CS_ENDPOINT,
  .bDescriptorSubtype = UAC_AS_GENERAL,
  .bmAttributes       = 0,
  .bLockDelayUnits    = 0,
  .wLockDelay         = 0,
};

/*** Configuration ************************************************************/

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
    // UVC
    UVC_DESCRIPTORS_LIST
    // CDC
    CDC_DESCRIPTORS_LIST
    // UAC
    // TODO: { .generic   = (struct usb_desc_generic *) &usb_uvc_if_assoc              },
    { .interface =                             &usb_uac_std_ac_interface         },
    { .generic   = (struct usb_desc_generic *) &uac_ac_header                    },
    { .generic   = (struct usb_desc_generic *) &uac_input_terminal               },
    { .generic   = (struct usb_desc_generic *) &uac_output_terminal              },
    { .interface =                             &uac_std_streaming_interface_alt0 },
    { .interface =                             &uac_std_streaming_interface_alt1 },
    { .generic   = (struct usb_desc_generic *) &uac_as_header                    },
    { .generic   = (struct usb_desc_generic *) &uac_format                       },
    { .generic   = (struct usb_desc_generic *) &uac_audio_endpoint               },
    { .generic   = (struct usb_desc_generic *) &uac_iso_endpoint                 },
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

