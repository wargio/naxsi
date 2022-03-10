// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_CONFIG_H
#define NAXSI_CONFIG_H

#include <naxsi.h>
#include "naxsi_list.h"
#include "naxsi_rule.h"

typedef struct naxsi_config {
	naxsi_list_t /*<naxsi_rule_t*>*/ *rules;
	naxsi_list_t /*<naxsi_whitelist_t*>*/ *whitelist;
} naxsi_config_t;

#endif /* NAXSI_CONFIG_H */
