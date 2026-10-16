/* ============================================================================
 *   _______             ____                  __  ____
 *  /_  __(_)__  __ __  / __/__  __ _____  ___/ / / __/__  __ _____________
 *   / / / / _ \/ // / _\ \/ _ \/ // / _ \/ _  / _\ \/ _ \/ // / __/ __/ -_)
 *  /_/ /_/_//_/\_, / /___/\___/\_,_/_//_/\_,_/ /___/\___/\_,_/_/  \__/\__/
 *             /___/
 *  Small program for MS-DOS to power on/off Disney Sound Source
 *  Part of Tiny Sound Source project.
 *
 *  Written in Borland Turbo C v2.01
 *  Uses FAR pointer, so the LARGE model is required.
 *
 *  ver: 1.0
 *  author: 13chuck13
 *  date: 11.10.2026
 * ---------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <dos.h>


typedef unsigned int uint;


uint get_lpt1_base(void);


int main(int argc, char **argv)
{
    uint lpt_base;

    if(  argc != 2 || \
        (strcmp(argv[1], "on") && \
         strcmp(argv[1], "off")))
        {
            printf("\n -= Disney Sound Source power control utility =-\n");
			printf("        Part of Tiny Sound Source project\n");
			printf("           v1.0  13chuck13  11.10.2026\n\n");
            printf(" Usage:\n   dss-ctl.exe on\n   dss-ctl.exe off\n");
            return 1;
        }

    lpt_base = get_lpt1_base();
    if(!lpt_base)
    {
        printf(" Oops! LPT1 base address not found.\n");
        return 1;
    }
    printf(" LPT1 base address: 0x%X\n", lpt_base);

    if( !strcmp(argv[1], "on"))
        {
            printf(" Turning ON Disney Sound Source.\n");
            outportb(lpt_base + 2, 0x00);
            return 0;
        }
        else if( !strcmp(argv[1], "off"))
        {
            printf(" Turning OFF Disney Sound Source.\n");
            outportb(lpt_base + 2, 0x08);
            return 0;
        }

	printf(" WTF?! How is this even possible?\n");
    return 0;
}


uint get_lpt1_base(void)
{
    uint *bios_lpt_table;

    bios_lpt_table = (uint far *) MK_FP(0x0040, 0x0008);

    return bios_lpt_table[0];
}
