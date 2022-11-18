// SPDX-FileCopyrightText: 2019, Giovanni Dante Grazioli <gda@nbs-system.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef __NAXSI_NET_H__
#define __NAXSI_NET_H__

#include <naxsi.h>

#if defined(__FreeBSD__) || defined(__OpenBSD__)
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#ifndef _WIN32
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif // !_WIN32
#include <stdint.h>
#include <string.h>

typedef enum
{
  CIDR_OK = 0,
  CIDR_ERROR_MISSING_MASK,
  CIDR_ERROR_INVALID_IP_NET,
  CIDR_ERROR_INVALID_CIDR_MASK,
} cidr_error_t;

typedef enum
{
  IPv4 = 0,
  IPv6
} ip_type_t;

typedef struct
{
  union
  {
    uint64_t v6[2];
    uint32_t v4;
  };
  uint32_t version;
} ip_t;

typedef struct
{
  ip_t mask;
  ip_t subnet;
} cidr_t;

int
naxsi_parse_ip(const ngx_str_t* nx_ip, ip_t* ip, char* ip_str);

int /*cidr_error_t*/
naxsi_parse_cidr(const ngx_str_t* nx_cidr, cidr_t* cidr);

int
naxsi_is_in_subnet(const cidr_t* cidr, const ip_t* ip);

#endif /* __NAXSI_NET_H__ */
