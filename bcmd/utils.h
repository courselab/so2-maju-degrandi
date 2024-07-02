/*
 *    SPDX-FileCopyrightText: 2024 Monaco F. J. <monaco@usp.br>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */

#ifndef UTILS_H
#define UTILS_H

int __attribute__((fastcall)) strcmp(const char *s1, const char *s2, int n);
int __attribute__((fastcall)) to_int(const char *str);

#endif  /* UTILS_H  */
