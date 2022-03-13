// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_VECTOR_H
#define NAXSI_VECTOR_H

#include <naxsi_types.h>
#include "naxsi_mem.h"

/* vector of native types */
#define naxsi_vector(name, type) \
	typedef struct naxsi_##name##_s { \
		type *data; \
		size_t length; \
		size_t capacity; \
	} naxsi_##name##_t; \
\
	naxsi_##name##_t *naxsi_##name##_new(const naxsi_mem_t *memory, size_t capacity); \
	void naxsi_##name##_free(const naxsi_mem_t *memory, naxsi_##name##_t *vec); \
	bool naxsi_##name##_push(const naxsi_mem_t *memory, naxsi_##name##_t *vec, type value); \
	type naxsi_##name##_get_at(naxsi_##name##_t *vec, size_t index, type def_value)

/* vector of structs */
#define naxsi_svector(name, type) \
	typedef struct naxsi_##name##_s { \
		type *data; \
		size_t length; \
		size_t capacity; \
	} naxsi_##name##_t; \
\
	naxsi_##name##_t *naxsi_##name##_new(const naxsi_mem_t *memory, size_t capacity); \
	void naxsi_##name##_free(const naxsi_mem_t *memory, naxsi_##name##_t *vec); \
	bool naxsi_##name##_push(const naxsi_mem_t *memory, naxsi_##name##_t *vec, type *value); \
	type *naxsi_##name##_get_at(naxsi_##name##_t *vec, size_t index, type *def_value)

/* vector of pointers */
#define naxsi_pvector(name, type) \
	typedef struct naxsi_##name##_s { \
		type **data; \
		size_t length; \
		size_t capacity; \
		naxsi_free_t free; \
	} naxsi_##name##_t; \
\
	naxsi_##name##_t *naxsi_##name##_new(const const naxsi_mem_t *memory, size_t capacity, naxsi_free_t pfree); \
	void naxsi_##name##_free(const const naxsi_mem_t *memory, naxsi_##name##_t *vec); \
	bool naxsi_##name##_push(const naxsi_mem_t *memory, naxsi_##name##_t *vec, type *value); \
	type *naxsi_##name##_get_at(naxsi_##name##_t *vec, size_t index, type *def_value)

/* vector of native types */
#define naxsi_vector_define(name, type) \
	naxsi_##name##_t *naxsi_##name##_new(const naxsi_mem_t *memory, size_t capacity) { \
		if (!memory || capacity < 1) { \
			return NULL; \
		} \
		naxsi_##name##_t *vec = naxsi_mem_new(memory, naxsi_##name##_t); \
		if (!vec) { \
			return NULL; \
		} \
		vec->length = 0; \
		vec->capacity = capacity; \
		vec->data = naxsi_mem_calloc(memory, capacity, sizeof(type)); \
		if (!vec->data) { \
			naxsi_mem_free(memory, vec); \
			return NULL; \
		} \
		return vec; \
	} \
\
	void naxsi_##name##_free(const naxsi_mem_t *memory, naxsi_##name##_t *vec) { \
		if (!memory || !vec) { \
			return; \
		} \
		naxsi_mem_free(memory, vec->data); \
		naxsi_mem_free(memory, vec); \
	} \
\
	bool naxsi_##name##_push(const naxsi_mem_t *memory, naxsi_##name##_t *vec, type value) { \
		if (!vec) { \
			return false; \
		} else if (vec->length == vec->capacity) { \
			size_t new_capacity = vec->capacity + 32; \
			type *tmp = naxsi_mem_calloc(memory, new_capacity, sizeof(type)); \
			if (tmp == NULL) { \
				return false; \
			} \
			size_t copy_len = new_capacity * sizeof(type); \
			memcpy(tmp, vec->data, copy_len); \
			naxsi_mem_free(memory, vec->data); \
			vec->data = tmp; \
			vec->capacity = new_capacity; \
		} \
		vec->data[vec->length] = value; \
		vec->length++; \
		return true; \
	} \
\
	type naxsi_##name##_get_at(naxsi_##name##_t *vec, size_t index, type def_value) { \
		if (!vec || index >= vec->length) { \
			return def_value; \
		} \
		return vec->data[index]; \
	}

/* vector of structs */
#define naxsi_svector_define(name, type) \
	naxsi_##name##_t *naxsi_##name##_new(const naxsi_mem_t *memory, size_t capacity) { \
		if (!memory || capacity < 1) { \
			return NULL; \
		} \
		naxsi_##name##_t *vec = naxsi_mem_new(memory, naxsi_##name##_t); \
		if (!vec) { \
			return NULL; \
		} \
		vec->length = 0; \
		vec->capacity = capacity; \
		vec->data = naxsi_mem_calloc(memory, capacity, sizeof(type)); \
		if (!vec->data) { \
			naxsi_mem_free(memory, vec); \
			return NULL; \
		} \
		return vec; \
	} \
\
	void naxsi_##name##_free(const naxsi_mem_t *memory, naxsi_##name##_t *vec) { \
		if (!memory || !vec) { \
			return; \
		} \
		naxsi_mem_free(memory, vec->data); \
		naxsi_mem_free(memory, vec); \
	} \
\
	bool naxsi_##name##_push(const naxsi_mem_t *memory, naxsi_##name##_t *vec, type *value) { \
		if (!vec) { \
			return false; \
		} else if (vec->length == vec->capacity) { \
			size_t new_capacity = vec->capacity + 32; \
			type *tmp = naxsi_mem_calloc(memory, new_capacity, sizeof(type)); \
			if (tmp == NULL) { \
				return false; \
			} \
			size_t copy_len = new_capacity * sizeof(type); \
			memcpy(tmp, vec->data, copy_len); \
			naxsi_mem_free(memory, vec->data); \
			vec->data = tmp; \
			vec->capacity = new_capacity; \
		} \
		vec->data[vec->length] = *value; \
		vec->length++; \
		return true; \
	} \
\
	type *naxsi_##name##_get_at(naxsi_##name##_t *vec, size_t index, type *def_value) { \
		if (!vec || index >= vec->length) { \
			return def_value; \
		} \
		return &vec->data[index]; \
	}

/* vector of pointers */
#define naxsi_pvector_define(name, type) \
	naxsi_##name##_t *naxsi_##name##_new(const const naxsi_mem_t *memory, size_t capacity, naxsi_free_t pfree) { \
		if (!memory || capacity < 1) { \
			return NULL; \
		} \
		naxsi_##name##_t *vec = naxsi_mem_new(memory, naxsi_##name##_t); \
		if (!vec) { \
			return NULL; \
		} \
		vec->free = pfree; \
		vec->length = 0; \
		vec->capacity = capacity; \
		vec->data = naxsi_mem_calloc(memory, capacity, sizeof(type *)); \
		if (!vec->data) { \
			naxsi_mem_free(memory, vec); \
			vec = NULL; \
		} \
		return vec; \
	} \
\
	void naxsi_##name##_free(const const naxsi_mem_t *memory, naxsi_##name##_t *vec) { \
		if (!memory || !vec) { \
			return; \
		} \
		if (vec->free) { \
			for (size_t i = 0; i < vec->length; ++i) { \
				vec->free(memory, vec->data[i]); \
			} \
		} \
		naxsi_mem_free(memory, vec->data); \
		naxsi_mem_free(memory, vec); \
	} \
\
	bool naxsi_##name##_push(const naxsi_mem_t *memory, naxsi_##name##_t *vec, type *value) { \
		if (!vec) { \
			return false; \
		} else if (vec->length == vec->capacity) { \
			size_t new_capacity = vec->capacity + 32; \
			type **tmp = naxsi_mem_calloc(memory, new_capacity, sizeof(type *)); \
			if (tmp == NULL) { \
				return false; \
			} \
			size_t copy_len = new_capacity * sizeof(type *); \
			memcpy(tmp, vec->data, copy_len); \
			naxsi_mem_free(memory, vec->data); \
			vec->data = tmp; \
			vec->capacity = new_capacity; \
		} \
		vec->data[vec->length] = value; \
		vec->length++; \
		return true; \
	} \
\
	type *naxsi_##name##_get_at(naxsi_##name##_t *vec, size_t index, type *def_value) { \
		if (!vec || index >= vec->length) { \
			return def_value; \
		} \
		return vec->data[index]; \
	}

#endif /* NAXSI_VECTOR_H */
