// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_config.h"
#include "naxsi_mem.h"

static bool naxsi_config_add_defaults(naxsi_config_t *config) {
	return true;
}

/**
 * @brief      Allocates and initializes a naxsi_config_t structure
 *
 * @param      memory          The naxsi_mem_t structure to use for handling memory
 * @param      defaults_rules  When enabled, adds the default rules into the configuration
 *
 * @return     NULL if fails to allocate, otherwise non NULL.
 */
NAXSI_API naxsi_config_t *naxsi_config_new(const naxsi_mem_t *memory, bool defaults_rules) {
	if (!memory) {
		return NULL;
	}

	naxsi_config_t *cfg = naxsi_mem_new(memory, naxsi_config_t);
	if (!cfg) {
		return NULL;
	}

	cfg->rules = naxsi_list_new(memory, (naxsi_free_t)&naxsi_rule_free);
	cfg->whitelist = naxsi_list_new(memory, (naxsi_free_t)&naxsi_whitelist_free);
	if (!cfg->rules || !cfg->whitelist) {
		naxsi_config_free(memory, cfg);
		return NULL;
	}

	if (defaults_rules && !naxsi_config_add_defaults(cfg)) {
		naxsi_config_free(memory, cfg);
		return NULL;
	}

	return cfg;
}

/**
 * @brief      Finalize and frees a naxsi_config_t pointer.
 *
 * @param      memory  The naxsi_mem_t structure to use for handling memory
 * @param      cfg     The naxsi_config_t to free.
 */
NAXSI_API void naxsi_config_free(const naxsi_mem_t *memory, naxsi_config_t *cfg) {
	if (!memory || !cfg) {
		return;
	}
	naxsi_list_free(memory, cfg->rules);
	naxsi_list_free(memory, cfg->whitelist);
	naxsi_mem_free(memory, cfg);
}
