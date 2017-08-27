# Audio Firmware
This directory contains the FX2 audio firmware for the HDMI2USB project. It uses
the Open Source fx2lib, a free reimplementation of the Cypress support library.

The firmware is responsible for transporting the audio data off of the FPGA. It
does this by enumerating as a USB Audio Class device; a standard way of
interfacing devices such as microphones to a host. Linux, Windows and OS X all
include support out of the box for reading taking to such devices.

# Building
The firmware uses the Open Source fx2lib, which will be downloaded as part of
the build process.

The build process requires git, make and SDCC. SDCC, the Small Device C
Compiler, is packaged in Debian and derivatives such as Ubuntu, as well as
Fedora:

`sudo apt-get install sdcc`
`sudo yum install sdcc`

There are three firmware versions included in this directory:

 * out - an audio playback device for implementing a loopback device using two
         FX2 miniboards (only supports FX2 miniboard)
 * fifo - an audio source that reads from the FIFO to send over USB
 * in - an audio source that generates a constant tone

Each can be compied using make. Currently only one can be built at a time and
all artifacts of any perviously built versions must be removed first (Fixme!)

`make BOARD=fx2miniboard FIRMWARE=out`
`make BOARD=fx2miniboard FIRMWARE=fifo`
`make BOARD=fx2miniboard FIRMWARE=in`

and can be cleaned using

`make BOARD=fx2miniboard FIRMWARE=out clean`
`make BOARD=fx2miniboard FIRMWARE=fifo clean`
`make BOARD=fx2miniboard FIRMWARE=in clean`

`load` works in the same way.

# Serial

 * Baud rate of 115200
 * Serial device /dev/ttyUSB0 (default on linux)
 * Connections
  - RXD -> PD3
  - GND -> GND
