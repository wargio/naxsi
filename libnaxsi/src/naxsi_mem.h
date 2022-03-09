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

#endif /* NAXSI_MEM_H */
