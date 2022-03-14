// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only
#include "minunit.h"
#include "naxsi_utils.h"
#include <stdlib.h>

#define mu_assert_utf8(a, e, m) \
	do { \
		naxsi_str_t _input; \
		_input.data = (a); \
		_input.length = sizeof(a) - 1; \
		bool _r = naxsi_is_invalid_utf8(&_input); \
		mu_assert("utf8: " m, _r == e); \
	} while (0)

#define mu_assert_strstr_offset(a, o, b, e, m) \
	do { \
		naxsi_str_t _input = naxsi_string(a); \
		naxsi_str_t _substr = naxsi_string(b); \
		const char *_x = naxsi_strstr_offset(&_input, (o), &_substr); \
		mu_assert_ptreq(_x, (e) ? ((a) + (e)) : NULL, (m)); \
	} while (0)

#define mu_assert_strnchr(a, b, e, m) \
	do { \
		size_t _l = strlen(a); \
		const char *_x = naxsi_strnchr((a), (b), _l); \
		mu_assert_ptreq(_x, (e) ? ((a) + (e)) : NULL, "strnchr: " m); \
	} while (0)

bool test_naxsi_escape_nullbytes(void) {
	int i, count = 0;
	naxsi_str_t input;
	input.data = malloc(32);

	// apply null bytes twice
	memcpy(input.data, "with\x00 nullbytes\x00 here", 22);
	input.length = 21;
	mu_assert_true(naxsi_escape_nullbytes(&input), "has escaped null bytes");
	for (i = 0, count = 0; i < input.length; ++i) {
		if (input.data[i] == '0') {
			count++;
		}
	}
	mu_assert_eq_fmt(count, 2, "escaped 2 null bytes", "%u");
	mu_assert_true(input.data[4] == '0', "escape null byte at 4");
	mu_assert_true(input.data[15] == '0', "escape null byte at 15");

	// fail because there are no null byte
	strcpy(input.data, "no nullbytes here");
	input.length = strlen(input.data);
	mu_assert_false(naxsi_escape_nullbytes(&input), "has not escaped null bytes");
	for (i = 0, count = 0; i < input.length; ++i) {
		if (input.data[i] == '0') {
			count++;
		}
	}
	mu_assert_eq_fmt(count, 0, "escaped 0 null bytes", "%u");

	free(input.data);
	mu_end;
}

bool test_naxsi_strstr_offset(void) {
	mu_assert_strstr_offset("test this strstr @ function", 0, "strstr", 10u, "strstr is in string");
	mu_assert_strstr_offset("test this strstr @ function", 11, "strstr", 0u, "strstr is NOT in string when offset is 11");
	mu_assert_strstr_offset("a", 0, "aaaa", 0u, "cannot find a substring in a smaller string");
	mu_assert_strstr_offset("", 0, "bbbbb", 0u, "cannot find a substring in a empty string");
	mu_assert_strstr_offset("aaaaaa", 0, "", 0u, "cannot find a empty substring in a string");
	mu_assert_strstr_offset("bbaaaaa", 12, "aaa", 0u, "cannot find a substring in a string when offset exceeds string length");
	mu_end;
}

bool test_naxsi_strnchr(void) {
	mu_assert_strnchr("test this strnchr @ function", '@', 18u, "@ is in string");
	mu_assert_strnchr("test this strnchr @ function", '~', 0, "~ is NOT in string");
	mu_assert_strnchr("", 'A', 0, "return NULL when string is empty");
	mu_assert_strnchr("   aaaaaBBBBccc", 'A', 0, "A is NOT in string because is case sensitive");
	mu_end;
}

bool test_naxsi_is_invalid_utf8(void) {
	mu_assert_utf8("aaaaaaa", false, "valid ascii");
	mu_assert_utf8("\xc3\xb1", false, "valid 2 octet sequence");
	mu_assert_utf8("\xc3\x28", true, "invalid 2 octet sequence");
	mu_assert_utf8("\xa0\xa1", true, "invalid sequence identifier");
	mu_assert_utf8("\xe2\x82\xa1", false, "valid 3 octet sequence");
	mu_assert_utf8("\xe2\x28\xa1", true, "invalid 3 octet sequence (in 2nd octet)");
	mu_assert_utf8("\xe2\x82\x28", true, "invalid 3 octet sequence (in 3rd octet)");
	mu_assert_utf8("\xf0\x90\x8c\xbc", false, "valid 4 octet sequence");
	mu_assert_utf8("\xf0\x28\x8c\xbc", true, "invalid 4 octet sequence (in 2nd octet)");
	mu_assert_utf8("\xf0\x90\x28\xbc", true, "invalid 4 octet sequence (in 3rd octet)");
	mu_assert_utf8("\xf0\x28\x8c\x28", true, "invalid 4 octet sequence (in 4th octet)");
	mu_assert_utf8("\xf8\xa1\xa1\xa1\xa1", true, "valid 5 octet sequence, but not unicode");
	mu_assert_utf8("\xfc\xa1\xa1\xa1\xa1\xa1", true, "valid 6 octet sequence, but not unicode");
	mu_end;
}

int all_tests() {
	mu_run_test(test_naxsi_escape_nullbytes);
	mu_run_test(test_naxsi_strstr_offset);
	mu_run_test(test_naxsi_strnchr);
	mu_run_test(test_naxsi_is_invalid_utf8);
	return tests_passed != tests_run;
}

mu_main(all_tests)
