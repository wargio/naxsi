// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_mem.h"

/**
 * @brief      Frees the memory space pointed to by `pointer`.
 *
 * @param      memory   The naxsi_mem_t structure to use
 * @param      pointer  The pointer to free (NULL pointers are accepted)
 */
void naxsi_mem_free(const naxsi_mem_t *memory, void *pointer) {
	if (!pointer || !memory) {
		return;
	}
	memory->free(memory->user, pointer);
}

/**
 * @brief      Allocates memory of `size` bytes.
 *
 * @param      memory  The naxsi_mem_t structure to use
 * @param[in]  size    The size of each element to allocate
 *
 * @return     NULL if size is 0, otherwise the allocated pointer.
 */
void *naxsi_mem_malloc(const naxsi_mem_t *memory, size_t size) {
	if (!size || !memory) {
		return NULL;
	}
	return memory->malloc(memory->user, size);
}

/**
 * @brief      Allocates memory for a zeroed array of `nmemb` elements of `size` bytes each.
 *
 * @param      memory  The naxsi_mem_t structure to use
 * @param[in]  nmemb   The number of elements to allocate
 * @param[in]  size    The size of each element to allocate
 *
 * @return     NULL if size or nmemb is 0, otherwise the allocated pointer.
 */
void *naxsi_mem_calloc(const naxsi_mem_t *memory, size_t nmemb, size_t size) {
	if (!size || !nmemb || !memory) {
		return NULL;
	}
	return memory->calloc(memory->user, nmemb, size);
}
