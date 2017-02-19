A bootloader for the Cypress FX2 designed to be as small as possible.

In microload every program **byte** counts and thus is hand coded in assembly.

It uses `as31 - Intel 8031/8051 assembler`. (FIXME: Document why?)

The firmware is used on the Numato Opsis board to allow the FPGA to switch
the FX2 functionality.

As the microload code is located at XXX, this region can't be loaded with
stuff. It can be reused after the loading is finished.

