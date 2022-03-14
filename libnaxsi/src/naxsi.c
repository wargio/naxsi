// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include <naxsi.h>
#include <strings.h>
#include "naxsi_list.h"
#include "naxsi_rule.h"
#include "naxsi_inet.h"
#include "naxsi_mem.h"

#define find_prefix(arg, prefix)  (arg->length > strlen(prefix) && !strncmp(arg->data, prefix, strlen(prefix)))
#define find_keyword(arg, prefix) (arg->length == strlen(prefix) && !strncmp(arg->data, prefix, strlen(prefix)))

typedef struct naxsi {
	naxsi_list_t /*<naxsi_rule_t*>*/ *rules; ///< Rules to check
	naxsi_list_t /*<naxsi_whitelist_t*>*/ *whitelist; ///< Whitelist to check after rule match
	naxsi_list_t /*<naxsi_checkrules_t*>*/ *checkrules; ///< CheckRules
	naxsi_list_t /*<naxsi_inet_t*>*/ *ignore_ips; ///< IPs to ignore
	naxsi_list_t /*<naxsi_subnet_t*>*/ *ignore_cidrs; ///< CIDR to use to ignore IPs
	naxsi_str_t *denied_url;
	bool learning_mode;
	bool security_rules;
	bool libinjection_sql;
	bool libinjection_xss;
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
	nxs->checkrules = naxsi_list_new(memory, (naxsi_free_t)&naxsi_checkrule_free);
	nxs->ignore_ips = naxsi_list_new(memory, &naxsi_mem_free);
	nxs->ignore_cidrs = naxsi_list_new(memory, &naxsi_mem_free);
	if (!nxs->rules || !nxs->whitelist || !nxs->checkrules || !nxs->ignore_ips || !nxs->ignore_cidrs) {
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
	naxsi_list_free(memory, nxs->checkrules);
	naxsi_list_free(memory, nxs->ignore_ips);
	naxsi_list_free(memory, nxs->ignore_cidrs);
	naxsi_str_free(memory, nxs->denied_url, true);
	naxsi_mem_free(memory, nxs);
}

/**
 * @brief      Sets the LearningMode value
 *
 * @param      nxs     The nxs
 * @param[in]  enable  The enable
 *
 * @return     On success returns true, on failure false.
 */
NAXSI_API bool naxsi_set_learning_mode(naxsi_t *nxs, bool enable) {
	if (!nxs) {
		return false;
	}
	nxs->learning_mode = enable;
	return true;
}

/**
 * @brief      Sets the SecRulesEnabled/SecRulesDisabled value
 *
 * @param      nxs     The nxs
 * @param[in]  enable  The enable
 *
 * @return     On success returns true, on failure false.
 */
NAXSI_API bool naxsi_set_security_rules(naxsi_t *nxs, bool enable) {
	if (!nxs) {
		return false;
	}
	nxs->security_rules = enable;
	return true;
}

/**
 * @brief      Sets the LibInjectionSql value
 *
 * @param      nxs     The nxs
 * @param[in]  enable  The enable
 *
 * @return     On success returns true, on failure false.
 */
NAXSI_API bool naxsi_set_libinjection_sql(naxsi_t *nxs, bool enable) {
	if (!nxs) {
		return false;
	}
	nxs->libinjection_sql = enable;
	return true;
}

/**
 * @brief      Sets the LibInjectionXss value
 *
 * @param      nxs     The nxs
 * @param[in]  enable  The enable
 *
 * @return     On success returns true, on failure false.
 */
NAXSI_API bool naxsi_set_libinjection_xss(naxsi_t *nxs, bool enable) {
	if (!nxs) {
		return false;
	}
	nxs->libinjection_xss = enable;
	return true;
}

/**
 * @brief      Sets the DeniedUrl value
 *
 * @param[in]  memory      The naxsi_mem_t structure to use for handling memory
 * @param      nxs         The naxsi_t struct to use
 * @param      denied_url  The url to use to redirect on denied requests
 *
 * @return     On success returns true, on failure false.
 */
NAXSI_API bool naxsi_set_denied_url(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *denied_url) {
	if (!memory || !nxs || !denied_url) {
		return false;
	}
	nxs->denied_url = naxsi_str_new(memory, denied_url->data, denied_url->length, true);
	return nxs->denied_url != NULL;
}

/**
 * @brief      Adds a CheckRule to the naxsi_t structure
 *
 * @param[in]  memory       The naxsi_mem_t structure to use for handling memory
 * @param      nxs          The naxsi_t struct to use
 * @param      checkrule_s  The checkrule
 * @param      action_s     The action to take
 *
 * @return     On success returns true, on failure false.
 */
NAXSI_API bool naxsi_add_checkrule(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *checkrule_s, naxsi_str_t *action_s) {
	if (!memory || !nxs || !checkrule_s || !action_s) {
		return false;
	}
	naxsi_checkrule_t *checkrule = naxsi_checkrule_new(memory, checkrule_s, action_s);
	if (!checkrule || !naxsi_list_append(memory, nxs->checkrules, checkrule)) {
		naxsi_checkrule_free(memory, checkrule);
		return false;
	}
	return true;
}

/**
 * @brief      Adds a new rule to naxsi_t structure, based on the given args
 *
 * @param[in]  memory  The naxsi_mem_t structure to use for handling memory
 * @param      nxs     The naxsi_t struct to use
 * @param[in]  argc    The count of the rule arguments
 * @param      argv    The rule arguments array
 *
 * @return     On success returns true, on failure false.
 */
NAXSI_API bool naxsi_add_rule_args(const naxsi_mem_t *memory, naxsi_t *nxs, size_t argc, naxsi_str_t *argv) {
	if (!memory || !nxs || argc < 4 || !argv) {
		return false;
	}
	naxsi_str_t *id_s = NULL, *descr_s = NULL, *match_s = NULL, *matchzone_s = NULL, *scores_s = NULL, *whitelist_s = NULL;
	bool negative = false;

	for (size_t i = 0; i < argc; ++i) {
		naxsi_str_t *arg = &argv[i];
		if (find_prefix(arg, NAXSI_RULE_PREFIX_IDENTIFIER)) {
			if (id_s) {
				return false;
			}
			id_s = arg;
			continue;
		} else if (find_prefix(arg, NAXSI_RULE_PREFIX_DESCRIPTION)) {
			if (descr_s) {
				return false;
			}
			descr_s = arg;
			continue;
		} else if (find_prefix(arg, NAXSI_RULE_PREFIX_MATCH_STRING)) {
			if (match_s) {
				return false;
			}
			match_s = arg;
			continue;
		} else if (find_prefix(arg, NAXSI_RULE_PREFIX_MATCH_REGEX)) {
			if (match_s) {
				return false;
			}
			match_s = arg;
			continue;
		} else if (find_prefix(arg, NAXSI_RULE_PREFIX_MATCHZONE)) {
			if (matchzone_s) {
				return false;
			}
			matchzone_s = arg;
			continue;
		} else if (find_prefix(arg, NAXSI_RULE_PREFIX_SCORES)) {
			if (scores_s) {
				return false;
			}
			scores_s = arg;
			continue;
		} else if (find_prefix(arg, NAXSI_RULE_PREFIX_WHITELIST)) {
			if (whitelist_s) {
				return false;
			}
			whitelist_s = arg;
			continue;
		} else if (find_keyword(arg, NAXSI_RULE_NEGATIVE)) {
			if (negative) {
				return false;
			}
			negative = true;
			continue;
		}
	}

	if (whitelist_s) {
		naxsi_whitelist_t *whitelist = naxsi_whitelist_new(memory, whitelist_s, matchzone_s, negative);
		if (!whitelist || !naxsi_list_append(memory, nxs->whitelist, whitelist)) {
			naxsi_whitelist_free(memory, whitelist);
			return false;
		}
	} else {
		naxsi_rule_t *rule = naxsi_rule_new(memory, id_s, descr_s, match_s, matchzone_s, scores_s, negative);
		if (!rule || !naxsi_list_append(memory, nxs->rules, rule)) {
			naxsi_rule_free(memory, rule);
			return false;
		}
	}

	return true;
}

/**
 * @brief      Adds an IP address to the ignore ip list (i.e. whitelisting IP addresses)
 *
 * @param[in]  memory      The naxsi_mem_t structure to use for handling memory
 * @param      nxs         The naxsi_t struct to use
 * @param      ip_address  The ip address to whitelist
 *
 * @return     On success returns true, on error false.
 */
NAXSI_API bool naxsi_add_ignore_ip(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *ip_address) {
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
NAXSI_API bool naxsi_add_ignore_cidr(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *cidr) {
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