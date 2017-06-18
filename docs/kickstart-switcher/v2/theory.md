# Theory of operation

A ROM is an array of storage locations indexed by a binary-encoded
address at the address pins of the chip. The standard 512kB Amiga ROM
is 256k x 16-bit words, addressed by pins A0-A17. Larger ROMs require
an extra address pin for each doubling of capacity: A18 (1MB), A19
(2MB), and so on.

These larger ROMs can be considered a banked (or multi-ROM-image)
version of the 512kB ROM: A0-A17 address the currently selected 512kB
bank (aka ROM image), while A18+ select between 2 or more banks.

The purpose of the Kickstart switcher, then, is to control the
bank-selection pin(s). Since this design can switch between up to 8
ROM images, it controls up to 3 address pins (A18-A20) via outputs
PB12-PB14. A0-A17 remain under control of the Amiga hardware.

The switcher defaults to image 0 at startup. The next image is
selected when it sees a low pulse at pin PA8 which is longer than 3
seconds. The input is expected to be attached to the active-low reset
line of the Amiga, so that holding Ctrl-Amiga-Amiga for multiple
seconds will trigger the Kickstart switch.
