// SPDX-FileCopyrightText: 2022, Alex <alex@staticlibs.net>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <ngx_config.h>

#ifdef _WIN32
#include <naxsi_windows.h>

int
gettimeofday(struct timeval* t, void* timezone)
{
  struct _timeb timebuffer;
  _ftime(&timebuffer);
  t->tv_sec  = timebuffer.time;
  t->tv_usec = 1000 * timebuffer.millitm;
  return 0;
}

clock_t
times(struct tms* __buffer)
{
  __buffer->tms_utime  = clock();
  __buffer->tms_stime  = 0;
  __buffer->tms_cstime = 0;
  __buffer->tms_cutime = 0;
  return __buffer->tms_utime;
}

int
inet_pton(int af, const char* src, void* dst)
{
  struct sockaddr_storage ss;
  int                     size = sizeof(ss);
  char                    src_copy[INET6_ADDRSTRLEN + 1];

  ZeroMemory(&ss, sizeof(ss));
  /* stupid non-const API */
  strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
  src_copy[INET6_ADDRSTRLEN] = 0;

  if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr*)&ss, &size) == 0) {
    switch (af) {
      case AF_INET:
        *(struct in_addr*)dst = ((struct sockaddr_in*)&ss)->sin_addr;
        return 1;
      case AF_INET6:
        *(struct in6_addr*)dst = ((struct sockaddr_in6*)&ss)->sin6_addr;
        return 1;
    }
  }
  return 0;
}

const char*
inet_ntop(int af, const void* src, char* dst, socklen_t size)
{
  struct sockaddr_storage ss;
  unsigned long           s = size;

  ZeroMemory(&ss, sizeof(ss));
  ss.ss_family = af;

  switch (af) {
    case AF_INET:
      ((struct sockaddr_in*)&ss)->sin_addr = *(struct in_addr*)src;
      break;
    case AF_INET6:
      ((struct sockaddr_in6*)&ss)->sin6_addr = *(struct in6_addr*)src;
      break;
    default:
      return NULL;
  }
  /* cannot direclty use &size because of strict aliasing rules */
  return (WSAAddressToString((struct sockaddr*)&ss, sizeof(ss), NULL, dst, &s) == 0) ? dst : NULL;
}

#endif // _WIN32