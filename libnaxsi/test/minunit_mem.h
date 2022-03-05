// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only
#ifndef MU_MEMORY
#define MU_MEMORY
#include "minunit.h"
#include <stdlib.h>

#define mu_naxsi_mem() naxsi_memory(NULL, &mu_free, &mu_malloc, &mu_calloc)

static void mu_free(void *unused, void *pointer) {
	(void)unused;
	free(pointer);
}

static void *mu_malloc(void *unused, size_t size) {
	(void)unused;
	return malloc(size);
}

static void *mu_calloc(void *unused, size_t nmemb, size_t size) {
	(void)unused;
	return calloc(nmemb, size);
}

#endif /* MU_MEMORY */
