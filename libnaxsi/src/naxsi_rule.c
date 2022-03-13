// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_rule.h"
#include "naxsi_regex.h"
#include "naxsi_utils.h"
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>

#define starts_with(s, p) (!strncmp(s, p, strlen(p)))

naxsi_svector_define(scores, naxsi_score_t);
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

	for (char *start = mz_s->data, *var_end = NULL; start < endp;) {
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
			mz->args |= NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_ARGS);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_BODY)) {
			if (mz->body != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->body |= NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_BODY);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_FILE_EXT)) {
			if (mz->file_ext != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->file_ext |= NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_FILE_EXT);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_HEADERS)) {
			if (mz->headers != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->headers |= NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_HEADERS);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_RAW_BODY)) {
			if (mz->raw_body != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->raw_body |= NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_RAW_BODY);
			continue;
		} else if (starts_with(start, NAXSI_RULE_MATCHZONE_URL)) {
			if (mz->url != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->url |= NAXSI_MZ_TYPE_GLOBAL;
			start += strlen(NAXSI_RULE_MATCHZONE_URL);
			continue;
		}
		/* case-insensitive string */
		else if (starts_with(start, NAXSI_RULE_MATCHZONE_ARGS_S)) {
			if (mz->args != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->args |= NAXSI_MZ_TYPE_STRING;
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
			mz->body |= NAXSI_MZ_TYPE_STRING;
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
			mz->headers |= NAXSI_MZ_TYPE_STRING;
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
			if (mz->url != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->url |= NAXSI_MZ_TYPE_STRING;
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
			mz->args |= NAXSI_MZ_TYPE_REGEX;
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
			mz->body |= NAXSI_MZ_TYPE_REGEX;
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
			mz->headers |= NAXSI_MZ_TYPE_REGEX;
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
			if (mz->url != NAXSI_MZ_TYPE_UNSET) {
				return false;
			}
			mz->url |= NAXSI_MZ_TYPE_REGEX;
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
	if (mz->name) {
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
	return naxsi_str_new(memory, string->data + prefix_length, string->length + prefix_length, true);
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
 * @brief      Finds a score number end char and returns its pointer.
 *
 * @param      beginp  The begin pointer
 * @param      endp    The end pointer
 *
 * @return     On fail returns NULL, otherwise a valid pointer
 */
static char *find_score_number_end(char *beginp, char const *endp) {
	for (char *p = beginp; p < endp; ++p) {
		if (*p == ',' || !(*p)) {
			// do not allow variables named "$"
			return p > beginp ? p : NULL;
		} else if (!isdigit(*p)) {
			break;
		}
	}
	return NULL;
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
	rule->scores = naxsi_scores_new(memory, 5);
	if (!rule->scores) {
		return false;
	}
	naxsi_score_t score;
	char const *endp = scores_s->data + scores_s->length;
	for (char *start = scores_s->data, *name_end = NULL, *num_end = NULL; start < endp;) {
		if (*start == ',') {
			start++;
			continue;
		} else if (*start == '$') {
			name_end = find_score_name_end(start + 1, endp);
			if (!name_end) {
				return false;
			}
			num_end = find_score_number_end(name_end + 1, endp);
			if (!num_end) {
				return false;
			}
			score.score = parse_number(name_end + 1, num_end);
			if (score.score < 1) {
				return false;
			}
			size_t length = name_end - start;
			naxsi_str_init(memory, &score.name, start, length, true);
			if (!naxsi_scores_push(memory, rule->scores, &score)) {
				naxsi_str_fini(memory, &score.name, true);
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

	rule->rule_id = atoi(id_s->data + strlen(NAXSI_RULE_PREFIX_IDENTIFIER));
	if (rule->rule_id < 1000) {
		/* IDs between 0 - 999 are reserved internally */
		goto fail;
	}

	if (starts_with(match_s->data, NAXSI_RULE_PREFIX_MATCH_STRING)) {
		rule->mtype = NAXSI_MATCH_TYPE_STRING;
		rule->match = (void *)string_copy_without_prefix(memory, match_s, NAXSI_RULE_PREFIX_MATCH_STRING);
	} else if (starts_with(match_s->data, NAXSI_RULE_PREFIX_MATCH_REGEX)) {
		rule->mtype = NAXSI_MATCH_TYPE_REGEX;
		rule->match = (void *)naxsi_regex_new(memory, match_s->data + strlen(NAXSI_RULE_PREFIX_MATCH_REGEX));
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
