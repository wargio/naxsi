// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_TYPES_H
#define NAXSI_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#define U64_MAX (0xfffffffffffffffful)
#define U32_MAX (0xffffffffu)
#define U16_MAX (0xffffu)
#define U8_MAX  (0xffu)

typedef struct naxsi_str {
	char *data;
	size_t length;
} naxsi_str_t;

#define naxsi_string(x) \
	{ x, strlen(x) }

typedef void (*naxsi_mem_free_t)(void *user, void *pointer);
typedef void *(*naxsi_mem_malloc_t)(void *user, size_t size);
typedef void *(*naxsi_mem_calloc_t)(void *user, size_t nmemb, size_t size);

typedef struct naxsi_mem {
	void *user;
	naxsi_mem_free_t free;
	naxsi_mem_malloc_t malloc;
	naxsi_mem_calloc_t calloc;
} naxsi_mem_t;

#define naxsi_memory(puser, pfree, pmalloc, pcalloc) \
	{ (void *)puser, (naxsi_mem_free_t)pfree, (naxsi_mem_malloc_t)pmalloc, (naxsi_mem_calloc_t)pcalloc }

typedef struct naxsi naxsi_t;

#ifdef NAXSI_API
#undef NAXSI_API
#endif
#if defined(__GNUC__) && __GNUC__ >= 4
#define NAXSI_API __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define NAXSI_API __declspec(dllexport)
#else
#define NAXSI_API
#endif

#ifdef __cplusplus
}
#endif

#endif /* NAXSI_TYPES_H */
