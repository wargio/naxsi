// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_inet.h"
#include "naxsi_str.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(__FreeBSD__) || defined(__OpenBSD__)
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
#include <arpa/inet.h>

#define NAXSI_INET6_HI 0
#define NAXSI_INET6_LO 1

/**
 * @brief      converts buffer to u64 (big endian)
 *
 * @param      buffer  The buffer to use
 *
 * @return     the parsed number
 */
static u64 to_u64(u8 *buffer) {
	u64 result = ((u64)buffer[0]) << 56;
	result |= ((u64)buffer[1]) << 48;
	result |= ((u64)buffer[2]) << 40;
	result |= ((u64)buffer[3]) << 32;
	result |= ((u64)buffer[4]) << 24;
	result |= ((u64)buffer[5]) << 16;
	result |= ((u64)buffer[6]) << 8;
	result |= ((u64)buffer[7]);
	return result;
}

/**
 * @brief      Initializes a naxsi_subnet_t by parsing a subnet in ipv6 format with a net mask
 *
 * @param[in]  memory    The naxsi_mem_t structure to use
 * @param      cidr      The naxsi_subnet_t structure to initialize
 * @param[in]  subnet    The subnet to parse
 * @param[in]  net_mask  The net mask to apply
 *
 * @return     true when succeeds to parse the subnet with a valid net mask, else false
 */
static bool naxsi_subnet6_init(const naxsi_mem_t *memory, naxsi_subnet_t *cidr, const char *subnet, u32 net_mask) {
	if (net_mask > 128) {
		return false;
	}
	struct in6_addr in6addr = { 0 };
	if (inet_pton(AF_INET6, subnet, &in6addr) != 1) {
		return false;
	}

	cidr->family = NAXSI_IN_FAMILY_IPv6;
	if (net_mask > 63) {
		cidr->submask.addr6[NAXSI_INET6_HI] = U64_MAX;
		cidr->submask.addr6[NAXSI_INET6_LO] = U64_MAX << (128 - net_mask);
	} else {
		cidr->submask.addr6[NAXSI_INET6_HI] = U64_MAX << (64 - net_mask);
		cidr->submask.addr6[NAXSI_INET6_LO] = 0;
	}

	cidr->subnet.addr6[NAXSI_INET6_HI] = to_u64(in6addr.s6_addr) & cidr->submask.addr6[NAXSI_INET6_HI];
	cidr->subnet.addr6[NAXSI_INET6_LO] = to_u64(in6addr.s6_addr + 8) & cidr->submask.addr6[NAXSI_INET6_LO];
	return true;
}

/**
 * @brief      Initializes a naxsi_subnet_t by parsing a subnet in ipv4 format with a net mask
 *
 * @param[in]  memory    The naxsi_mem_t structure to use
 * @param      cidr      The naxsi_subnet_t structure to initialize
 * @param[in]  subnet    The subnet to parse
 * @param[in]  net_mask  The net mask to apply
 *
 * @return     true when succeeds to parse the subnet with a valid net mask, else false
 */
static bool naxsi_subnet4_init(const naxsi_mem_t *memory, naxsi_subnet_t *cidr, const char *subnet, u32 net_mask) {
	if (net_mask > 32) {
		return false;
	}
	struct in_addr in4addr = { 0 };
	if (inet_pton(AF_INET, subnet, &in4addr) != 1) {
		return false;
	}

	cidr->family = NAXSI_IN_FAMILY_IPv4;
	cidr->submask.addr4 = U32_MAX << (32 - net_mask);
	cidr->subnet.addr4 = htonl(in4addr.s_addr) & cidr->submask.addr4;
	return true;
}

/**
 * @brief      Allocates and initializes a naxsi_subnet_t struct using a subnet in CIDR format
 *
 * @param[in]  memory      The naxsi_mem_t structure to use
 * @param      str_subnet  The subnet in CIDR format to parse
 *
 * @return     On success returns a naxsi_subnet_t, otherwise NULL
 */
naxsi_subnet_t *naxsi_subnet_new(const naxsi_mem_t *memory, naxsi_str_t *str_subnet) {
	if (!memory || !str_subnet) {
		return NULL;
	}
	u32 net_mask = 0;
	char *slash = NULL;
	char buffer[INET6_ADDRSTRLEN + 4];

	snprintf(buffer, sizeof(buffer), "%s", str_subnet->data);
	if (!(slash = strchr(buffer, '/'))) {
		return NULL;
	}
	*slash = 0;
	net_mask = atoi(slash + 1);
	if (net_mask < 1) {
		return NULL;
	}

	naxsi_subnet_t *cidr = naxsi_mem_new(memory, naxsi_subnet_t);
	if (!cidr) {
		return NULL;
	}

	if (strchr(buffer, ':')) {
		if (naxsi_subnet6_init(memory, cidr, buffer, net_mask)) {
			return cidr;
		}
	} else if (naxsi_subnet4_init(memory, cidr, buffer, net_mask)) {
		return cidr;
	}

	naxsi_mem_free(memory, cidr);
	return NULL;
}

/**
 * @brief      Initializes a naxsi_inet_t by parsing an ipv6 address
 *
 * @param[in]  memory    The naxsi_mem_t structure to use
 * @param      inet      The naxsi_inet_t to initialize
 * @param      str_addr  The address to parse
 *
 * @return     On success returns true, otherwise false
 */
static bool naxsi_inet6_init(const naxsi_mem_t *memory, naxsi_inet_t *inet, naxsi_str_t *str_addr) {
	struct in6_addr in6addr = { 0 };
	if (inet_pton(AF_INET6, str_addr->data, &in6addr) != 1) {
		return false;
	}

	inet->family = NAXSI_IN_FAMILY_IPv6;
	inet->in.addr6[NAXSI_INET6_HI] = to_u64(in6addr.s6_addr);
	inet->in.addr6[NAXSI_INET6_LO] = to_u64(in6addr.s6_addr + 8);
	// according to RFC :: and ff00::/8 are illegal addresses
	if (inet->in.addr6[NAXSI_INET6_HI] == 0 && inet->in.addr6[NAXSI_INET6_LO] == 0) {
		return false;
	} else if (inet->in.addr6[NAXSI_INET6_HI] >= 0xff00000000000000ull) {
		return false;
	}
	return true;
}

/**
 * @brief      Initializes a naxsi_inet_t by parsing an ipv4 address
 *
 * @param[in]  memory    The naxsi_mem_t structure to use
 * @param      inet      The naxsi_inet_t to initialize
 * @param      str_addr  The address to parse
 *
 * @return     On success returns true, otherwise false
 */
static bool naxsi_inet4_init(const naxsi_mem_t *memory, naxsi_inet_t *inet, naxsi_str_t *str_addr) {
	struct in_addr in4addr = { 0 };
	if (inet_pton(AF_INET, str_addr->data, &in4addr) != 1) {
		return false;
	}

	inet->family = NAXSI_IN_FAMILY_IPv4;
	inet->in.addr4 = htonl(in4addr.s_addr);
	// according to RFC 0.0.0.0, 240.0.0.0-255.255.255.254, 255.255.255.255
	// are illegal addresses
	return inet->in.addr4 > 0 && inet->in.addr4 < 0xf0000000;
}

/**
 * @brief      Allocates and initializes a naxsi_inet_t struct using an IP address
 *
 * @param[in]  memory    The naxsi_mem_t structure to use
 * @param      str_addr  The address string to parse
 *
 * @return     On success returns a naxsi_inet_t, otherwise NULL
 */
naxsi_inet_t *naxsi_inet_new(const naxsi_mem_t *memory, naxsi_str_t *str_addr) {
	if (!memory || !str_addr) {
		return NULL;
	}

	naxsi_inet_t *inet = naxsi_mem_new(memory, naxsi_inet_t);
	if (!inet) {
		return NULL;
	}

	if (strchr(str_addr->data, ':')) {
		if (naxsi_inet6_init(memory, inet, str_addr)) {
			return inet;
		}
	} else if (naxsi_inet4_init(memory, inet, str_addr)) {
		return inet;
	}

	naxsi_mem_free(memory, inet);
	return NULL;
}

/**
 * @brief      Checks if a given address is part of a given subnet
 *
 * @param[in]  cidr  The CIDR subnet to be part of
 * @param[in]  inet  The address to check
 *
 * @return     On success returns a true, otherwise false
 */
bool naxsi_subnet_match(const naxsi_subnet_t *cidr, const naxsi_inet_t *inet) {
	if (!cidr || !inet || cidr->family != inet->family) {
		return false;
	} else if (cidr->family == NAXSI_IN_FAMILY_IPv4) {
		u32 subnet = inet->in.addr4 & cidr->submask.addr4;
		return subnet == cidr->subnet.addr4;
	}

	u64 subnet_hi = inet->in.addr6[NAXSI_INET6_HI] & cidr->submask.addr6[NAXSI_INET6_HI];
	u64 subnet_lo = inet->in.addr6[NAXSI_INET6_LO] & cidr->submask.addr6[NAXSI_INET6_LO];
	return subnet_hi == cidr->subnet.addr6[NAXSI_INET6_HI] && subnet_lo == cidr->subnet.addr6[NAXSI_INET6_LO];
}
