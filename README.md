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
|1|PB.14|                        |
|2|PB.13|      |
|3|PB.12|    |
|4|X32O|   |
|5|X32I|           |
|6|PA.11|           |
|7|PA.10|                   |
|8|PA.9|                     |
|9|PA.8|                  |
|10|PB.4|         |
|11|PB.5|              |
|12|PB.6|              |
|13|PB.7|           |
|14|LDO|                  |
|15|VDD|                  |
|16|VSS| GND                  |
|17|VBUS|                  |
|18|VDD33|       |
|19|D-| USBD-      |
|20|D+| USBD+       |
|21|PB.0|     |
|22|PB.1|   |
|23|PB.2|                     |
|24|PB.3|                   |
|25|PC.3|     |
|26|PC.2|   |
|27|PC.1|     |
|28|PC.0|   |
|29|PE.5|             |
|30|PB.11|                        |
|31|PB.10|                        |
|32|PB.9|                   |
|33|PC.11|                  |
|34|PC.10|                   |
|35|PC.9|                    |
|36|PC.8|                   |
|37|PA.15|                   |
|38|PA.14|  |
|39|PA.13|        |
|40|PA.12|       |
|41|ICE_DAT|       |
|42|ICE_CLK|        |
|43|AVSS|        |
|44|PA.0|                        |
|45|PA.1|     |
|46|PA.2|   |
|47|PA.3|                    |
|48|PA.4|                    |
|49|PA.5|                   |
|50|PA.6|                   |
|51|PA.7|                   |
|52|AVDD|                  |
|53|PC.7|                   |
|54|PC.6|                   |
|55|PC.15|                   |
|56|PC.14|              |
|57|PB.15|                  |
|58|PF.0|                   |
|59|PF.1|                   |
|60|/RESET| RESET                   |
|61|VSS| GND                 |
|62|VDD|                  |
|63|PVSS|                   |
|64|PB.8|                    |

