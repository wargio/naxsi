// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include <naxsi.h>
#include "naxsi_list.h"
#include "naxsi_rule.h"
#include "naxsi_inet.h"
#include "naxsi_mem.h"

typedef struct naxsi {
	naxsi_list_t /*<naxsi_rule_t*>*/ *rules; ///< Rules to check
	naxsi_list_t /*<naxsi_whitelist_t*>*/ *whitelist; ///<  Whitelist to check after rule match
	naxsi_list_t /*<naxsi_inet_t*>*/ *ignore_ips; ///< IPs to ignore
	naxsi_list_t /*<naxsi_subnet_t*>*/ *ignore_cidrs; ///< CIDR to use to ignore IPs
} naxsi_t;

/**
 * @brief      Allocates and initializes a naxsi_t structure
 *
 * @param[in]  memory  The naxsi_mem_t structure to use for handling memory
 *
 * @return     NULL if fails to allocate, otherwise non NULL.
 */
NAXSI_API naxsi_t *naxsi_new(const naxsi_mem_t *memory) {
	if (!memory) {
		return NULL;
	}

	naxsi_t *nxs = naxsi_mem_new(memory, naxsi_t);
	if (!nxs) {
		return NULL;
	}

	nxs->rules = naxsi_list_new(memory, (naxsi_free_t)&naxsi_rule_free);
	nxs->whitelist = naxsi_list_new(memory, (naxsi_free_t)&naxsi_whitelist_free);
	nxs->ignore_ips = naxsi_list_new(memory, &naxsi_mem_free);
	nxs->ignore_cidrs = naxsi_list_new(memory, &naxsi_mem_free);
	if (!nxs->rules || !nxs->whitelist || !nxs->ignore_ips || !nxs->ignore_cidrs) {
		naxsi_free(memory, nxs);
		return NULL;
	}

	return nxs;
}

/**
 * @brief      Finalize and frees a naxsi_t pointer.
 *
 * @param[in]  memory  The naxsi_mem_t structure to use for handling memory
 * @param      nxs     The naxsi_t to free.
 */
NAXSI_API void naxsi_free(const naxsi_mem_t *memory, naxsi_t *nxs) {
	if (!memory || !nxs) {
		return;
	}
	naxsi_list_free(memory, nxs->rules);
	naxsi_list_free(memory, nxs->whitelist);
	naxsi_list_free(memory, nxs->ignore_ips);
	naxsi_list_free(memory, nxs->ignore_cidrs);
	naxsi_mem_free(memory, nxs);
}

/**
 * @brief      Adds an IP address to the ignore ip list (i.e. whitelisting IP addresses)
 *
 * @param[in]  memory  The naxsi_mem_t structure to use for handling memory
 * @param      nxs         The naxsi_t struct to use
 * @param      ip_address  The ip address to whitelist
 *
 * @return     On success returns true, on error false.
 */
NAXSI_API bool naxsi_ignore_ip(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *ip_address) {
	if (!memory || !nxs || !ip_address) {
		return false;
	}
	naxsi_inet_t *inet = naxsi_inet_new(memory, ip_address);
	if (!inet || !naxsi_list_append(memory, nxs->ignore_ips, inet)) {
		naxsi_inet_free(memory, inet);
		return false;
	}
	return true;
}

/**
 * @brief      Adds an CIDR range to the ignore subnet list (i.e. whitelisting CIDR ranges)
 *
 * @param[in]  memory  The naxsi_mem_t structure to use for handling memory
 * @param      nxs     The naxsi_t struct to use
 * @param      cidr    The ip address to whitelist
 *
 * @return     On success returns true, on error false.
 */
NAXSI_API bool naxsi_ignore_cidr(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *cidr) {
	if (!memory || !nxs || !cidr) {
		return false;
	}
	naxsi_subnet_t *subnet = naxsi_subnet_new(memory, cidr);
	if (!subnet || !naxsi_list_append(memory, nxs->ignore_cidrs, subnet)) {
		naxsi_subnet_free(memory, subnet);
		return false;
	}
	return true;
}