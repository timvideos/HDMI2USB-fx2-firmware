# Serial Example
This serial example is for testing serial connections from the FX2 chip. It is
uses the same setup as the HDMI2USB firmware, which is:

* Baud rate of 115200
* Serial device /dev/ttyUSB0 (default on linux)
* RXD -> PD3
* TXD -> RDY0
* GND -> GND

This example prints "This is the serial example for the HDMI2USB firmware" to
the TXD pin every two seconds

To build the firmware run `make`, which will pull in the required dependencies
and build the `serial.hex` firmware image. This can be loaded using the 
[HDMI2USB-mode-switch](https://github.com/timvideos/HDMI2USB-mode-switch)
utility, which is what `make load` uses.

Under linux serial can be read using minicom. This example was tested using the
Digilent PmodUSBUART Serial to USB adaptor. 

## Wiring Diagram
![](http://cloud.paddatrapper.me/s/h1M4CxbsItW8aM4/download)
