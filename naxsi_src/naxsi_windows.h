// SPDX-FileCopyrightText: 2022, Alex <alex@staticlibs.net>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef __NAXSI_WINDOWS_H__
#define __NAXSI_WINDOWS_H__

#include <sys/timeb.h>
#include <sys/types.h>
#include <winsock2.h>

int
gettimeofday(struct timeval* t, void* timezone);

// from linux's sys/times.h

//#include <features.h>

#define __need_clock_t
#include <time.h>

#define strncasecmp _strnicmp
#define strcasecmp  _stricmp

// From http://www.linuxjournal.com/article/5574

/* Structure describing CPU time used by a process and its children.  */
struct tms
{
  clock_t tms_utime; /* User CPU time.  */
  clock_t tms_stime; /* System CPU time.  */

  clock_t tms_cutime; /* User CPU time of dead children.  */
  clock_t tms_cstime; /* System CPU time of dead children.  */
};

/* Store the CPU time used by this process and all its
   dead children (and their dead children) in BUFFER.
   Return the elapsed real time, or (clock_t) -1 for errors.
   All times are in CLK_TCKths of a second.  */
clock_t
times(struct tms* __buffer);

typedef long long suseconds_t;

// From https://stackoverflow.com/a/20816961

int
inet_pton(int af, const char* src, void* dst);

const char*
inet_ntop(int af, const void* src, char* dst, socklen_t size);

#endif // __NAXSI_WINDOWS_H__