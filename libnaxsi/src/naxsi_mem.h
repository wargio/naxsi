// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_MEM_H
#define NAXSI_MEM_H

#include <naxsi_types.h>

typedef void (*naxsi_free_t)(const naxsi_mem_t *memory, void *pointer);

void naxsi_mem_free(const naxsi_mem_t *memory, void *pointer);
void *naxsi_mem_malloc(const naxsi_mem_t *memory, size_t size);
void *naxsi_mem_calloc(const naxsi_mem_t *memory, size_t nmemb, size_t size);
#define naxsi_mem_new(m, t) naxsi_mem_calloc(m, 1, sizeof(t))

naxsi_str_t *naxsi_str_new(const naxsi_mem_t *memory, char *string, size_t length, bool copy);
#define naxsi_mem_new2(m, s, c) naxsi_str_new(m, s, strlen(s), c)
void naxsi_str_free(const naxsi_mem_t *memory, naxsi_str_t *string, bool copy);

#endif /* NAXSI_MEM_H */
