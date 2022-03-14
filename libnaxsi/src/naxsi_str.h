// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_STR_H
#define NAXSI_STR_H

#include <naxsi_mem.h>

bool naxsi_str_init(const naxsi_mem_t *memory, naxsi_str_t *nxstr, char *string, size_t length, bool copy);
naxsi_str_t *naxsi_str_new(const naxsi_mem_t *memory, char *string, size_t length, bool copy);
#define naxsi_str_new2(m, s, c) naxsi_str_new(m, s, strlen(s), c)
void naxsi_str_fini(const naxsi_mem_t *memory, naxsi_str_t *string, bool copy);
void naxsi_str_free(const naxsi_mem_t *memory, naxsi_str_t *string, bool copy);

#endif /* NAXSI_STR_H */
