# Alesis Q88
Reverse Engineer Project
## About
Project to reverse engineer a full 88 key MIDI controller and ultimately make MIDI 2.0 compatible. Chose the [Alesis Q88](https://www.alesis.com/products/view/q88) as it is relatively cheap for a full keyboard.
## Tools
(only built on Windows thus far)
1. [NuEclipse](https://www.nuvoton.com/tool-and-software/ide-and-compiler/)
2. Hardware debugger
   1. [Nu-Link-Pro](https://www.nuvoton.com/tool-and-software/debugger-and-programmer/1-to-1-debugger-and-programmer/nu-link-pro/)   
3. 2mm single row pin header (4 pin)
4. 2mm female connector with wires to plug into the 4 pin header
5. Solder iron
6. Solder wick
## Prep
1. Turn over the keyboard and remove all screws (there are a lot).
2. Carefully turn over the keyboard and remove the top cover noting it is connected via wires (unplug the 2 cables).
3. Unplug the 2 keyboard ribbon cables.
4. Unscrew the "L" shaped control board and remove it (use proper ESD protection, ground yourself!).
5. Locate the 2mm 4 pin connector (not populated) on the PCB and CAREFULLY remove the solder with the solder iron and soler wick.
6. Solder in the new 2mm pin header.
7. Plug in the 2mm female connector with wires and connect the wires to the Nu-Link-Pro.
   1. Pin 1 - ICEDAT
   2. Pin 2 - ICECLK
   3. Pin 3 - RESET
   4. Pin 4 - GND
8. Power the board with USB.
## Building
1. The Alesis Q88 is built on an [NUC120RD2DN](https://www.nuvoton.com/products/microcontrollers/arm-cortex-m0-mcus/nuc120-122-123-220-usb-series/nuc120rd2dn/).
2. Open the project in NuEclipse and build.
3. Set the desired MIDI version (MIDI_VERSION=1 or MIDI_VERSION=2) preprocessor in project settings (C/C++ Build -> Settings) -> (Cross ARM GNU C Compiler -> Preprocessor).
3. Debugging is a little slow compared to other tools but it works, so be patient between starting a debug session and hitting main().
## Misc
1. Each key on the keyboard was designed with a 2 stage micro-switch. Just barely pressing the key registers the first actuation, continuing the press motion activates the second stage. Noting the delta time between the actuations represents the key velocity (loudness). The circuit is multiplexed so we need to scan at a high rate.
2. MIDI 2.0 implemented but not tested so far.
## To Do
1. Test MIDI 2.0 support.
2. Some of the extra advanced functions.
3. Add commenting.
4. Expression pedal.
5. Directional/transport button support (buttons work but not handled).
## Code
1. keyboard.c owns the keyboard task, it scans and processes key presses.
2. Went with a super loop instead of an OS due to processor resource constraints.
## Peripheral Map
To the best of my knowledge, here is the map of microcontroller pins:

| pin | pin name | function               |
| --- | --- |------------------------|
|1|PB.14| keyboard key 7 |
|2|PB.13| keyboard key 6 |
|3|PB.12| keyboard key 5 |
|4|X32O| 12MHz XTAL |
|5|X32I| 12MHz XTAL |
|6|PA.11| keyboard detect group 9 |
|7|PA.10| keyboard press group 9 |
|8|PA.9| keypad row 1 |
|9|PA.8| keypad row 2 |
|10|PB.4| keypad row 3 |
|11|PB.5| serial midi tx |
|12|PB.6| |
|13|PB.7| sustain pedal |
|14|LDO| |
|15|VDD| 3.3VDC |
|16|VSS| GND |
|17|VBUS| USB detect |
|18|VDD33| |
|19|D-| USBD- |
|20|D+| USBD+ |
|21|PB.0| keyboard press group 7 |
|22|PB.1| keyboard detect group 7 |
|23|PB.2| keyboard press group 8 |
|24|PB.3| keyboard detect group 8 |
|25|PC.3| keyboard detect group 2  - keypad col 2 |
|26|PC.2| keyboard press group 2 |
|27|PC.1| keyboard detect group 1 - keypad col 1 |
|28|PC.0| keyboard press group 1 |
|29|PE.5| led octave up a |
|30|PB.11| keyboard key 4 |
|31|PB.10| keyboard key 3 |
|32|PB.9| keyboard key 2 |
|33|PC.11| keyboard detect group 5 |
|34|PC.10| keyboard press group 5 |
|35|PC.9| keyboard detect group 4 - keypad col 4 |
|36|PC.8| keyboard press group 4 |
|37|PA.15| keyboard detect group 11 |
|38|PA.14| keyboard press group 11 |
|39|PA.13| keyboard detect group 10 |
|40|PA.12| keyboard press group 10 |
|41|ICE_DAT| programming data pin |
|42|ICE_CLK| programming clock pin |
|43|AVSS| GND |
|44|PA.0| volume |
|45|PA.1| pitch |
|46|PA.2| modulation |
|47|PA.3| expression pedal |
|48|PA.4| led advanced |
|49|PA.5| led octave up b |
|50|PA.6| led octave down a |
|51|PA.7| led octave down b |
|52|AVDD| analog power |
|53|PC.7| keyboard detect group 3 - keypad col 3 |
|54|PC.6| keyboard press group 3 |
|55|PC.15| keyboard detect group 6 |
|56|PC.14| keyboard press group 6 |
|57|PB.15| keyboard key 8 |
|58|PF.0| |
|59|PF.1| |
|60|/RESET| RESET |
|61|VSS| GND |
|62|VDD| 3.3VDC |
|63|PVSS| |
|64|PB.8| keyboard key 1 |

