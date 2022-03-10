// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_INET_H
#define NAXSI_INET_H

#include <naxsi_types.h>
#include "naxsi_mem.h"

typedef enum {
	NAXSI_IN_FAMILY_UNKNOWN = 0, ///< unknown inet type. used only for errors and to declare an uninitialized naxsi_inet_t
	NAXSI_IN_FAMILY_IPv6, ///< IPv6
	NAXSI_IN_FAMILY_IPv4, ///< IPv4
} naxsi_in_family_t;

typedef struct naxsi_inet {
	naxsi_in_family_t family;
	union {
		u64 addr6[2];
		u32 addr4;
	} in;
} naxsi_inet_t;

typedef struct naxsi_subnet {
	naxsi_in_family_t family;
	union {
		u64 addr6[2];
		u32 addr4;
	} submask;
	union {
		u64 addr6[2];
		u32 addr4;
	} subnet;
} naxsi_subnet_t;

naxsi_subnet_t *naxsi_subnet_new(const naxsi_mem_t *memory, naxsi_str_t *str_subnet);
#define naxsi_subnet_free(memory, subnet) naxsi_mem_free(memory, subnet)
naxsi_inet_t *naxsi_inet_new(const naxsi_mem_t *memory, naxsi_str_t *str_addr);
#define naxsi_inet_free(memory, inet) naxsi_mem_free(memory, inet)
bool naxsi_subnet_match(const naxsi_subnet_t *cidr, const naxsi_inet_t *inet);

#endif /* NAXSI_INET_H */
