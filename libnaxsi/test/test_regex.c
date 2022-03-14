// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only
#include "minunit_mem.h"
#include "naxsi_regex.h"

#define test_regex_match(r, s) naxsi_regex_match(r, s, strlen(s))

bool test_naxsi_regex(void) {
	u32 n_matches = 0;
	naxsi_regex_t *regex;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* test invalid allocation */

	regex = naxsi_regex_new(NULL, NULL);
	mu_assert_null(regex, "naxsi_regex_new(NULL, NULL) shall return NULL");

	regex = naxsi_regex_new(NULL, "");
	mu_assert_null(regex, "naxsi_regex_new(NULL, \"\") shall return NULL");

	regex = naxsi_regex_new(&mem, NULL);
	mu_assert_null(regex, "naxsi_regex_new(&mem, NULL) shall return NULL");

	/* compile regex */
	regex = naxsi_regex_new(&mem, "^[a-z]+$");
	mu_assert_notnull(regex, "naxsi_regex_new(&mem, \"^[a-z]+^$\") shall NOT return NULL");
	naxsi_regex_free(&mem, regex);

	/* compile bad regex */
	regex = naxsi_regex_new(&mem, "^[a-z+$");
	mu_assert_null(regex, "naxsi_regex_new(&mem, \"^[a-z+$\") shall return NULL");

	/* compile regex & match string */
	regex = naxsi_regex_new(&mem, "^[a-z]+$");

	/* check regex works (does not match) */
	n_matches = test_regex_match(regex, "some string");
	mu_assert_eq_fmt(0u, n_matches, "n_matches shall be 0", "%u");

	/* check regex works (does match) */
	n_matches = test_regex_match(regex, "something");
	mu_assert_eq_fmt(1u, n_matches, "n_matches shall be 1", "%u");

	/* check regex works (does match due PCRE_CASELESS) */
	n_matches = test_regex_match(regex, "someThing");
	mu_assert_eq_fmt(1u, n_matches, "n_matches shall be 1 (matches are case insensitive)", "%u");

	/* check regex works (does match due PCRE_MULTILINE) */
	n_matches = test_regex_match(regex, "something\nelse\n");
	mu_assert_eq_fmt(2u, n_matches, "n_matches shall be 2", "%u");

	naxsi_regex_free(&mem, regex);

	mu_end;
}

int all_tests() {
	mu_run_test(test_naxsi_regex);
	return tests_passed != tests_run;
}

mu_main(all_tests)
