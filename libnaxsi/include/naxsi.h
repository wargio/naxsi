// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_H
#define NAXSI_H

#include <naxsi_types.h>
#include <naxsi_const.h>

#ifdef __cplusplus
extern "C" {
#endif

NAXSI_API naxsi_t *naxsi_new(const naxsi_mem_t *memory);
NAXSI_API void naxsi_free(const naxsi_mem_t *memory, naxsi_t *nxs);

NAXSI_API bool naxsi_ignore_ip(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *ip_address);
NAXSI_API bool naxsi_ignore_cidr(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *ip_cidr);

#ifdef __cplusplus
}
#endif

#endif /* NAXSI_H */
