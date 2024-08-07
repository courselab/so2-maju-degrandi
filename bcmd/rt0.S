#    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
#    SPDX-FileCopyrightText: 2024 maju-degrandi <maju.degrandi@usp.br>
#   
#    SPDX-License-Identifier: GPL-3.0-or-later
#
#  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
#  and contains modifications carried out by the following author(s):
#  maju-degrandi <maju.degrandi@usp.br>

	.code16			/* Select 16-bit code.                    */
	.global _start		/* This will be the program entry point.  */
	
	.text
_start:

	cli			/* Disable interruptions.                 */
        ljmp $0x0,$init0	/* Canonicalize %cs:%ip to 0000:7c000     */
init0:                               
        xorw %ax, %ax		/* Zero all other segement registers.     */
        movw %ax, %ds           
        movw %ax, %es           
        movw %ax, %fs                
        movw %ax, %gs                
        movw %ax, %ss                
        mov $0x7c00, %sp      	/* Set the stack right bellow the program.*/
        sti			/* Reenable interruptions.                */
        call main		/* Call main().                           */
 halt:				/* Upon main()'s return, halt.            */
        hlt                          
        jmp halt                     

