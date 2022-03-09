// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_rule.h"

naxsi_svector_define(scores, naxsi_score_t);
naxsi_vector_define(ids, u32);

/**
 * @brief      Frees naxsi_matchzone_t contents
 *
 * @param      memory     The naxsi_mem_t structure to use
 * @param      matchzone  The naxsi_matchzone_t to contents free
 */
static void matchzone_fini(const naxsi_mem_t *memory, naxsi_matchzone_t *matchzone) {
	if (matchzone->args == NAXSI_MZ_TYPE_STRING) {
		naxsi_str_free(memory, matchzone->args_val, true);
	} else if (matchzone->args == NAXSI_MZ_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)matchzone->args_val);
	}
	if (matchzone->headers == NAXSI_MZ_TYPE_STRING) {
		naxsi_str_free(memory, matchzone->headers_val, true);
	} else if (matchzone->headers == NAXSI_MZ_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)matchzone->headers_val);
	}
	if (matchzone->url == NAXSI_MZ_TYPE_STRING) {
		naxsi_str_free(memory, matchzone->url_val, true);
	} else if (matchzone->url == NAXSI_MZ_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)matchzone->url_val);
	}
}

/**
 * @brief      Frees a naxsi_rule_t pointer
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param      rule    The naxsi_rule_t to be freed
 */
void naxsi_rule_free(const naxsi_mem_t *memory, naxsi_rule_t *rule) {
	if (!memory || !rule) {
		return;
	}
	naxsi_str_free(memory, rule->description, true);
	naxsi_scores_free(memory, rule->scores);
	if (rule->mtype == NAXSI_MATCH_TYPE_STRING) {
		naxsi_str_free(memory, (naxsi_str_t *)rule->match, true);
	} else if (rule->mtype == NAXSI_MATCH_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)rule->match);
	}
	matchzone_fini(memory, &rule->matchzone);
	naxsi_mem_free(memory, rule);
}

/**
 * @brief      Frees a naxsi_whitelist_t pointer
 *
 * @param[in]  memory     The naxsi_mem_t structure to use
 * @param      whitelist  The naxsi_whitelist_t to be freed
 */
void naxsi_whitelist_free(const naxsi_mem_t *memory, naxsi_whitelist_t *whitelist) {
	if (!memory || !whitelist) {
		return;
	}
	naxsi_ids_free(memory, whitelist->ids);
	matchzone_fini(memory, &whitelist->matchzone);
	naxsi_mem_free(memory, whitelist);
}
