// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_list.h"

/**
 * @brief      Allocates and initialize a new naxsi_list_t
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param[in]  pfree   The pointer to a naxsi_free_t
 *
 * @return     Pointer to naxsi_list_t (NULL if fails)
 */
naxsi_list_t *naxsi_list_new(const naxsi_mem_t *memory, naxsi_free_t pfree) {
	if (!memory) {
		return NULL;
	}
	naxsi_list_t *list = naxsi_mem_new(memory, naxsi_list_t);
	if (!list) {
		return NULL;
	}
	list->free = pfree;
	return list;
}

/**
 * @brief      Appends a value to a naxsi_list_t
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param      list    The naxsi_list_t to append to
 * @param      value   The value to append
 *
 * @return     Pointer to naxsi_list_iter_t (NULL if fails)
 */
naxsi_list_iter_t *naxsi_list_append(const naxsi_mem_t *memory, naxsi_list_t *list, void *value) {
	if (!memory || !list) {
		return NULL;
	}
	naxsi_list_iter_t *item = naxsi_mem_new(memory, naxsi_list_iter_t);
	if (!item) {
		return item;
	}
	list->length++;
	item->value = value;
	if (list->tail) {
		list->tail->next = item;
	}
	list->tail = item;
	if (!list->head) {
		list->head = item;
	}
	return item;
}

/**
 * @brief      Frees a naxsi_list_t structure
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param      list    The naxsi_list_t pointer to free
 */
void naxsi_list_free(const naxsi_mem_t *memory, naxsi_list_t *list) {
	if (!memory || !list) {
		return;
	}
	naxsi_list_iter_t *iter = list->head;
	while (iter) {
		naxsi_list_iter_t *next = iter->next;
		if (list->free) {
			list->free(memory, iter->value);
		}
		naxsi_mem_free(memory, iter);
		iter = next;
	}
	naxsi_mem_free(memory, list);
}
