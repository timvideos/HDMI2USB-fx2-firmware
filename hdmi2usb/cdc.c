#include "cdc.h"

void cdc_config(struct cdc_config *config) {
  // configure interface numbers
  uint8_t if_num_data = config->if_num_comm + 1;
  ((__xdata struct usb_desc_interface *) &usb_cdc_if_cic)->bInterfaceNumber = config->if_num_comm;
  ((__xdata struct usb_desc_interface *) &usb_cdc_if_dic)->bInterfaceNumber = if_num_data;
  // interface association
  ((__xdata struct usb_desc_if_assoc *) &usb_cdc_if_assoc)->bFirstInterface = config->if_num_comm;
  // cic union
  ((__xdata struct usb_cdc_desc_functional_union *) &usb_cdc_func_cic_union)->bSubordinateInterface[0] = if_num_data;
  // configure enpoint numbers
  ((__xdata struct usb_desc_endpoint *) &usb_cdc_ep_comm)->bEndpointAddress     = config->ep_addr_comm | USB_DIR_IN;
  ((__xdata struct usb_desc_endpoint *) &usb_cdc_ep_data_out)->bEndpointAddress = config->ep_addr_data_host2dev;
  ((__xdata struct usb_desc_endpoint *) &usb_cdc_ep_data_in)->bEndpointAddress  = config->ep_addr_data_dev2host | USB_DIR_IN;
}

bool cdc_handle_usb_setup(__xdata struct usb_req_setup *req) {
  // We *very specifically* declare that we do not support, among others, SET_CONTROL_LINE_STATE
  // request, but Linux sends it anyway and this results in timeouts propagating to userspace.
  // Linux will send us other requests we explicitly declare to not support, but those just fail.
  if (req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
      req->bRequest == USB_CDC_PSTN_REQ_SET_CONTROL_LINE_STATE &&
      req->wIndex == 0 && req->wLength == 0)
  {
    ACK_EP0();
    return true;
  }

  // We *very specifically* declare that we do not support, among others, GET_LINE_CODING request,
  // but Windows sends it anyway and this results in errors propagating to userspace.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_IN) &&
     req->bRequest == USB_CDC_PSTN_REQ_GET_LINE_CODING &&
     req->wIndex == 0 && req->wLength == 7)
  {
    __xdata struct usb_cdc_req_line_coding *line_coding =
        (__xdata struct usb_cdc_req_line_coding *)EP0BUF;
    line_coding->dwDTERate = 115200;
    line_coding->bCharFormat = USB_CDC_REQ_LINE_CODING_STOP_BITS_1;
    line_coding->bParityType = USB_CDC_REQ_LINE_CODING_PARITY_NONE;
    line_coding->bDataBits = 8;
    SETUP_EP0_BUF(sizeof(struct usb_cdc_req_line_coding));
    return true;
  }

  // We *very specifically* declare that we do not support, among others, SET_LINE_CODING request,
  // but Windows sends it anyway and this results in errors propagating to userspace.
  if(req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
     req->bRequest == USB_CDC_PSTN_REQ_SET_LINE_CODING &&
     req->wIndex == 0 && req->wLength == 7)
  {
    SETUP_EP0_BUF(0);
    return true;
  }

  return false;
}

/*** Descriptors **************************************************************/

usb_desc_if_assoc_c usb_cdc_if_assoc = {
  .bLength              = sizeof(struct usb_desc_if_assoc),
  .bDescriptorType      = USB_DESC_IF_ASSOC,
  .bFirstInterface      = 0, // cdc_config
  .bInterfaceCount      = 2,
  .bFunctionClass       = USB_IFACE_CLASS_CIC,
  .bFunctionSubClass    = USB_IFACE_SUBCLASS_CDC_CIC_ACM,
  .bFunctionProtocol    = USB_IFACE_PROTOCOL_CDC_CIC_NONE,
  .iFunction            = 0,
};

usb_cdc_desc_functional_header_c usb_cdc_func_cic_header = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_header),
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_HEADER,
  .bcdCDC               = 0x0120,
};

usb_cdc_desc_functional_acm_c usb_cdc_func_cic_acm = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_acm),
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_ACM,
  .bmCapabilities       = 0,
};

usb_cdc_desc_functional_union_c usb_cdc_func_cic_union = {
  .bLength              = sizeof(struct usb_cdc_desc_functional_union) +
                          sizeof(uint8_t) * 1,
  .bDescriptorType      = USB_DESC_CS_INTERFACE,
  .bDescriptorSubType   = USB_DESC_CDC_FUNCTIONAL_SUBTYPE_UNION,
  .bControlInterface    = 0,
  .bSubordinateInterface = { 0/* cdc_config */ },
};

/*** Interface descriptors ****************************************************/

usb_desc_interface_c usb_cdc_if_cic = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 0, // cdc_config
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 1,
  .bInterfaceClass      = USB_IFACE_CLASS_CIC,
  .bInterfaceSubClass   = USB_IFACE_SUBCLASS_CDC_CIC_ACM,
  .bInterfaceProtocol   = USB_IFACE_PROTOCOL_CDC_CIC_NONE,
  .iInterface           = 0,
};

usb_desc_interface_c usb_cdc_if_dic = {
  .bLength              = sizeof(struct usb_desc_interface),
  .bDescriptorType      = USB_DESC_INTERFACE,
  .bInterfaceNumber     = 0, // cdc_config
  .bAlternateSetting    = 0,
  .bNumEndpoints        = 2,
  .bInterfaceClass      = USB_IFACE_CLASS_DIC,
  .bInterfaceSubClass   = USB_IFACE_SUBCLASS_CDC_DIC,
  .bInterfaceProtocol   = USB_IFACE_PROTOCOL_CDC_DIC_NONE,
  .iInterface           = 0,
};

/*** Endpoint descriptors *****************************************************/

usb_desc_endpoint_c usb_cdc_ep_comm = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 0, // cdc_config
  .bmAttributes         = USB_XFER_INTERRUPT,
  .wMaxPacketSize       = 8,
  .bInterval            = 10,
};

usb_desc_endpoint_c usb_cdc_ep_data_out = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 0, // cdc_config
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};

usb_desc_endpoint_c usb_cdc_ep_data_in = {
  .bLength              = sizeof(struct usb_desc_endpoint),
  .bDescriptorType      = USB_DESC_ENDPOINT,
  .bEndpointAddress     = 0, // cdc_config
  .bmAttributes         = USB_XFER_BULK,
  .wMaxPacketSize       = 512,
  .bInterval            = 0,
};
