// SPDX-FileCopyrightText: 2019-2022, Giovanni Dante Grazioli <wargio@libero.it>
// SPDX-FileCopyrightText: 2019, Giovanni Dante Grazioli <gda@nbs-system.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <ngx_config.h>

#include <naxsi.h>
#include <naxsi_macros.h>
#include <naxsi_net.h>

static int
parse_ipv6(const ngx_str_t* nx_ip, ip_t* ip, char* ip_str)
{
  struct in6_addr ipv6                       = { .s6_addr = { 0 } };
  char            addr[INET6_ADDRSTRLEN + 1] = { 0 };
  size_t          addr_len = (nx_ip->len > INET6_ADDRSTRLEN) ? INET6_ADDRSTRLEN : nx_ip->len;
  memcpy(addr, nx_ip->data, addr_len);

  if (inet_pton(AF_INET6, addr, &ipv6) != 1) {
    return 0;
  }

  if (ip) {
    ip->version = IPv6;
    // ipv6 hi
    ip->u.v6[0] = ipv6.s6_addr[0];
    ip->u.v6[0] = (ip->u.v6[0] << 8) | ipv6.s6_addr[1];
    ip->u.v6[0] = (ip->u.v6[0] << 8) | ipv6.s6_addr[2];
    ip->u.v6[0] = (ip->u.v6[0] << 8) | ipv6.s6_addr[3];
    ip->u.v6[0] = (ip->u.v6[0] << 8) | ipv6.s6_addr[4];
    ip->u.v6[0] = (ip->u.v6[0] << 8) | ipv6.s6_addr[5];
    ip->u.v6[0] = (ip->u.v6[0] << 8) | ipv6.s6_addr[6];
    ip->u.v6[0] = (ip->u.v6[0] << 8) | ipv6.s6_addr[7];

    // ipv6 low
    ip->u.v6[1] = ipv6.s6_addr[8];
    ip->u.v6[1] = (ip->u.v6[1] << 8) | ipv6.s6_addr[9];
    ip->u.v6[1] = (ip->u.v6[1] << 8) | ipv6.s6_addr[10];
    ip->u.v6[1] = (ip->u.v6[1] << 8) | ipv6.s6_addr[11];
    ip->u.v6[1] = (ip->u.v6[1] << 8) | ipv6.s6_addr[12];
    ip->u.v6[1] = (ip->u.v6[1] << 8) | ipv6.s6_addr[13];
    ip->u.v6[1] = (ip->u.v6[1] << 8) | ipv6.s6_addr[14];
    ip->u.v6[1] = (ip->u.v6[1] << 8) | ipv6.s6_addr[15];
  }

  if (ip_str) {
    inet_ntop(AF_INET6, &ipv6, ip_str, INET6_ADDRSTRLEN);
  }
  return 1;
}

static int
parse_ipv4(const ngx_str_t* nx_ip, ip_t* ip, char* ip_str)
{
  struct in_addr ipv4                       = { .s_addr = 0 };
  char           addr[INET6_ADDRSTRLEN + 1] = { 0 };
  size_t         addr_len = (nx_ip->len > INET_ADDRSTRLEN) ? INET_ADDRSTRLEN : nx_ip->len;
  memcpy(addr, nx_ip->data, addr_len);

  if (inet_pton(AF_INET, addr, &ipv4) != 1) {
    return 0;
  }

  if (ip) {
    ip->version = IPv4;
    ip->u.v4    = htonl(ipv4.s_addr);
  }

  if (ip_str) {
    inet_ntop(AF_INET, &ipv4, ip_str, INET_ADDRSTRLEN);
  }
  return 1;
}

int
naxsi_parse_ip(const ngx_str_t* nx_ip, ip_t* ip, char* ip_str)
{
  if (strnchr((const char*)nx_ip->data, ':', nx_ip->len) != NULL) {
    return parse_ipv6(nx_ip, ip, ip_str);
  }
  return parse_ipv4(nx_ip, ip, ip_str);
}

int /*cidr_error_t*/
naxsi_parse_cidr(const ngx_str_t* nx_cidr, cidr_t* cidr)
{
  ngx_str_t copy  = *nx_cidr;
  char*     smask = strnchr((const char*)copy.data, '/', copy.len);
  if (!smask) {
    return (CIDR_ERROR_MISSING_MASK);
  }

  ip_t   ip       = { 0 };
  size_t orig_len = copy.len;

  copy.len = smask - (const char*)copy.data;
  int ret  = naxsi_parse_ip(&copy, &ip, NULL);
  copy.len = orig_len;
  if (!ret) {
    return (CIDR_ERROR_INVALID_IP_NET);
  }

  unsigned int mask = atoi(smask + 1);
  if ((ip.version == IPv6 && mask > 128) || (ip.version == IPv4 && mask > 32) || mask == 0) {
    return (CIDR_ERROR_INVALID_CIDR_MASK);
  }
  if ((ip.version == IPv6 && mask > 128) || (ip.version == IPv4 && mask > 32) || mask == 0) {
    return (CIDR_ERROR_INVALID_CIDR_MASK);
  }

  cidr->subnet = ip;
  /* Generate IPv[46] mask (optimized) */
  if (ip.version == IPv6) {
    if (mask > 63) {
      cidr->mask.u.v6[0] = U64_MAX;
      cidr->mask.u.v6[1] = U64_MAX << (128 - mask);
    } else {
      cidr->mask.u.v6[0] = U64_MAX << (64 - mask);
      cidr->mask.u.v6[1] = 0;
    }
  } else {
    cidr->mask.u.v4 = U32_MAX << (32 - mask);
  }

  return CIDR_OK;
}

int
naxsi_is_in_subnet(const cidr_t* cidr, const ip_t* ip)
{
  if (cidr->subnet.version != ip->version) {
    return 0;
  }

  if (cidr->subnet.version == IPv4) {
    return (ip->u.v4 & cidr->mask.u.v4) == (cidr->subnet.u.v4 & cidr->mask.u.v4);
  }

  return (ip->u.v6[0] & cidr->mask.u.v6[0]) == (cidr->subnet.u.v6[0] & cidr->mask.u.v6[0]) &&
         (ip->u.v6[1] & cidr->mask.u.v6[1]) == (cidr->subnet.u.v6[1] & cidr->mask.u.v6[1]);
}
