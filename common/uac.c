#include "uac.h"

extern usb_descriptor_set_c usb_descriptor_set;

// keeps track of current alternate setting of streaming interface
__xdata uint8_t uac_as_alt_setting = 0;
// store the interface numbers for set/get interface handlers
__xdata static uint8_t if_num_ctrl;
__xdata static uint8_t if_num_streaming;

bool uac_handle_usb_set_interface(uint8_t interface, uint8_t alt_setting) {
  if (interface == if_num_ctrl && alt_setting == 0) {
    usb_reset_data_toggles(&usb_descriptor_set, interface, alt_setting);
    return true;
  }
  if (interface == if_num_streaming && (alt_setting == 0 || alt_setting == 1)) {
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
  if (interface == if_num_ctrl) {
    EP0BUF[0] = 0; // only 1 alternate setting
    SETUP_EP0_BUF(1);
  }
  if (interface == if_num_ctrl) {
    EP0BUF[0] = uac_as_alt_setting;
    SETUP_EP0_BUF(1);
  }
  return false; // not handled
}

void uac_config(struct uac_configuration *config) {
  if_num_ctrl = config->if_num_ctrl;
  if_num_streaming = config->if_num_ctrl + 1;
  // interface numbers
  ((__xdata struct usb_desc_interface *) &usb_uac_std_ac_interface)->bInterfaceNumber = if_num_ctrl;
  ((__xdata struct usb_desc_interface *) &usb_uac_std_streaming_interface_alt0)->bInterfaceNumber = if_num_streaming;
  ((__xdata struct usb_desc_interface *) &usb_uac_std_streaming_interface_alt1)->bInterfaceNumber = if_num_streaming;
  // endpoint numbers
  ((__xdata struct usb_desc_endpoint *) &usb_uac_audio_endpoint)->bEndpointAddress = config->ep_addr_streaming | USB_DIR_IN;
  // strings
  ((__xdata struct usb_desc_uac_input_terminal *) &usb_uac_input_terminal)->iChannelNames = config->i_str_channel_left;
}

/*** Descriptors **************************************************************/

usb_desc_interface_c usb_uac_std_ac_interface = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 0, // uac_config
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOCONTROL,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

#define LENGTH_uac_input_terminal (sizeof(struct usb_desc_uac_input_terminal))
usb_desc_uac_input_terminal_c usb_uac_input_terminal = {
  .bLength            = LENGTH_uac_input_terminal,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_INPUT_TERMINAL,
  .bTerminalID        = 1,
  .wTerminalType      = UAC_INPUT_TERMINAL_MICROPHONE,
  .bAssocTerminal     = 0,
  .bNrChannels        = 2, // stereo
  .wChannelConfig     = (UAC_CHANNEL_LEFT | UAC_CHANNEL_RIGHT),
  .iChannelNames      = 0, // uvc_config // first channel name, other channels must have consequetive indices
  .iTerminal          = 0,
};

#define LENGTH_uac_output_terminal (sizeof(struct usb_desc_uac1_output_terminal))
usb_desc_uac1_output_terminal_c usb_uac_output_terminal = {
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
usb_desc_uac1_ac_header_c usb_uac_ac_header = {
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
usb_desc_interface_c usb_uac_std_streaming_interface_alt0 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 0, // uac_config
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 0,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

// Setting 1 of streaming interface, regular operation
usb_desc_interface_c usb_uac_std_streaming_interface_alt1 = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 0, // uac_config
  .bAlternateSetting    = 1,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_CLASS_AUDIO,
  .bInterfaceSubClass   = USB_SUBCLASS_AUDIOSTREAMING,
  .bInterfaceProtocol   = 0,
  .iInterface           = 0,
};

usb_desc_uac1_as_header_c usb_uac_as_header = {
  .bLength            = sizeof(struct usb_desc_uac1_as_header),
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubtype = UAC_AS_GENERAL,
  .bTerminalLink      = 2, // connected to output terminal
  .bDelay             = 1,
  .wFormatTag         = UAC_FORMAT_TYPE_I_PCM,
};

usb_desc_uac_format_type_i_discrete_c usb_uac_format = {
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

usb_desc_audio_endpoint_c usb_uac_audio_endpoint = {
  .bLength          = sizeof(struct usb_desc_audio_endpoint),
  .bDescriptorType  = USB_DESC_ENDPOINT,
  .bEndpointAddress = 0, // uac_config
  .bmAttributes     = USB_XFER_ISOCHRONOUS,
  .wMaxPacketSize   = 512,
  .bInterval        = 4,
  .bRefresh         = 0,
  .bSynchAddress    = 0,
};

usb_desc_uac_iso_endpoint_c usb_uac_iso_endpoint = {
  .bLength            = sizeof(struct usb_desc_uac_iso_endpoint),
  .bDescriptorType    = USB_DT_CS_ENDPOINT,
  .bDescriptorSubtype = UAC_AS_GENERAL,
  .bmAttributes       = 0,
  .bLockDelayUnits    = 0,
  .wLockDelay         = 0,
};
