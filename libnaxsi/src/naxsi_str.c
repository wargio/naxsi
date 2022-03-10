// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include <naxsi_types.h>
#include "naxsi_mem.h"

/**
 * @brief      Initialize a naxsi_str_t structure with the given input string
 *
 * @param      memory  The naxsi_mem_t structure to use
 * @param      nxstr   The naxsi_str_t struct to initialize
 * @param      string  The string to hold or to copy
 * @param[in]  length  The string length
 * @param[in]  copy    Set it to true to copy the string
 *
 * @return     True when initializes the struct, otherwise false
 */
bool naxsi_str_init(const naxsi_mem_t *memory, naxsi_str_t *nxstr, char *string, size_t length, bool copy) {
	if (!memory || !nxstr || !string || length < 1) {
		return false;
	}
	if (copy) {
		nxstr->data = naxsi_mem_malloc(memory, length + 1);
		if (!nxstr->data) {
			return false;
		}
		memcpy(nxstr->data, string, length + 1);
	} else {
		nxstr->data = string;
	}
	nxstr->length = length;
	return true;
}

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
	naxsi_str_t *nxstr = naxsi_mem_new(memory, naxsi_str_t);
	if (!nxstr) {
		return NULL;
	}
	if (!naxsi_str_init(memory, nxstr, string, length, copy)) {
		naxsi_mem_free(memory, nxstr);
		return NULL;
	}
	return nxstr;
}

/**
 * @brief      Deinitializes a naxsi_str_t struct
 *
 * @param      memory  The naxsi_mem_t structure to use
 * @param      string  The naxsi_str_t pointer to clean
 * @param[in]  copy    Set it to true if you have called naxsi_str_new with `copy` option set to true
 */
void naxsi_str_fini(const naxsi_mem_t *memory, naxsi_str_t *string, bool copy) {
	if (!memory || !string) {
		return;
	}
	if (copy) {
		naxsi_mem_free(memory, string->data);
	}
	memset(string, 0, sizeof(naxsi_str_t));
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
	naxsi_str_fini(memory, string, copy);
	naxsi_mem_free(memory, string);
}
