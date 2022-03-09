// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_regex.h"
#include "naxsi_mem.h"

#if USE_PCRE1
#include <pcre.h>
#elif USE_PCRE2
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#else
#error "unknown PCRE version."
#endif

/**
 * @brief      Allocates and compiles a regular expression pattern
 *
 * @param[in]  memory   The naxsi_mem_t structure to use
 * @param      pattern  The regular expression patter to compile
 *
 * @return     Pointer to naxsi_regex_t or NULL when fails
 */
naxsi_regex_t *naxsi_regex_new(const naxsi_mem_t *memory, char *pattern) {
	if (!memory || !pattern) {
		return NULL;
	}
	int errcode;
#if USE_PCRE1
	int erroffset;
	return (naxsi_regex_t *)pcre_compile(pattern, PCRE_CASELESS | PCRE_MULTILINE, &errcode, &erroffset, NULL);
#else /* USE_PCRE2 */
	PCRE2_SIZE erroffset;
	return (naxsi_regex_t *)pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, PCRE2_CASELESS | PCRE2_MULTILINE, &errcode, &erroffset, NULL);
#endif
}

/**
 * @brief      Frees a compiled regex pattern
 *
 * @param[in]  memory   The naxsi_mem_t structure to use
 * @param      regex    The compiled regular expression to free
 */
void naxsi_regex_free(const naxsi_mem_t *memory, naxsi_regex_t *regex) {
	if (!memory || !regex) {
		return;
	}
#if USE_PCRE1
	pcre_free((pcre *)regex);
#else /* USE_PCRE2 */
	pcre2_code_free((pcre2_code *)regex);
#endif
}

/**
 * @brief      Verifies if a given string matches the regular expression
 *
 * @param      regex   The compile regular expression to use
 * @param[in]  string  The string to check for matches
 * @param[in]  length  The string length
 *
 * @return     Returns the total number of matches within the string
 */
u32 naxsi_regex_match(naxsi_regex_t *regex, const char *string, size_t length) {
	if (!regex || !string || length < 1) {
		return 0;
	}
	u32 n_matches = 0;
	int ret = 0;
#if USE_PCRE1
#define OVEC_SIZE 30
	int ovector[OVEC_SIZE]; // must be a multiple of 3
	for (size_t i = 0; i < length && ret >= 0; i = (size_t)ovector[1], n_matches += ret) {
		ret = pcre_exec((pcre *)regex, NULL, string, length, i, 0, ovector, OVEC_SIZE);
	}
#undef OVEC_SIZE
#else /* USE_PCRE2 */
	PCRE2_SIZE *ovector = NULL;
	pcre2_match_data *match_data = pcre2_match_data_create_from_pattern((pcre2_code *)regex, /* gcontext */ NULL);
	for (size_t i = 0; i < length; n_matches += ret) {
		if (!match_data) {
			return 0;
		}
		ret = pcre2_match((pcre2_code *)regex, (PCRE2_SPTR)string, length, i, 0, match_data, /* gcontext */ NULL);
		if (ret < 0) {
			break;
		}
		ovector = pcre2_get_ovector_pointer(match_data);
		i = (size_t)ovector[1];
	}
	pcre2_match_data_free(match_data);
#endif
	return n_matches;
}
