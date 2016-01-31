# Overview

Programmer for microcontrollers (ATTINY85) to receive intercept gamecube controller serial output and
flash LEDs appropriately.

## Installation & Usage

toolchain:
    sudo apt-get install gcc-avr avr-libc avrdude

building/flashing
    make; make flash;

## Dependencies

This project depends on two libraries, which are included in the libs folder.
Josh Levine's simple [NeoPixel Demo](https://github.com/bigjosh/SimpleNeoPixelDemo) library
