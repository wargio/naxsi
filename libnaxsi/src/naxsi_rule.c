// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_rule.h"
#include "naxsi_regex.h"
#include "naxsi_utils.h"
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define starts_with(s, p) (!strncmp(s, p, strlen(p)))

naxsi_pvector_define(scores, naxsi_score_t);
naxsi_vector_define(ids, u32);

/**
 * @brief      Frees naxsi_matchzone_t contents
 *
 * @param      memory     The naxsi_mem_t structure to use
 * @param      matchzone  The naxsi_matchzone_t to contents free
 */
static void matchzone_fini(const naxsi_mem_t *memory, naxsi_matchzone_t *matchzone) {
	if (matchzone->args == NAXSI_MZ_TYPE_STRING) {
		naxsi_str_free(memory, matchzone->args_val, true);
	} else if (matchzone->args == NAXSI_MZ_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)matchzone->args_val);
	}
	if (matchzone->headers == NAXSI_MZ_TYPE_STRING) {
		naxsi_str_free(memory, matchzone->headers_val, true);
	} else if (matchzone->headers == NAXSI_MZ_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)matchzone->headers_val);
	}
	if (matchzone->body == NAXSI_MZ_TYPE_STRING) {
		naxsi_str_free(memory, matchzone->body_val, true);
	} else if (matchzone->body == NAXSI_MZ_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)matchzone->body_val);
	}
	if (matchzone->url == NAXSI_MZ_TYPE_STRING) {
		naxsi_str_free(memory, matchzone->url_val, true);
	} else if (matchzone->url == NAXSI_MZ_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)matchzone->url_val);
	}
}

/**
 * @brief      Returns a pointer pointing to either a naxsi_str_t or naxsi_regex_t
 *
 * @param[in]  memory    The naxsi_mem_t structure to use
 * @param      start     The variable start location
 * @param      end       The variable end location
 * @param[in]  is_regex  When true returns a naxsi_regex_t, otherwise a naxsi_str_t
 *
 * @return     On failure returns NULL, otherwise a non-zero pointer
 */
static void *matchzone_value_new(const naxsi_mem_t *memory, char *start, char *end, bool is_regex) {
	naxsi_str_t *variable = naxsi_str_new(memory, start, end - start, true);
	if (!variable || !is_regex) {
		return (void *)variable;
	}
	naxsi_regex_t *regex = naxsi_regex_new(memory, variable->data);
	naxsi_str_free(memory, variable, true);
	return regex;
}

/**
 * @brief      Parses the matchzone and sets it in the rule
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param      mz_s    The scores string
 * @param      mz      The matchzone to initialize
 *
 * @return     On success true, otherwise false
 */
static bool parse_matchzone(const naxsi_mem_t *memory, naxsi_str_t *mz_s, naxsi_matchzone_t *mz) {
	char *endp = mz_s->data + mz_s->length;

	mz->args = NAXSI_MZ_TYPE_UNSET;
	mz->body = NAXSI_MZ_TYPE_UNSET;
	mz->file_ext = NAXSI_MZ_TYPE_UNSET;
	mz->headers = NAXSI_MZ_TYPE_UNSET;
	mz->raw_body = NAXSI_MZ_TYPE_UNSET;
	mz->url = NAXSI_MZ_TYPE_UNSET;

	for (char *start = mz_s->data + strlen(NAXSI_RULE_PREFIX_MATCHZONE), *var_end = NULL; start < endp;) {
		if (*start == '|') {
			start++;
			continue;
		}
		/* matchzone modifiers */
		else if (starts_with(start, NAXSI_RULE_MATCHZONE_MOD_NAME)) {
			if (mz->name) {
				return false;
			}
			mz->name = true;
			start += strlen(NAXSI_RULE_MATCHZONE_MOD_NAME);
			continue;
		}
		/* global matchzone */
		else if (starts_with(start, NAXSI_RULE_MATCHZONE_ARGS)) {
			if (mz->args != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->args = NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_ARGS);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_BODY)) {
			if (mz->body != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->body = NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_BODY);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_FILE_EXT)) {
			if (mz->file_ext != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->file_ext = NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_FILE_EXT);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_HEADERS)) {
			if (mz->headers != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->headers = NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_HEADERS);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_RAW_BODY)) {
			if (mz->raw_body != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->raw_body = NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_RAW_BODY);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_URL)) {
			if (mz->url == NAXSI_MZ_TYPE_UNSET) {
				mz->url = NAXSI_MZ_TYPE_GLOBAL;
			}
			start += strlen(NAXSI_RULE_MATCHZONE_URL);
			continue;
		}
		/* case-insensitive string */
		else if (starts_with(start, NAXSI_RULE_MATCHZONE_ARGS_S)) {
			if (mz->args != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->args = NAXSI_MZ_TYPE_STRING;
			start += strlen(NAXSI_RULE_MATCHZONE_ARGS_S);
			if (!(var_end = strchr(start, '|'))) {
				var_end = endp;
			}
			mz->args_val = matchzone_value_new(memory, start, var_end, false);
			if (!mz->args_val) {
				return false;
			}
			start = var_end;
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_BODY_S)) {
			if (mz->body != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->body = NAXSI_MZ_TYPE_STRING;
			start += strlen(NAXSI_RULE_MATCHZONE_BODY_S);
			if (!(var_end = strchr(start, '|'))) {
				var_end = endp;
			}
			mz->body_val = matchzone_value_new(memory, start, var_end, false);
			if (!mz->body_val) {
				return false;
			}
			start = var_end;
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_HEADERS_S)) {
			if (mz->headers != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->headers = NAXSI_MZ_TYPE_STRING;
			start += strlen(NAXSI_RULE_MATCHZONE_HEADERS_S);
			if (!(var_end = strchr(start, '|'))) {
				var_end = endp;
			}
			mz->headers_val = matchzone_value_new(memory, start, var_end, false);
			if (!mz->headers_val) {
				return false;
			}
			start = var_end;
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_URL_S)) {
			if (mz->url != NAXSI_MZ_TYPE_UNSET && mz->url != NAXSI_MZ_TYPE_GLOBAL) {
				return false;
			}
			mz->url = NAXSI_MZ_TYPE_STRING;
			start += strlen(NAXSI_RULE_MATCHZONE_URL_S);
			if (!(var_end = strchr(start, '|'))) {
				var_end = endp;
			}
			mz->url_val = matchzone_value_new(memory, start, var_end, false);
			if (!mz->url_val) {
				return false;
			}
			start = var_end;
			continue;
		}
		/* case-insensitive regex */
		else if (starts_with(start, NAXSI_RULE_MATCHZONE_ARGS_X)) {
			if (mz->args != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->args = NAXSI_MZ_TYPE_REGEX;
			start += strlen(NAXSI_RULE_MATCHZONE_ARGS_X);
			if (!(var_end = strchr(start, '|'))) {
				var_end = endp;
			}
			mz->args_val = matchzone_value_new(memory, start, var_end, true);
			if (!mz->args_val) {
				return false;
			}
			start = var_end;
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_BODY_X)) {
			if (mz->body != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->body = NAXSI_MZ_TYPE_REGEX;
			start += strlen(NAXSI_RULE_MATCHZONE_BODY_X);
			if (!(var_end = strchr(start, '|'))) {
				var_end = endp;
			}
			mz->body_val = matchzone_value_new(memory, start, var_end, true);
			if (!mz->body_val) {
				return false;
			}
			start = var_end;
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_HEADERS_X)) {
			if (mz->headers != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->headers = NAXSI_MZ_TYPE_REGEX;
			start += strlen(NAXSI_RULE_MATCHZONE_HEADERS_X);
			if (!(var_end = strchr(start, '|'))) {
				var_end = endp;
			}
			mz->headers_val = matchzone_value_new(memory, start, var_end, true);
			if (!mz->headers_val) {
				return false;
			}
			start = var_end;
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_URL_X)) {
			if (mz->url != NAXSI_MZ_TYPE_UNSET && mz->url != NAXSI_MZ_TYPE_GLOBAL) {
				return false;
			}
			mz->url = NAXSI_MZ_TYPE_REGEX;
			start += strlen(NAXSI_RULE_MATCHZONE_URL_X);
			if (!(var_end = strchr(start, '|'))) {
				var_end = endp;
			}
			mz->url_val = matchzone_value_new(memory, start, var_end, true);
			if (!mz->url_val) {
				return false;
			}
			start = var_end;
			continue;
		}
		return false;
	}

	if (mz->args == NAXSI_MZ_TYPE_UNSET &&
		mz->body == NAXSI_MZ_TYPE_UNSET &&
		mz->file_ext == NAXSI_MZ_TYPE_UNSET &&
		mz->headers == NAXSI_MZ_TYPE_UNSET &&
		mz->raw_body == NAXSI_MZ_TYPE_UNSET &&
		mz->url == NAXSI_MZ_TYPE_UNSET) {
		return false;
	} else if (mz->name) {
		if (mz->args == NAXSI_MZ_TYPE_UNSET &&
			mz->body == NAXSI_MZ_TYPE_UNSET &&
			mz->headers == NAXSI_MZ_TYPE_UNSET) {
			return false;
		} else if (mz->file_ext != NAXSI_MZ_TYPE_UNSET || mz->raw_body != NAXSI_MZ_TYPE_UNSET) {
			return false;
		} else if (mz->url == NAXSI_MZ_TYPE_GLOBAL) {
			return false;
		}
	}

	return true;
}

/**
 * @brief      Frees a naxsi_rule_t pointer
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param      rule    The naxsi_rule_t to be freed
 */
void naxsi_rule_free(const naxsi_mem_t *memory, naxsi_rule_t *rule) {
	if (!memory || !rule) {
		return;
	}
	naxsi_str_free(memory, rule->description, true);
	naxsi_scores_free(memory, rule->scores);
	if (rule->mtype == NAXSI_MATCH_TYPE_STRING) {
		naxsi_str_free(memory, (naxsi_str_t *)rule->match, true);
	} else if (rule->mtype == NAXSI_MATCH_TYPE_REGEX) {
		naxsi_regex_free(memory, (naxsi_regex_t *)rule->match);
	}
	matchzone_fini(memory, &rule->matchzone);
	naxsi_mem_free(memory, rule);
}

/**
 * @brief      Copies a string without the prefix
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param      string  The string to copy
 * @param[in]  prefix  The prefix to skip
 *
 * @return     On success returns the copied string, otherwise NULL
 */
static naxsi_str_t *string_copy_without_prefix(const naxsi_mem_t *memory, naxsi_str_t *string, const char *prefix) {
	size_t prefix_length = strlen(prefix);
	if (!string || string->length <= prefix_length) {
		return NULL;
	}
	return naxsi_str_new(memory, string->data + prefix_length, string->length - prefix_length, true);
}

/**
 * @brief      Finds a score name end char and returns its pointer.
 *
 * @param      beginp  The begin pointer
 * @param      endp    The end pointer
 *
 * @return     On fail returns NULL, otherwise a valid pointer
 */
static char *find_score_name_end(char *beginp, char const *endp) {
	for (char *p = beginp; p < endp; ++p) {
		if (*p == ':') {
			// do not allow variables named "$"
			return p > beginp ? p : NULL;
		} else if (!isalpha(*p) && *p != '_') {
			break;
		}
	}
	return NULL;
}

/**
 * @brief      Finds a number end char and returns its pointer.
 *
 * @param      beginp  The begin pointer
 * @param      endp    The end pointer
 *
 * @return     On fail returns NULL, otherwise a valid pointer
 */
static char *find_number_end(char *beginp, char const *endp) {
	char *p = NULL;
	for (p = beginp; p < endp; ++p) {
		if (*p == ',' || !(*p)) {
			// do not allow empty strings
			return p > beginp ? p : NULL;
		} else if (!isdigit(*p)) {
			break;
		}
	}
	return p == endp ? p : NULL;
}

/**
 * @brief      Parses a base 10 number in string form between beginp and endp
 *
 * @param      beginp  The begin pointer
 * @param      endp    The end pointer
 *
 * @return     Returns 0 if overflow, otherwise the parsed number
 */
static u32 parse_number(char *beginp, char const *endp) {
	u32 num = 0, old = 0;
	for (char *p = beginp; p < endp; ++p) {
		old = num;
		num *= 10;
		num += (*p) - '0';
		if (num < old) {
			// overflow
			return 0;
		}
	}
	return num;
}

/**
 * @brief      Frees a naxsi_score_t pointer
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param      score   The score pointer to be freed
 */
static void score_free(const naxsi_mem_t *memory, naxsi_score_t *score) {
	if (!memory || !score) {
		return;
	}
	naxsi_str_fini(memory, &score->name, true);
	naxsi_mem_free(memory, score);
}

/**
 * @brief      Allocates and initializes a naxsi_score_t struct
 *
 * @param[in]  memory  The naxsi_mem_t structure to use
 * @param[in]  value   The score value
 * @param      name    The score name
 * @param[in]  length  The score name length
 *
 * @return     On success returns a valid pointer, otherwise NULL
 */
static naxsi_score_t *score_new(const naxsi_mem_t *memory, u32 value, char *name, size_t length) {
	naxsi_score_t *score = naxsi_mem_new(memory, naxsi_score_t);
	if (!score || !naxsi_str_init(memory, &score->name, name, length, true)) {
		naxsi_mem_free(memory, score);
		return false;
	}
	score->value = value;
	return score;
}

/**
 * @brief      Parses the scores and sets the rule scores & action
 *
 * @param[in]  memory    The naxsi_mem_t structure to use
 * @param      scores_s  The scores string
 * @param      rule      The rule to use
 *
 * @return     On success true, otherwise false
 */
static bool parse_scores(const naxsi_mem_t *memory, naxsi_str_t *scores_s, naxsi_rule_t *rule) {
	rule->action = NAXSI_RULE_ACTION_UNSET;
	rule->scores = naxsi_scores_new(memory, 5, (naxsi_free_t)&score_free);
	if (!rule->scores) {
		return false;
	}
	u32 value;
	char const *endp = scores_s->data + scores_s->length;
	for (char *start = scores_s->data + strlen(NAXSI_RULE_PREFIX_SCORES), *name_end = NULL, *num_end = NULL; start < endp;) {
		if (*start == ',') {
			start++;
			continue;
		} else if (*start == '$') {
			name_end = find_score_name_end(start + 1, endp);
			if (!name_end) {
				return false;
			}
			num_end = find_number_end(name_end + 1, endp);
			if (!num_end) {
				return false;
			}
			value = parse_number(name_end + 1, num_end);
			if (value < 1) {
				return false;
			}
			size_t length = name_end - start;

			naxsi_score_t *score = score_new(memory, value, start, length);
			if (!score || !naxsi_scores_push(memory, rule->scores, score)) {
				score_free(memory, score);
				return false;
			}
			start = num_end;
			rule->action |= NAXSI_RULE_ACTION_SCORE;
			continue;
		} else if (starts_with(start, NAXSI_RULE_SCORE_BLOCK)) {
			rule->action |= NAXSI_RULE_ACTION_BLOCK;
			start += strlen(NAXSI_RULE_SCORE_BLOCK);
			continue;
		} else if (starts_with(start, NAXSI_RULE_SCORE_ALLOW)) {
			rule->action |= NAXSI_RULE_ACTION_ALLOW;
			start += strlen(NAXSI_RULE_SCORE_ALLOW);
			continue;
		} else if (starts_with(start, NAXSI_RULE_SCORE_DROP)) {
			rule->action |= NAXSI_RULE_ACTION_DROP;
			start += strlen(NAXSI_RULE_SCORE_DROP);
			continue;
		} else if (starts_with(start, NAXSI_RULE_SCORE_LOG)) {
			rule->action |= NAXSI_RULE_ACTION_LOG;
			start += strlen(NAXSI_RULE_SCORE_LOG);
			continue;
		}
		return false;
	}
	if (rule->action == NAXSI_RULE_ACTION_UNSET) {
		return false;
	} else if (naxsi_is_set(rule->action, NAXSI_RULE_ACTION_BLOCK) && naxsi_is_set(rule->action, NAXSI_RULE_ACTION_ALLOW)) {
		return false;
	} else if (naxsi_is_set(rule->action, NAXSI_RULE_ACTION_BLOCK) && naxsi_is_set(rule->action, NAXSI_RULE_ACTION_DROP)) {
		return false;
	} else if (naxsi_is_set(rule->action, NAXSI_RULE_ACTION_ALLOW) && naxsi_is_set(rule->action, NAXSI_RULE_ACTION_DROP)) {
		return false;
	}
	if (rule->scores->length < 1) {
		naxsi_scores_free(memory, rule->scores);
		rule->scores = NULL;
	}
	return true;
}

/**
 * @brief      Allocates and initializes a naxsi_rule_t structure with the given params
 *
 * @param[in]  memory       The naxsi_mem_t structure to use
 * @param      id_s         The rule identifier
 * @param      descr_s      The rule description
 * @param      match_s      The rule match
 * @param      matchzone_s  The rule matchzone
 * @param      scores_s     The rule score or action
 * @param[in]  negative     When set to true, negates the match
 *
 * @return     On success returns the pointer to naxsi_rule_t, otherwise NULL
 */
naxsi_rule_t *naxsi_rule_new(const naxsi_mem_t *memory, naxsi_str_t *id_s, naxsi_str_t *descr_s, naxsi_str_t *match_s, naxsi_str_t *matchzone_s, naxsi_str_t *scores_s, bool negative) {
	if (!memory || !id_s || !match_s || !matchzone_s || !scores_s) {
		return NULL;
	}

	naxsi_rule_t *rule = naxsi_mem_new(memory, naxsi_rule_t);
	if (!rule) {
		return NULL;
	}

	rule->negative = negative;
	rule->description = string_copy_without_prefix(memory, descr_s, NAXSI_RULE_PREFIX_DESCRIPTION);

	rule->identifier = parse_number(id_s->data + strlen(NAXSI_RULE_PREFIX_IDENTIFIER), id_s->data + id_s->length);
	if (rule->identifier < 1000) {
		/* IDs between 0 - 999 are reserved internally */
		goto fail;
	}

	if (starts_with(match_s->data, NAXSI_RULE_PREFIX_MATCH_STRING)) {
		rule->mtype = NAXSI_MATCH_TYPE_STRING;
		rule->match = (void *)string_copy_without_prefix(memory, match_s, NAXSI_RULE_PREFIX_MATCH_STRING);
		if (!rule->match) {
			goto fail;
		}
	} else if (starts_with(match_s->data, NAXSI_RULE_PREFIX_MATCH_REGEX)) {
		rule->mtype = NAXSI_MATCH_TYPE_REGEX;
		rule->match = (void *)naxsi_regex_new(memory, match_s->data + strlen(NAXSI_RULE_PREFIX_MATCH_REGEX));
		if (!rule->match) {
			goto fail;
		}
	} else if (starts_with(match_s->data, NAXSI_RULE_LIBINJECTION_XSS)) {
		rule->mtype = NAXSI_MATCH_TYPE_XSS;
	} else if (starts_with(match_s->data, NAXSI_RULE_LIBINJECTION_SQL)) {
		rule->mtype = NAXSI_MATCH_TYPE_SQL;
	} else {
		goto fail;
	}

	if (!parse_scores(memory, scores_s, rule)) {
		goto fail;
	}

	if (!parse_matchzone(memory, matchzone_s, &rule->matchzone)) {
		goto fail;
	}

	return rule;

fail:
	naxsi_rule_free(memory, rule);
	return NULL;
}

/**
 * @brief      Frees a naxsi_whitelist_t pointer
 *
 * @param[in]  memory     The naxsi_mem_t structure to use
 * @param      whitelist  The naxsi_whitelist_t to be freed
 */
void naxsi_whitelist_free(const naxsi_mem_t *memory, naxsi_whitelist_t *whitelist) {
	if (!memory || !whitelist) {
		return;
	}
	naxsi_ids_free(memory, whitelist->ids);
	matchzone_fini(memory, &whitelist->matchzone);
	naxsi_mem_free(memory, whitelist);
}

/**
 * @brief      Parses the list of rule identifiers that needs to be whitelisted
 *
 * @param[in]  memory       The naxsi_mem_t structure to use
 * @param      whitelist_s  The list of ids to whitelist in string format
 * @param      whitelist    The naxsi_whitelist_t to use
 *
 * @return     On success true, otherwise false
 */
static bool parse_whitelist_ids(const naxsi_mem_t *memory, naxsi_str_t *whitelist_s, naxsi_whitelist_t *whitelist) {
	u32 identifier = 0, count = 0;
	char const *endp = whitelist_s->data + whitelist_s->length;
	for (char *start = whitelist_s->data + strlen(NAXSI_RULE_PREFIX_WHITELIST); start < endp;) {
		if (*start == ',') {
			start++;
			continue;
		} else if (!isdigit(*start)) {
			return false;
		}
		start = find_number_end(start, endp);
		if (!start) {
			return false;
		}
		count++;
	}

	whitelist->ids = naxsi_ids_new(memory, count);
	if (!whitelist->ids) {
		return false;
	}

	for (char *start = whitelist_s->data + strlen(NAXSI_RULE_PREFIX_WHITELIST), *num_end = NULL; start < endp;) {
		if (*start == ',') {
			start++;
			continue;
		} else if (!isdigit(*start)) {
			return false;
		}
		num_end = find_number_end(start, endp);
		if (!num_end) {
			return false;
		}
		identifier = parse_number(start, num_end);
		if (identifier < 1 || !naxsi_ids_push(memory, whitelist->ids, identifier)) {
			return false;
		}
		start = num_end;
	}
	return true;
}

/**
 * @brief      Allocates and initialize a naxsi_whitelist_t struct
 *
 * @param[in]  memory       The naxsi_mem_t structure to use
 * @param      whitelist_s  The list of ids to whitelist
 * @param      matchzone_s  The whitelist matchzone
 * @param[in]  negative     When set to true, negates the match
 *
 * @return     On success returns the pointer to naxsi_whitelist_t, otherwise NULL
 */
naxsi_whitelist_t *naxsi_whitelist_new(const naxsi_mem_t *memory, naxsi_str_t *whitelist_s, naxsi_str_t *matchzone_s, bool negative) {
	if (!memory || !whitelist_s || !matchzone_s) {
		return NULL;
	}

	naxsi_whitelist_t *whitelist = naxsi_mem_new(memory, naxsi_whitelist_t);
	if (!whitelist) {
		return NULL;
	}

	whitelist->negative = negative;

	if (!parse_matchzone(memory, matchzone_s, &whitelist->matchzone)) {
		goto fail;
	}

	if (!parse_whitelist_ids(memory, whitelist_s, whitelist)) {
		goto fail;
	}

	return whitelist;

fail:
	naxsi_whitelist_free(memory, whitelist);
	return NULL;
}

/**
 * @brief      Finds checkrule named variable end
 *
 * @param      checkrule_s  The checkrule string to parse
 *
 * @return     On success returns valid pointer, otherwise NULL
 */
static char *find_checkrule_name_end(naxsi_str_t *checkrule_s) {
	if (checkrule_s->length < 4 || *checkrule_s->data != '$') {
		return NULL;
	}
	char *beginp = checkrule_s->data + 1;
	char const *endp = checkrule_s->data + checkrule_s->length;
	for (char *start = beginp; start < endp; start++) {
		if (*start == ' ' || *start == '<' || *start == '>') {
			return start > beginp ? start : NULL;
		} else if (!isalpha(*start) && *start != '_') {
			break;
		}
	}
	return NULL;
}

/**
 * @brief      Finds checkrule compare operation
 *
 * @param      checkrule_s  The checkrule string to parse
 * @param      beginp       The start position
 *
 * @return     On success returns valid pointer, otherwise NULL
 */
static char *find_checkrule_compare(naxsi_str_t *checkrule_s, char *beginp) {
	char const *endp = checkrule_s->data + checkrule_s->length;
	for (char *start = beginp; start < endp; start++) {
		if (*start == '<' || *start == '>') {
			return start;
		} else if (*start != ' ') {
			break;
		}
	}
	return NULL;
}

/**
 * @brief      Finds checkrule numeric score value
 *
 * @param      checkrule_s  The checkrule string to parse
 * @param      beginp       The start position
 *
 * @return     On success returns valid pointer, otherwise NULL
 */
static char *find_checkrule_score(naxsi_str_t *checkrule_s, char *beginp) {
	char const *endp = checkrule_s->data + checkrule_s->length;
	for (char *start = beginp; start < endp; start++) {
		if (isdigit(*start)) {
			return start;
		} else if (*start != ' ') {
			break;
		}
	}
	return NULL;
}

/**
 * @brief      Allocates and initialize a naxsi_checkrule_t struct
 *
 * @param[in]  memory       The naxsi_mem_t structure to use
 * @param      checkrule_s  The checkrule
 * @param      action_s     The action linked to the checkrule
 *
 * @return     On success returns the pointer to naxsi_checkrule_t, otherwise NULL
 */
naxsi_checkrule_t *naxsi_checkrule_new(const naxsi_mem_t *memory, naxsi_str_t *checkrule_s, naxsi_str_t *action_s) {
	if (!memory || !checkrule_s || !action_s) {
		return NULL;
	}
	u32 value = 0;
	naxsi_cr_compare_t compare = NAXSI_CHECKRULE_COMPARE_INVALID;
	naxsi_cr_action_t action = NAXSI_CHECKRULE_ACTION_UNSET;
	char *name_end = NULL, *cmp = NULL, *num_beg = NULL, *num_end = NULL;

	if (action_s->length < 3 || action_s->length > 5 || !*action_s->data) {
		return NULL;
	} else if (!strcmp(action_s->data, NAXSI_RULE_SCORE_BLOCK)) {
		action = NAXSI_CHECKRULE_ACTION_BLOCK;
	} else if (!strcmp(action_s->data, NAXSI_RULE_SCORE_ALLOW)) {
		action = NAXSI_CHECKRULE_ACTION_ALLOW;
	} else if (!strcmp(action_s->data, NAXSI_RULE_SCORE_DROP)) {
		action = NAXSI_CHECKRULE_ACTION_DROP;
	} else if (!strcmp(action_s->data, NAXSI_RULE_SCORE_LOG)) {
		action = NAXSI_CHECKRULE_ACTION_LOG;
	} else {
		return NULL;
	}

	name_end = find_checkrule_name_end(checkrule_s);
	if (!name_end) {
		return NULL;
	}

	cmp = find_checkrule_compare(checkrule_s, name_end);
	if (!cmp) {
		return NULL;
	} else if (starts_with(cmp, "<=")) {
		compare = NAXSI_CHECKRULE_COMPARE_LE;
		cmp += strlen("<=");
	} else if (starts_with(cmp, ">=")) {
		compare = NAXSI_CHECKRULE_COMPARE_GE;
		cmp += strlen(">=");
	} else if (starts_with(cmp, "<")) {
		compare = NAXSI_CHECKRULE_COMPARE_LT;
		cmp += strlen("<");
	} else if (starts_with(cmp, ">")) {
		compare = NAXSI_CHECKRULE_COMPARE_GT;
		cmp += strlen(">");
	} else {
		return NULL;
	}

	num_beg = find_checkrule_score(checkrule_s, cmp);
	if (!num_beg) {
		return NULL;
	}

	num_end = find_number_end(num_beg, checkrule_s->data + checkrule_s->length);
	if (!num_end) {
		return false;
	}
	value = parse_number(num_beg, num_end);

	naxsi_checkrule_t *checkrule = naxsi_mem_new(memory, naxsi_checkrule_t);
	if (!checkrule || !naxsi_str_init(memory, &checkrule->name, checkrule_s->data, name_end - checkrule_s->data, true)) {
		naxsi_mem_free(memory, checkrule);
		return false;
	}
	checkrule->value = value;
	checkrule->compare = compare;
	checkrule->action = action;
	return checkrule;
}

/**
 * @brief      Frees a naxsi_checkrule_t pointer
 *
 * @param[in]  memory     The naxsi_mem_t structure to use
 * @param      checkrule  The naxsi_checkrule_t to be freed
 */
void naxsi_checkrule_free(const naxsi_mem_t *memory, naxsi_checkrule_t *checkrule) {
	if (!memory || !checkrule) {
		return;
	}
	naxsi_str_fini(memory, &checkrule->name, true);
	naxsi_mem_free(memory, checkrule);
}
