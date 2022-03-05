// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only
#include "minunit_mem.h"
#include "naxsi_list.h"

typedef struct test_s {
	int padding;
} test_s_t;

static void test_s_free(const naxsi_mem_t *memory, void *p) {
	naxsi_mem_free(memory, p);
}

bool test_naxsi_list(void) {
	test_s_t *elem;
	naxsi_list_t *list;
	naxsi_list_iter_t *iter;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* test invalid allocation */

	list = naxsi_list_new(NULL, NULL);
	mu_assert_null(list, "naxsi_list_new(NULL, NULL) shall return NULL");

	list = naxsi_list_new(NULL, (naxsi_free_t)43);
	mu_assert_null(list, "naxsi_list_new(NULL, 43) shall return NULL");

	/* test valid allocation */

	list = naxsi_list_new(&mem, NULL);
	mu_assert_notnull(list, "naxsi_list_new(&mem, NULL) shall not return NULL");
	mu_assert_null(list->head, "list->head shall not be NULL");
	mu_assert_null(list->tail, "list->tail shall not be NULL");
	mu_assert_eq_fmt(list->length, 0lu, "list->length shall be 0", "%lu");
	mu_assert_null(list->free, "list->free shall not be NULL");
	naxsi_list_free(&mem, list);

	/* test with custom free function */

	list = naxsi_list_new(&mem, (naxsi_free_t)&test_s_free);
	mu_assert_notnull(list, "naxsi_list_new(&mem, test_s_free) shall not return NULL");
	mu_assert_null(list->head, "list->head shall not be NULL");
	mu_assert_null(list->tail, "list->tail shall not be NULL");
	mu_assert_eq_fmt(list->length, 0lu, "list->length shall be 0", "%lu");
	mu_assert_ptreq(list->free, &test_s_free, "list->free shall be test_s_free");

	elem = naxsi_mem_new(&mem, test_s_t);
	mu_assert_notnull(elem, "naxsi_mem_new(&mem, test_s_t) shall not return NULL");

	iter = naxsi_list_append(NULL, NULL, NULL);
	mu_assert_null(iter, "naxsi_list_append(NULL, NULL, NULL) shall return NULL");

	iter = naxsi_list_append(NULL, (naxsi_list_t *)42, NULL);
	mu_assert_null(iter, "naxsi_list_append(NULL, 42, NULL) shall return NULL");

	iter = naxsi_list_append((naxsi_mem_t *)42, NULL, NULL);
	mu_assert_null(iter, "naxsi_list_append(42, NULL, NULL) shall return NULL");

	iter = naxsi_list_append(&mem, list, NULL);
	mu_assert_notnull(iter, "naxsi_list_append(&mem, list, NULL) shall not return NULL");
	mu_assert_eq_fmt(list->length, 1lu, "list->length shall be 1", "%lu");
	mu_assert_ptreq(list->tail, iter, "list->tail shall be iter");
	mu_assert_ptreq(list->head, iter, "list->tail shall be iter");

	iter = naxsi_list_append(&mem, list, elem);
	mu_assert_notnull(iter, "naxsi_list_append(&mem, list, elem) shall not return NULL");
	mu_assert_eq_fmt(list->length, 2lu, "list->length shall be 2", "%lu");
	mu_assert_ptreq(list->tail, iter, "list->tail shall be iter");
	mu_assert_ptrneq(list->head, iter, "list->tail shall not be iter");
	mu_assert_null(iter->next, "iter->next shall not be iter");

	naxsi_list_free(&mem, list);

	/* test with without free function */

	list = naxsi_list_new(&mem, NULL);
	mu_assert_notnull(list, "naxsi_list_new(&mem, NULL) shall not return NULL");

	iter = naxsi_list_append(&mem, list, NULL);
	mu_assert_notnull(iter, "naxsi_list_append(&mem, list, NULL) shall not return NULL");
	mu_assert_eq_fmt(list->length, 1lu, "list->length shall be 1", "%lu");

	iter = naxsi_list_append(&mem, list, (void *)42);
	mu_assert_notnull(iter, "naxsi_list_append(&mem, list, elem) shall not return NULL");
	mu_assert_eq_fmt(list->length, 2lu, "list->length shall be 2", "%lu");

	iter = naxsi_list_append(&mem, list, (void *)43);
	mu_assert_notnull(iter, "naxsi_list_append(&mem, list, elem) shall not return NULL");
	mu_assert_eq_fmt(list->length, 3lu, "list->length shall be 3", "%lu");

	naxsi_list_free(&mem, list);
	mu_end;
}

int all_tests() {
	mu_run_test(test_naxsi_list);
	return tests_passed != tests_run;
}

mu_main(all_tests)
