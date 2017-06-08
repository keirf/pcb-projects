# Piggybacked Kickstart Switcher, v2.1

## How to use this guide

If you are received this switcher in kit form or are purchasing the
parts yourself then please read these instructions from start to
finish. If you have received this switcher fully assembled then you
can skip to the last section ("Installing the ROM").


## Theory of operation

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


## Building the switcher

To build a switcher for A500 or A600 you will need the following parts:
- STM32 "Blue Pill" board 
- USB-TTL serial adapter
- 12-pin 2.54mm-pitch right-angled single-row PCB header
- PC motherboard speaker/buzzer
- Hookup wire (kynar or similar)
- Suitable capacity EPROM: 27C160 (up to 4 images), 27C322 (up to 8 images)

First cut the PCB header into 2-pin, 3-pin and 7-pin sections. Solder
these along one side of the bare STM32 board as show below. Make sure
the headers are installed straight and even.

![Header pins soldered to the STM32 board](assets/10_angled_headers_added.jpg)

Next we program the STM32. You
will need a USB-TTL adapter such as shown below. These are available
for around £1 on Ebay.

![Example USB-TTL serial adapter](assets/22_usbttl.jpg)

Now adjust the yellow boot jumpers on the STM32 board and attach the
Dupont jumper cables supplied with the USB-TTL adapter as shown. In
this example the cables to connect to the following USB-TTL pins:
White to GND, Black to 5V, Purple to RXD, Blue to TXD.

![Programming the STM32](assets/20_programming_stm32.jpg)

Download the firmware zip file from [here (Google Drive)](https://drive.google.com/file/d/0B6F45EdkSMp1MV80cnJWRXFXMkk/view). This
contains a number of HEX programming files, one for each possible
number of ROM images (2 through 8). On Linux the firmware can now be
programmed with the stm32flash command-line utility:
```
 # unzip kswitch_v2.zip
 # sudo stm32flash -vw kswitch_v2/kswitch.hex /dev/ttyUSB0
```

Once the device is programmed you can remove the boot and debug
headers. The boot-select pins should be forced to 00 by bridging the
appropriate through holes with wire or low-value chip resistors as
shown.

![Final adjustments to the STM32 board](assets/33_stm32_headers_removed.jpg)

If fitting to an A500 with its top shield intact you may also wish to
remove the USB socket. The mounting tabs can be snipped with small
side cutters and the socket then gently pulled off.

Finally, you must configure the firmware via the serial line. To do
this, connect to the USB-serial line via a dumb serial terminal at
115200,8n1, and reset the STM32 board. On Linux, for example:
```
 # sudo miniterm.py /dev/ttyUSB0 115200
 > 1
 > <your number of images>
 > 9
```

## Programming the EPROM

This step requires a blank 27C160 or 27C322. Also typically you will
use a TL866 programmer and the 27C160/322 adapter which I build and
sell. You require 512kB ROM images which are 'byte-swapped' (bytes in
each 16-bit word are swapped). If this is done correctly then, when
loaded into the MiniPro programming software, the Kickstart copyright
message will appear in the buffer window ungarbled. If it's garbled,
you haven't swapped! If you are using a 256kB image (eg Kickstart 1.3)
then double it up.

In Linux or MacOS terminals, the double-up and byte swap could be done
as follows:
```
 # dd if=kick13.rom of=kick13_swapped.rom conv=swab
 # cat kick13_swapped.rom >kick13_swapped_doubled.rom
 # cat kick13_swapped.rom >>kick13_swapped_doubled.rom
```
On Windows you will have to find your own way!

Plug my adapter into the TL866 and set the jumpers to
`A20/A19/A18=0/0/0`. Select device `27C4096` and deselect `Check
ID`.Insert the EPROM into the adapter. Load your default ROM image and
program the EPROM.

Now load your second ROM image, select `A20/A19/A18=0/0/1`, and program
the EPROM.

Continue this process for your remaining ROM images, setting the
jumpers in this sequence: `0/1/0`, `0/1/1`, `1/0/0`, `1/0/1`, `1/1/0`,
`1/1/1`,


## Physically modifying the EPROM

The 27C160/27C322 must be physically modified to support the Kickstart
switcher. The below steps are for a 27C322 device. They also apply to 27C160 but ignore references to pin 32 and leave it unmodified!

![Modified 27C322 EPROM](assets/30_modded_27c322.jpg)

1. Lift pins 1 (A18), 42 (A19), and 32 (A20, 27C322 only). Cut them short
   (but with enough left to solder onto).

2. Flux pins 1, 22, 31, 32 (27C322 only), and 42.

3. Apply solder to all fluxed pins and then attach a short piece of
hookup wire ('Kynar' wire, or a strand from an ATA 80 ribbon cable is
appropriate here) to each one.

4. Attach the switcher to the top of the EPROM with hot glue or
double-sided foam tape with the angled headers on the right-hand side.
Pre-programmed EPROMs are supplied with tape already installed.

5. Connect pin 22 (VCC) to 5V on the switcher.

6. Connect pin 31 (VSS) to G (GND) on the switcher.

7. Connect pin 1 (A18) to B12 on the switcher.

8. Connect pin 42 (A19) to B13 on the switcher.

9. 27C322 only: Connect pin 32 (A20) to B14 on the switcher.

![Switcher attached to 27C322](assets/36_stm32_attached_27c322.jpg)

## Installing the ROM

The ROM is directly compatible with any single-ROM Amiga with a 42-pin
ROM socket. In this case insert the ROM with its pin 1 in pin 1 of the
socket (however since this pin is lifted, it will of course not make
contact with the socket).

If the Amiga has a 40-pin socket then ensure that pin 31 of the socket
is connected to VCC. This is true for A500 Rev 6 and later, A500 Plus,
and A600, among others. If compatible, insert the ROM with pin 2 in
pin 1 of the socket (ie. the lifted pins overhang the socket).

Attach the PC motherboard speaker to pins G and B8 of the
switcher. Attach A18 of the switcher to the Amiga's reset line. A
jumper wire and probe clip are supplied for this purpose (separately
if supplied in kit form; soldered together in assembled form).

**WARNING:** The ROM will not work reliably until connected to the
reset line. It will switch randomly between the programmed Kickstart
images causing crashes.

![Vanilla hookup to the switcher](assets/40_hookup_vanilla.jpg)


### A500 Installation
Clip to the leg of any DIL chip connected to the _RST line. Paula pin
11 is a good choice (see picture). You may
need to bend and/or open up the probe clip a little to hook it around
the chip pin -- make sure to retract the clip as far as possible and
snug the probe up against the pin, and check that the clip does not
touch any adjacent pin.

![Hookup to Paula pin 11](assets/50_a500_hookup_paula.jpg)

If you want a more permanent connection, _RST is available at various
vias on the A500 motherboard (eg. there is one near Paula on at least
Rev 6A: see below) or at resistor array RP101. A buzz
test with a multimeter for continuity with Paula pin 11 will confirm
that a via or track carries _RST.

![Hookup to Rev 6A via](assets/51_a500_hookup_via.jpg)

### A2000 Installation
Similar to A500 you can clip to any DIL chip connected to the _RST
line: eg. Paula pin 11, 68000 pin 18, or Gary pin 41.

### A600 Installation
Clip to pin 1 of the IDE port if it is unused.

![Hookup to A600 IDE port](../v1/assets/30_a600_hookup_ide.jpg)
![Hookup to A600 IDE port](../v1/assets/31_a600_hookup_ide.jpg)

Otherwise you will need to solder to resistor
R951B near Paula.

![Hookup to A600 resistor R951B](../v1/assets/40_a600_hookup_r951b.jpg)
![Hookup to A600 resistor R951B](../v1/assets/41_a600_hookup_r951b.jpg)


## User-Selectable Options

The default 3-second delay to switch ROM image can be reduced to 2
seconds by placing a jumper across pins B3-A15, or increased to 4
seconds by jumpering B3-B4.

The default speaker volume can be decreased by jumpering B5-B6, or
increased by jumpering B6:B7.

Below shows how to set the jumpers to reduce the switching delay and
quieten the speaker.

![Jumpers setting example 1](assets/46_hookup_short.jpg)

And this is how to increase the switching delay and the volume.

![Jumper setting example 2](assets/43_hookup_long.jpg)

These are just two examples: Volume and delay settings can be adjusted
independently.
