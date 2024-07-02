/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Maria Júlia De Grandi <maju.degrandi@usp.br>
 *
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  Maria Júlia De Grandi <maju.degrandi@usp.br>
 */

int __attribute__((fastcall)) strcmp(const char *s1, const char *s2, int n)
{
  int i = 0;
  while (*s1 && *s2 && *s1 == *s2 && i < n) {
    s1++;
    s2++;
    i++;
  }
  return (*s1 - *s2);
}

int __attribute__((fastcall)) to_int(const char *str) {
    int result = 0;

    while (*str) {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result;
}

