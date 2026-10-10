# Tiny Sound Source - KiCad files.
Schematic, PCB layout, and Gerber files.

The circuit has been tested and works correctly on:
- **IBM PS/1 model 2121** 80386SX-20MHz
- **CMP MIG-32 laptop** 80286-12MHz

## Schematic
![Tiny Sound Source](Images/Schematic.png)

 - **R2** та **C5** separate the long SELECT (INT1) signal from the short STROBE (INT0) pulses. Component values ​​may need to be adjusted.
 - **R3** and **C3** are the main PWM output filter.
 - **R4** and **C4** are an additional filter for DSS mode. If you do not need this filter, replace **R4** with a jumper and do not install **C4**.
 - "Compatibility with the FTL Sound Adapter" is simply a jumper wire between pins 12 and 17 of the LPT connector.

## PCB layout
![Tiny Sound Source](Images/PCB.png)

Ready-to-use Gerber files are located in the Gerber subdirectory.
If you wish to make changes and generate your own Gerber files, I recommend using the User.Drawings layer as F.Silkscreen.
