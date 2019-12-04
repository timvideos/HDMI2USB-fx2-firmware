#include "uvc.h"

__xdata uint8_t valuesArray[26] =
    {
        0x01, 0x00,             /* bmHint : No fixed parameters */
        0x01,                   /* Use 1st Video format index */
        0x01,                   /* Use 1st Video frame index */
        0x2A, 0x2C, 0x0A, 0x00, /* Desired frame interval in 100ns */

        0x00, 0x00,             /* Key frame rate in key frame/video frame units */
        0x00, 0x00,             /* PFrame rate in PFrame / key frame units */
        0x00, 0x00,             /* Compression quality control */
        0x00, 0x00,             /* Window size for average bit rate */

        0x05, 0x00,             /* Internal video streaming i/f latency in ms */

        0x00, 0x20, 0x1C, 0x00, /* Max video frame size in bytes*/
        0x00, 0x04, 0x00, 0x00  /* No. of bytes device can rx in single payload (1024) */
};

__xdata uint8_t fps[2][4] = {{0x2A, 0x2C, 0x0A, 0x00}, {0x54, 0x58, 0x14, 0x00}};        // 15 ,7
__xdata uint8_t frameSize[2][4] = {{0x00, 0x00, 0x18, 0x00}, {0x00, 0x20, 0x1C, 0x00}};  // Dvi , HDMI

bool uvc_handle_usb_setup(__xdata struct usb_req_setup *req) {
  if (req->bmRequestType == (USB_RECIP_IFACE|USB_TYPE_CLASS|USB_DIR_OUT) &&
      req->bRequest == USB_REQ_CLEAR_FEATURE) 
  {
    EP0BCH = 0;
    EP0BCL = 26;
    SYNCDELAY;
    while (EP0CS & _BUSY)
      ;
    while (EP0BCL != 26)
      ;

    valuesArray[2] = EP0BUF[2];  // formate
    valuesArray[3] = EP0BUF[3];  // frame

    // fps
    valuesArray[4] = fps[EP0BUF[2] - 1][0];
    valuesArray[5] = fps[EP0BUF[2] - 1][1];
    valuesArray[6] = fps[EP0BUF[2] - 1][2];
    valuesArray[7] = fps[EP0BUF[2] - 1][3];

    valuesArray[18] = frameSize[EP0BUF[3] - 1][0];
    valuesArray[19] = frameSize[EP0BUF[3] - 1][1];
    valuesArray[20] = frameSize[EP0BUF[3] - 1][2];
    valuesArray[21] = frameSize[EP0BUF[3] - 1][3];

    EP0BCH = 0;  // ACK
    EP0BCL = 0;  // ACK
    return true;
  }

  if (req->bRequest == USB_UVC_GET_CUR || req->bRequest == USB_UVC_GET_MIN || 
      req->bRequest == USB_UVC_GET_MAX) 
  {
    int i;
    SUDPTRCTL = 0x01;

    for (i = 0; i < 26; i++)
      EP0BUF[i] = valuesArray[i];

    EP0BCH = 0x00;
    SYNCDELAY;
    EP0BCL = 26;
    return true;

    // FIXME: What do these do????
    // case UVC_SET_CUR:
    // case UVC_GET_RES:
    // case UVC_GET_LEN:
    // case UVC_GET_INFO:

    // case UVC_GET_DEF:
    // FIXME: Missing this case causes the following errors
    // uvcvideo: UVC non compliance - GET_DEF(PROBE) not supported. Enabling workaround.
    // Unhandled Vendor Command: 87

  }

  return false;
}
