// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only
#include "minunit.h"
#include "naxsi_mem.h"
#include <stdlib.h>

static bool malloc_called = false;
static bool calloc_called = false;
static void *free_called = 0;
static void *free_user = 0;

#define mu_test_naxsi_mem() naxsi_memory(42, &test_free, &test_malloc, &test_calloc)

static void test_free(void *user, void *pointer) {
	free(pointer);
	free_called = pointer;
	free_user = user;
}

static void *test_malloc(void *user, size_t size) {
	malloc_called = true;
	if (user) {
		return malloc(size);
	}
	return NULL;
}

static void *test_calloc(void *user, size_t nmemb, size_t size) {
	calloc_called = true;
	if (user) {
		return calloc(nmemb, size);
	}
	return NULL;
}

bool test_naxsi_mem_malloc(void) {
	naxsi_mem_t mem = mu_test_naxsi_mem();
	void *ptr = NULL;

	malloc_called = false;
	ptr = naxsi_mem_malloc(NULL, 10);
	mu_assert_null(ptr, "naxsi_mem_malloc(NULL, 10) shall return NULL");
	mu_assert_false(calloc_called, "verify that memory->malloc is not called");

	ptr = naxsi_mem_malloc(&mem, 0);
	mu_assert_null(ptr, "naxsi_mem_malloc(&mem, 0) shall return NULL");

	ptr = naxsi_mem_malloc(&mem, 10);
	mu_assert_notnull(ptr, "naxsi_mem_malloc(&mem, 10) shall return not NULL");
	free(ptr);

	mem.user = NULL;
	ptr = naxsi_mem_malloc(&mem, 10);
	mu_assert_null(ptr, "verify that void *user is passed to memory->malloc");

	mu_end;
}

bool test_naxsi_mem_calloc(void) {
	naxsi_mem_t mem = mu_test_naxsi_mem();
	void *ptr = NULL;

	calloc_called = false;
	ptr = naxsi_mem_calloc(NULL, 10, 10);
	mu_assert_null(ptr, "naxsi_mem_calloc(NULL, 10, 10) shall return NULL");
	mu_assert_false(calloc_called, "verify that memory->calloc is not called");

	ptr = naxsi_mem_calloc(&mem, 10, 0);
	mu_assert_null(ptr, "naxsi_mem_calloc(&mem, 10, 0) shall return NULL");

	ptr = naxsi_mem_calloc(&mem, 1, 10);
	mu_assert_notnull(ptr, "naxsi_mem_calloc(&mem, 10) shall return not NULL");
	mu_assert_memeq(ptr, (const void *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 10, "naxsi_mem_calloc(10) shall be set to zero");
	free(ptr);

	mem.user = NULL;
	ptr = naxsi_mem_calloc(&mem, 10, 10);
	mu_assert_null(ptr, "verify that void *user is passed to memory->calloc");

	mu_end;
}

bool test_naxsi_mem_free(void) {
	naxsi_mem_t mem = mu_test_naxsi_mem();
	void *ptr = NULL;

	free_called = NULL;
	free_user = NULL;
	ptr = naxsi_mem_malloc(&mem, 10);
	naxsi_mem_free(&mem, ptr);
	mu_assert_notnull(free_called, "verify that free has been called");
	mu_assert_notnull(free_user, "verify that void *user is passed to free");

	free_called = (void *)32;
	naxsi_mem_free(&mem, NULL);
	mu_assert_ptreq(free_called, (void *)32, "naxsi_mem_free(&mem, NULL) shall not call memory->free");
	naxsi_mem_free(NULL, (void *)55);
	mu_assert_ptreq(free_called, (void *)32, "naxsi_mem_free(NULL, 55) shall not call memory->free");

	mu_end;
}

int all_tests() {
	mu_run_test(test_naxsi_mem_malloc);
	mu_run_test(test_naxsi_mem_calloc);
	mu_run_test(test_naxsi_mem_free);
	return tests_passed != tests_run;
}

mu_main(all_tests)
