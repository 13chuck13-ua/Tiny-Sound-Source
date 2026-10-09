# Tiny Sound Source
Tiny Sound Source is an 8-bit LPT DAC compatible with the Covox Speech Thing and Disney Sound Source. It is built around an AVR ATtiny2313 microcontroller and uses a minimal number of additional components.

## Key Features:
 - Automatic switching between Covox and DSS modes, with LED indication.
 - Covox mode: unrestricted sampling rate and protection against errors when reading data from the port.
 - DSS mode: doubled output sampling rate (14 kHz) with simple linear interpolation.
 - An additional low-pass filter, automatically enabled by software in DSS mode.
 - Audio output via the built-in PWM at 62.5 kHz.
 - Compatibility with the FTL Sound Adapter.

## Content
 - The AVR-GCC directory contains code for the microcontroller.
 - The KiCad directory contains the schematic, PCB layout, and Gerber files.

## Tested in the following games:
| Name                  | Sound     | Autodetect  |
| :---                  | :---:     | :---:       |
| Alone in the Dark     | 🟢Ok      | 🟢Ok         |
| Anohter World         | 🟢Ok      | ⚪N/A        |
| Dungeon Master        | 🟢Ok      | 🟢Ok         |
| Fast Doom             | 🟢Ok      | ⚪N/A        |
| Hocus Pocus           | 🟢Ok      | 🔴Not working|
| King`s Quest VI       | 🟢Ok      | 🟢Ok         |
| Prince of Persia v1.3 | 🟢Ok      | ⚪N/A        |
| Rocketeer             | 🟢Ok      | ⚪N/A        |
| Space Quest V         | 🟢Ok      | ⚪N/A        |
| Wolfenstein 3D        | 🟢Ok      | 🟢Ok         |

A quick word about Hocus Pocus.
This game behaves strangely during the DSS auto-detection process. For the device to work correctly, a buffer with TTL input levels needs to be added to the microcontroller's INT0 and INT1 inputs. However, the Tiny Sound Source is, first and foremost, a simple device. I refuse to add a separate chip just for auto-detection in a single game.
To force the use of the Disney Sound Source, use the following command-line switches:
 - -ss1 for LPT at 0x3BC
 - -ss2 for LPT at 0x378
 - -ss3 for LPT at 0x278
