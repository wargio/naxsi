// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only
#include "minunit_mem.h"
#include "naxsi_str.h"

bool test_naxsi_str(void) {
	char *ptr;
	naxsi_str_t *str;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* test invalid allocation */

	str = naxsi_str_new(NULL, NULL, 0, false);
	mu_assert_null(str, "naxsi_str_new(NULL, NULL, 0, false) shall return NULL");

	str = naxsi_str_new(NULL, "", 0, false);
	mu_assert_null(str, "naxsi_str_new(NULL, \"\", 0, false) shall return NULL");

	str = naxsi_str_new(NULL, "aaaaa", 5, false);
	mu_assert_null(str, "naxsi_str_new(NULL, \"aaaaa\", 5, false) shall return NULL");

	str = naxsi_str_new(&mem, "aaaaa", 0, false);
	mu_assert_null(str, "naxsi_str_new(&mem, \"aaaaa\", 0, false) shall return NULL");

	/* verify the string is not copied */
	ptr = (char *)"some string";
	str = naxsi_mem_new2(&mem, ptr, false);
	mu_assert_notnull(str, "naxsi_str_new2(&mem, \"some string\", false) shall NOT return NULL");
	mu_assert_ptreq(str->data, ptr, "str->data shall be ptr");
	naxsi_str_free(&mem, str, false); // should not crash

	/* verify the string is not copied */
	ptr = (char *)"some string";
	str = naxsi_mem_new2(&mem, ptr, true);
	mu_assert_notnull(str, "naxsi_str_new2(&mem, \"some string\", true) shall NOT return NULL");
	mu_assert_ptrneq(str->data, ptr, "str->data shall NOT be ptr");
	naxsi_str_free(&mem, str, true); // should free str->data

	mu_end;
}

int all_tests() {
	mu_run_test(test_naxsi_str);
	return tests_passed != tests_run;
}

mu_main(all_tests)
