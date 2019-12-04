# HDMI2USB-fx2-firmware
[![Build Status](https://api.travis-ci.org/timvideos/HDMI2USB-fx2-firmware.svg?branch=master)](https://travis-ci.org/timvideos/HDMI2USB-fx2-firmware)

This repository contains the firmware used in the
[HDMI2USB project](https://hdmi2usb.tv) on the Cypress FX2 chip found on boards
such as the [Numato Opsis](https://opsis.hdmi2usb.tv) and the Digilent Atlys
boards.

Firmware uses [libfx2](https://github.com/whitequark/libfx2), which fx2tool to load and flash firmware. It is compiled with SDCC version 3.9, but other versions may work.

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

 * [ixo-usb-jtag](https://github.com/mithro/ixo-usb-jtag)

## [HDMI2USB UVC+UAC+CDC](./hdmi2usb)

This is the primary firmware for HDMI2USB functionality.

The firmware is responsible for transporting the video data off of the FPGA. It
does this by enumerating as a USB Video Class device; a standard way of
interfacing devices such as webcams to a host. Linux, Windows and OS X all
include support out of the box for reading taking to such devices. It also enumerates as USB audio class for streaming audio data.

The firmware is also used for control and debugging of the system, through a
USB Communications Class Device. This is a common way of attaching serial ports
to the system, and under Linux it can be accessed at /dev/ttyUSBX.

# Cypress EZ-USB FX2LP

FIXME: Put some documentation about the FX2 here.

[Cypress EZ-USB FX2LP](http://www.cypress.com/?id=193)

> Cypress’s EZ-USB® FX2LP™ (CY7C68013A/14/15/16A) is a low power, highly
> integrated USB 2.0 microcontroller. FX2LP has a fully configurable General
> Programmable Interface (GPIF™) and master/slave endpoint FIFO (8-bit or
> 16-bit data bus), which provides an easy and glueless connection to popular
> interfaces such as ATA, UTOPIA, EPP, PCMCIA, DSP, and most processors.

# Boards

## [Numato Opsis](https://opsis.hdmi2usb.tv)

 * FX2 - `CY7C68013A_100AC` - 100 pin version
 * EEPROM - `24AA02E48` -  256*bytes* (128 usable)

 * `Bank A`/`Port A`, `Bank B`/`Port B`, `Bank C`/`Port D`, `Bank F`/`Port C`, `Bank H`/`RDY+CTRL` - FPGA IO Pins
 * `Bank G`/`Port E` connected to FPGA JTAG.
 * 2 x Hardware UARTs
 * 1 x Interrupts?

 * [Further Documentation](https://opsis.hdmi2usb.tv/features/usb-peripheral.html)

## [Digilent Atlys](https://www.digilentinc.com/atlys)

 * FX2 - `CY7C68013A-56` - 56 pin version
 * EEPROM - `24AA128` - 16k*bytes*

 * `Bank A`/`Port A`, `Bank B`/`Port B`, `RDY+CTRL` - FPGA IO Pins
 * `Bank C/Port D` connected to FPGA JTAG.


 * [Further Documentation](https://reference.digilentinc.com/atlys:atlys:atlys)

# Building

The HDMI2USB firmware is built using *hdmi2usb/Makefile*. To build the firmware [SDCC](http://sdcc.sourceforge.net/) is required.

To load the firmware, hdmi2usb-mode-switch tool is required:

    hdmi2usb-mode-switch --load-fx2-firmware hdmi2usb/hdmi2usb.ihex

The `hdmi2usb-mode-switch` command can be installed on Debian Stretch and later from the `hdmi2usb-mode-switch` package. Otherwise see [HDMI2USB-mode-switch](https://github.com/timvideos/HDMI2USB-mode-switch).

By default the firmware will be built for the Opsis board. This can be changed
by passing `BOARD=atlys` to make to build for the Atlys boards. 

There are three ways to run `hdmi2usb-mode-switch`:

 1. As root
 1. Install the
 [unbind-helper](https://github.com/timvideos/HDMI2USB-mode-switch/blob/master/unbind-helper.c)
 as a setuid binary
 1. Install the
 [udev-rules](https://github.com/timvideos/HDMI2USB-mode-switch/tree/master/udev)
 which sets the permissions of the unbind.
