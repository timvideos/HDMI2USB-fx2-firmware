
# TODO Items

## Import other existing firmware

Import the existing FX2 firmware used in the HDMI2USB project.

 - [ ] Import the serial firmware.
 - [ ] Import the ixo-usb-jtag firmware.

## HDMI2USB FX2 firmware refactor

Merge the FX2 firmware refactor that mithro's started.

 - [ ] Merge / finish the
       [fx2-refactor](https://github.com/timvideos/HDMI2USB-fx2-firmware/)
       branch.
 - [ ] Get the side channel from FX2 to FPGA working.
 - [ ] Get the UVC control for switching working (might need side channel
       working).
 - [ ] Get the UVC PTZ stuff working (might need side channel working).
 - [ ] Add the USB UVC preview stream.
 - [ ] Add the USB Audio support.
 - [ ] Fix up the USB Serial stuff on both the Atlys and Opsis.
 - [ ] Add USB HID support for button / trigger support.

## New firmware

 - [ ] USB Storage + vfat virtual disk
   * Might want to use existing fat library from Arduino?

 - [ ] USB Audio firmware (audio input/output)

 - [ ] USB Video firmware (video output)

 - [ ] FTDI emulation firmware
   * See https://github.com/mithro/fx2-ftdi-emulation

## ixo-usb-jtag firmware

 - [ ] Port ixo-usb-jtag to fx2lib
 - [ ] Improve the speed of `Port E` JTAG

## fx2lib

 - [ ] [Linux descriptors]() stuff.
 - [ ] 
 - [ ] 
