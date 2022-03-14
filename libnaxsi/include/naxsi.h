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

NAXSI_API bool naxsi_set_learning_mode(naxsi_t *nxs, bool enable);
NAXSI_API bool naxsi_set_security_rules(naxsi_t *nxs, bool enable);
NAXSI_API bool naxsi_set_libinjection_sql(naxsi_t *nxs, bool enable);
NAXSI_API bool naxsi_set_libinjection_xss(naxsi_t *nxs, bool enable);
NAXSI_API bool naxsi_set_denied_url(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *denied_url);
NAXSI_API bool naxsi_add_ignore_ip(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *ip_address);
NAXSI_API bool naxsi_add_ignore_cidr(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *ip_cidr);
NAXSI_API bool naxsi_add_checkrule(const naxsi_mem_t *memory, naxsi_t *nxs, naxsi_str_t *checkrule_s, naxsi_str_t *action_s);
NAXSI_API bool naxsi_add_rule_args(const naxsi_mem_t *memory, naxsi_t *nxs, size_t argc, naxsi_str_t *argv);

#ifdef __cplusplus
}
#endif

#endif /* NAXSI_H */
