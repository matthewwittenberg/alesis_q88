# Alesis Q88
Alesis Q88 - Reverse Engineer Project
## About
TBD
## Tools
(only built on Windows thus far)
1. [NuEclipse](https://www.nuvoton.com/tool-and-software/ide-and-compiler/)
2. Hardware debugger
   1. [Nu-Link-Pro](https://www.nuvoton.com/tool-and-software/debugger-and-programmer/1-to-1-debugger-and-programmer/nu-link-pro/)   
3. 2mm single row pin header (4 pin)
4. Female-female jumper wires to connect main PCB to J-Link
5. Solder iron
## Prep
TBD
## Building
1. The Alesis Q88 is built on an [NUC120RD2DN](https://www.nuvoton.com/products/microcontrollers/arm-cortex-m0-mcus/nuc120-122-123-220-usb-series/nuc120rd2dn/).

## Misc
1. Each key on the keyboard was designed with a 2 stage micro-switch. Just barely pressing the key registers the first actuation, continuing the press motion activates the second stage. Noting the delta time between the actuations represents the key velocity (loudness). The circuit is multiplexed so we need to scan at a high rate.

## Code
1. keyboard.c owns the keyboard task, it scans and processes key presses.

## Peripheral Map
To the best of my knowledge, here is the map of microcontroller pins

| pin | pin name | function               |
| --- | --- |------------------------|
|1|PB.14| keyboard key 7 |
|2|PB.13| keyboard key 6 |
|3|PB.12| keyboard key 5 |
|4|X32O| 12MHz XTAL |
|5|X32I| 12MHz XTAL |
|6|PA.11| keyboard detect group 9 |
|7|PA.10| keyboard press group 9 |
|8|PA.9|                     |
|9|PA.8|                  |
|10|PB.4|         |
|11|PB.5|              |
|12|PB.6|              |
|13|PB.7| sustain pedal |
|14|LDO|                  |
|15|VDD| 3.3VDC |
|16|VSS| GND |
|17|VBUS|                  |
|18|VDD33|       |
|19|D-| USBD- |
|20|D+| USBD+ |
|21|PB.0| keyboard press group 7 |
|22|PB.1| keyboard detect group 7 |
|23|PB.2| keyboard press group 8 |
|24|PB.3| keyboard detect group 8 |
|25|PC.3| keyboard detect group 2 |
|26|PC.2| keyboard press group 2 |
|27|PC.1| keyboard detect group 1 |
|28|PC.0| keyboard press group 1 |
|29|PE.5|             |
|30|PB.11| keyboard key 4 |
|31|PB.10| keyboard key 3 |
|32|PB.9| keyboard key 2 |
|33|PC.11| keyboard detect group 5 |
|34|PC.10| keyboard press group 5 |
|35|PC.9| keyboard detect group 4 |
|36|PC.8| keyboard press group 4 |
|37|PA.15| keyboard detect group 11 |
|38|PA.14| keyboard press group 11 |
|39|PA.13| keyboard detect group 10 |
|40|PA.12| keyboard press group 10 |
|41|ICE_DAT|       |
|42|ICE_CLK|        |
|43|AVSS|        |
|44|PA.0| volume |
|45|PA.1| pitch |
|46|PA.2| modulation |
|47|PA.3| expression pedal |
|48|PA.4|                    |
|49|PA.5|                   |
|50|PA.6|                   |
|51|PA.7|                   |
|52|AVDD|                  |
|53|PC.7| keyboard detect group 3 |
|54|PC.6| keyboard press group 3 |
|55|PC.15| keyboard detect group 6 |
|56|PC.14| keyboard press group 6 |
|57|PB.15| keyboard key 8 |
|58|PF.0|                   |
|59|PF.1|                   |
|60|/RESET| RESET |
|61|VSS| GND |
|62|VDD| 3.3VDC |
|63|PVSS|                   |
|64|PB.8| keyboard key 1 |

