// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_LIST_H
#define NAXSI_LIST_H

#include <naxsi_types.h>
#include "naxsi_mem.h"

typedef struct naxsi_list_iter naxsi_list_iter_t;
typedef struct naxsi_list_iter {
	naxsi_list_iter_t *next;
	void *value;
} naxsi_list_iter_t;

typedef struct naxsi_list {
	size_t length;
	naxsi_free_t free;
	naxsi_list_iter_t *head;
	naxsi_list_iter_t *tail;
} naxsi_list_t;

#define naxsi_list_foreach(list, it, pos) \
	if (list) \
		for (it = list->head; it && (pos = it->data, 1); it = it->next)

naxsi_list_t *naxsi_list_new(const naxsi_mem_t *memory, naxsi_free_t pfree);
naxsi_list_iter_t *naxsi_list_append(const naxsi_mem_t *memory, naxsi_list_t *list, void *data);
void naxsi_list_free(const naxsi_mem_t *memory, naxsi_list_t *list);

#endif /* NAXSI_LIST_H */
