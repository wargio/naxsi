// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include <naxsi_types.h>
#include "naxsi_mem.h"

/**
 * @brief      Allocates and initialize a new naxsi_str_t structure
 *
 * @param      memory  The naxsi_mem_t structure to use
 * @param      string  The string to hold or to copy
 * @param[in]  length  The string length
 * @param[in]  copy    Set it to true to copy the string
 *
 * @return     Pointer to naxsi_str_t or NULL
 */
naxsi_str_t *naxsi_str_new(const naxsi_mem_t *memory, char *string, size_t length, bool copy) {
	if (!memory || !string || length < 1) {
		return NULL;
	}
	naxsi_str_t *str = naxsi_mem_new(memory, naxsi_str_t);
	if (!str) {
		return NULL;
	}
	if (copy) {
		str->data = naxsi_mem_malloc(memory, length + 1);
		if (!str->data) {
			naxsi_mem_free(memory, str);
			return NULL;
		}
		memcpy(str->data, string, length + 1);
	} else {
		str->data = string;
	}
	str->length = length;
	return str;
}

/**
 * @brief      Frees the memory space pointed to by `string`.
 *
 * @param      memory  The naxsi_mem_t structure to use
 * @param      string  The naxsi_str_t pointer to free
 * @param[in]  copy    Set it to true if you have called naxsi_str_new with `copy` option set to true
 */
void naxsi_str_free(const naxsi_mem_t *memory, naxsi_str_t *string, bool copy) {
	if (!memory || !string) {
		return;
	}
	if (copy) {
		naxsi_mem_free(memory, string->data);
	}
	naxsi_mem_free(memory, string);
}
