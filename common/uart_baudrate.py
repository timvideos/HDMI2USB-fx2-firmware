#!/usr/bin/env python

"""
Code for testing timer baudrate generation errors.

NOTE: in practice, for the bit-banged uart implementation to work, the timer
      has to be configured for 2 times the desired serial baudrate
NOTE: only baudrate generation is being tested, cpu may not be able to handle
      higher baudrates when using bit-banged uart
"""

tested = [2400, 4800, 9600, 19200, 28800, 38400, 57600, 115200, 230400, 1000000, 2000000]


def u16(h, l):
    return ((h & 0xff) << 8) | (l & 0xff)


# simulate calculating rcap
def calc_rcap(baud, clk_multip):
    clk_out = int(12e6) // 4 * clk_multip
    n_clocks = clk_out // int(baud)
    rcap = 0xffff - (n_clocks - 1)
    return rcap


# get actual baud rate for given rcap
def calc_baud(rcap, clk_multip):
    clk_out = 12e6 * int(clk_multip) / 4
    n_clocks = (0xffff - int(rcap)) + 1
    baud = clk_out / n_clocks
    return baud


def test_baud_error(baud_desired, clk_multip):
    rcap = calc_rcap(baud_desired, clk_multip)
    baud_real = calc_baud(rcap, clk_multip)
    rel_error = (baud_real - baud_desired) / baud_desired
    return baud_real, rel_error


print('Clk freq,   Baud desired,       Baud real,  Rel error')
for clk_multip in [1, 2, 4]:
    for baud in tested:
        baud_real, rel_error = test_baud_error(baud, clk_multip)
        print('  {clk} MHz,   {desired:8} bps,  {actual:10.2f} bps,  {err:7.3f} %'.format(
            clk=12 * clk_multip, desired=baud, actual=baud_real, err=rel_error * 100
        ))
