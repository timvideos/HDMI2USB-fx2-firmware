# Lights Example
This example flashes the LEDs present on the FX2LP CY7C68013A mini board. It
can be used for testing loading of firmware onto these boards. The expected
pattern is D1 turning on, then D2 250ms later, then D1 off, then D2 off and the
cycle repeats

The firmware is built using `make` and loaded using `make load`, which uses the
[HDMI2USB-mode-switch](https://github.com/timvideos/HDMI2USB-mode-switch)
utility to load the firmware onto the FX2 chip.

![](http://cloud.paddatrapper.me/s/SSbZnHGQVJRFIj6/download)
