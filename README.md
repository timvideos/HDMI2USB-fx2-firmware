# HDMI2USB-fx2-firmware

This repository contains the firmware used in the 
[HDMI2USB project](https://hdmi2usb.tv) on the Cypress FX2 chip found on boards
such as the [Numato Opsis](https://opsis.hdmi2usb.tv) and the Digilent Atlys
boards.

## [microload](./microload)

microload is firmware which loads a second stage firmware from an FPGA via I2C.

In microload every program **byte** counts and thus is hand coded in assembly.

The firemware is used on the Numato Opsis board to allows the FPGA to switch
the FX2 functionality.

## [eeprom-unconfigured](./eeprom-unconfigured)

eeprom-unconfigured will cause the Cypress FX2 which boot with a given VID/PID
and then wait for full firmware to be loaded via USB.

## [JTAG](./jtag)

The JTAG firmware is used to allow loading gateware onto an FPGA.

## [HDMI2USB UVC+CDC](./hdmi2usb)

This is the primary firmware for HDMI2USB functionality.

The firmware is responsible for transporting the video data off of the FPGA. It
does this by enumerating as a USB Video Class device; a standard way of
interfacing devices such as webcams to a host. Linux, Windows and OS X all
include support out of the box for reading taking to such devices.

The firmware is also used for control and debugging of the system, through a
USB Communications Class Device. This is a common way of attaching serial ports
to the system, and under Linux it can be accessed at /dev/ttyUSBX.

