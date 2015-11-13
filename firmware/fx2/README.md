# Cypress FX2 Firmware

This directory contains the FX2 firmware for the HDMI2USB project. It uses the
Open Source fx2lib, a free reimplementation of the Cypress support library.

The firmware is responsible for transporting the video data off of the FPGA. It
does this by enumerating as a USB Video Class device; a standard way of
interfacing devices such as webcams to a host. Linux, Windows and OS X all
include support out of the box for reading taking to such devices.

The firmware is also used for control and debugging of the system, through a
USB Communications Class Device. This is a common way of attaching serial ports
to the system, and under Linux it can be accessed at /dev/ttyUSBX.

## USB Structure

 * The HDMI2USB is a "USB Composite Device"

 * We use "USB Video Class" for the video interface.
 * We use "CDC-ACM" for the serial control port.
 * TODO: We use the "Audio Class" for the audio interface.



TODO: Due to the very limited number of endpoints available on the FX2 could
make use of the "Common Class Shared Endpoints" and "Common Class
Notifications"? Doesn't look like Linux implements this?
 * [Common Class Shared Endpoints](http://cscott.net/usb_dev/data/devclass/ccsSharedEPFeature1_0.pdf)
 * [Common Class Notifications](http://cscott.net/usb_dev/data/devclass/ccsNotification1_0.pdf)

TODO: Due to the very limited number of endpoints consider support the I/O
Networks Serial Protocol to enable multiple serial ports.
 * http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/drivers/usb/serial/io_ionsp.h

TODO: Investigate using the "Audio/Video Device Class". The "Figure 4-1:
AVFunction Global View" in the "Universal Serial Bus Device Class Definition
for Audio/Video Devices" specification looks very much like a HDMI2USB block
diagram.

### USB Crib Sheet

 * Each device can have multiple "configurations".
    * Only one configuration can be enabled at one time.
    * Changing configurations requires stopping / starting the whole
      device.

 * Each configuration can have multiple "interfaces".
    * Multiple interfaces can be enabled at a time.
    * Each interface has a number of endpoints. 
    * Endpoints *can't* be shared between interfaces.


 * Each UVC function (a v4l2 device) requires **2 endpoints**
   * One for the data
   * (optional) One for the async control changes

 * Each CDC function (a serial port) requires **3 endpoints**
   * One for the control
   * One for the TX data
   * One for the RX data


### Video Device Class || USB Video Class (UVC)

The Video Device Class (VDC) was renamed to USB Video Class (UVC) in version 1.1

 * [Wikipedia Page](https://en.wikipedia.org/wiki/USB_video_device_class)
 * [Specification v1.5](http://www.usb.org/developers/docs/devclass_docs/USB_Video_Class_1_5.zip)
 * [Specification v1.1](http://www.usb.org/developers/docs/devclass_docs/USB_Video_Class_1_1_090711.zip)


#### Versions

 * 1.0  - Initial version. 
 * 1.0a -
 * 1.0c -
 * 1.1  - Added async controls 
 * 1.5  - Added H.264 + VP8 payloads + video encoder controls

Important options
 * Video controls?

 * Standard units?
 * Extension units?

 * Endpoint types (Bulk or Isochronous)

 * Parameter negotiation using probe

##### Linux supports 

##### Windows supports

 * https://msdn.microsoft.com/en-us/library/windows/hardware/ff568651(v=vs.85).aspx

 * V1.0 in
  * Windows XP with Service Pack 2
  * Windows Vista
  * Windows CE 6.0

 * V1.1 in
  * Windows 7

 * V1.5 in
  * Windows 8
  * Windows 10

TODO: Look into the USB Video Class Extension Unit stuff

 * https://msdn.microsoft.com/en-us/library/windows/hardware/ff560772(v=vs.85).aspx

TODO: Look at the USBView / UVCView tool under Windows

 * https://msdn.microsoft.com/en-us/library/windows/hardware/ff554257(v=vs.85).aspx


** USB Composite Devices Under Windows **

 * https://msdn.microsoft.com/en-us/library/windows/hardware/ff537109(v=vs.85).aspx

 * The device class field of the device descriptor (bDeviceClass) must contain
   a value of zero, or the class (bDeviceClass), subclass (bDeviceSubClass),
   and protocol (bDeviceProtocol) fields of the device descriptor must have the
   values 0xEF, 0x02 and 0x01 respectively, as explained in USB Interface
   Association Descriptor.
 
 * The device must have multiple interfaces.
 * The device must have a single configuration.


##### OS X

 * Some driver from 10.4.3?
 * 10.4.9 driver supports iChat?


# Building

The firmware uses the Open Source fx2lib, which will be downloaded as part of
the build process.

The build process requires git, make and sdcc. SDCC, the Small Device C
Compiler, is packaged in Debian and derivatives such as Ubuntu, as well as
Fedora:

`sudo apt-get install sdcc`
`sudo yum install sdcc`

`make`

# Flashing

Use fx2loader from the libfpgalink project:

`fx2loader -v 0925:3881 firmware.hex ram`


## Planned HDMI2USB USB endpoint usage

TODO:

| Endpoint | Direction | Transfer type | Used? | Comments                              |
| -------- | --------- | ------------- | ----- | --------------------------------------|
|     0    |     -     | CONTROL       | No    | USB Reserved                          |
|     1    |    IN     | BULK          | Yes   | CDC UART RX                           |
|     1    |    OUT    | BULK          | Yes   | CDC UART TX                           |
|     2    |    IN     | INT           | Yes   | CDC Control                           |
|     4    |    IN     | INT           | Yes   | Primary UVC Camera status             |
|     6    |    IN     | ISO or BULK   | Yes   | Primary UVC Camera data               |
|     8    |    IN     | BULK          | Yes   | Preview UVC Camera data               |

Endpoint 8 can be disabled to make EP6, 1024 bytes, double buffered.


## What Cypress FX2LP supports

| Endpoint | Direction  | Transfer type | Comments                              |
| -------- | ---------- | ------------- | --------------------------------------|
|     0    |      -     | Control       | Reserved |
|     1    | IN and OUT | INT/BULK      | 64-byte buffers for smaller payloads |
|     2    | IN or OUT  | BULK/ISO/INT  | 512 or 1024 byte buffers for larger payloads |
|     4    | IN or OUT  | BULK/ISO/INT  |  |
|     6    | IN or OUT  | BULK/ISO/INT  |  |
|     8    | IN or OUT  | BULK/ISO/INT  |  |



# References
    Create a USB Virtual COM Port: http://janaxelson.com/usb_virtual_com_port.htm
    USBCDC1.2 Spec PSTN120.pdf Page

