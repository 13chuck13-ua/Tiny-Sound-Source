# Tiny Sound Source. GCC code
AVR-GCC firmware for Tiny Sound Source.

Device: ATtiny2313

Files:
 - main.c — source code.
 - Makefile — build instructions.
 - tss.hex — compiled firmware image.

Building:
 - Requires AVR-GCC and GNU Make.
 - Run make to build the firmware.

Flashing:
 - Use a compatible AVR programmer and the appropriate flashing utility to write tss.hex to microcontroller.
 - FUSES: LFUSE - 0xDF HFUSE - 0xD1
