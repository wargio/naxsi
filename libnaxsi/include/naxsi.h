// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_H
#define NAXSI_H

#include <naxsi_types.h>
#include <naxsi_const.h>

#ifdef __cplusplus
extern "C" {
#endif

NAXSI_API naxsi_config_t *naxsi_config_new(const naxsi_mem_t *memory, bool defaults_rules);
NAXSI_API void naxsi_config_free(const naxsi_mem_t *memory, naxsi_config_t *nctx);

#ifdef __cplusplus
}
#endif

#endif /* NAXSI_H */
