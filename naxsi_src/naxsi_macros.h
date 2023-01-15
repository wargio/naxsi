// SPDX-FileCopyrightText: 2019, Giovanni Dante Grazioli <gda@nbs-system.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NAXSI_MACROS_H
#define NAXSI_MACROS_H

#define is_whitespace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
#define is_printable(c)  ((c) >= ' ' && (c) <= '~')
#define is_numeric(c)    ((c) >= '0' && (c) <= '9')
#define const_len(s)     (sizeof(s) - sizeof(s[0]))
#define array_size(x)    (sizeof(x) / sizeof((x)[0]))

#define U64_MAX 0xffffffffffffffffull
#define U32_MAX 0xffffffffu

#define return_value_if(cond, val)                                                                 \
  if ((cond))                                                                                      \
  return (val)
#define return_void_if(cond)                                                                       \
  if ((cond))                                                                                      \
  return
#define break_if(cond)                                                                             \
  if ((cond))                                                                                      \
  break

#endif /* NAXSI_MACROS_H */
