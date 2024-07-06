/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 maju-degrandi <maju.degrandi@usp.br>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  maju-degrandi <maju.degrandi@usp.br>
 */

#include "bios.h"
#include "utils.h"

#define PROMPT "$ "		/* Prompt sign.      */
#define SIZE 20			/* Read buffer size. */

char buffer[SIZE];		/* Read buffer.      */

void make_triang(int n)
{
  for( int i = 1; i <= n; i++)
    println("#", i);
}

int main()
{
  clear(); 
  println("Hi :)", 1);
  while (1)
    {
      print(PROMPT);		/* Show prompt.               */
      readln(buffer);		/* Read use input.            */

      if (buffer[0])		/* Execute built-in command.  */
	    {
        if (!strcmp(buffer,"triang ", 6))
        {
          int n = to_int(&buffer[7]);
          make_triang(n);
        }
	    }
    }
  
  return 0;

}

