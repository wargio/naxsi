// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_REGEX_H
#define NAXSI_REGEX_H

#include <naxsi_types.h>

typedef void naxsi_regex_t;

naxsi_regex_t *naxsi_regex_new(const naxsi_mem_t *memory, char *pattern);
void naxsi_regex_free(const naxsi_mem_t *memory, naxsi_regex_t *regex);
u32 naxsi_regex_match(naxsi_regex_t *regex, const char *string, size_t length);

#endif /* NAXSI_REGEX_H */
