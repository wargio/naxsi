// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_RULE_H
#define NAXSI_RULE_H

#include <naxsi_types.h>
#include "naxsi_str.h"
#include "naxsi_regex.h"
#include "naxsi_vector.h"

#define NAXSI_MAIN_RULE_ID_WEIRD_REQUEST         (1) // not really used anywhere, added for historic reasons.
#define NAXSI_MAIN_RULE_ID_TOO_BIG_REQUEST       (2)
#define NAXSI_MAIN_RULE_ID_INVALID_HEX           (10)
#define NAXSI_MAIN_RULE_ID_UNKNOWN_CONTENT_TYPE  (11)
#define NAXSI_MAIN_RULE_ID_INVALID_FORMATTED_URL (11)
#define NAXSI_MAIN_RULE_ID_INVALID_POST_FORMAT   (13)
#define NAXSI_MAIN_RULE_ID_INVALID_POST_BOUNDARY (14)
#define NAXSI_MAIN_RULE_ID_INVALID_JSON          (15)
#define NAXSI_MAIN_RULE_ID_EMPTY_POST            (16)
#define NAXSI_MAIN_RULE_ID_LIBINJECTION_SQL      (17)
#define NAXSI_MAIN_RULE_ID_LIBINJECTION_XSS      (18)
#define NAXSI_MAIN_RULE_ID_NO_GENERIC_RULES      (19)
#define NAXSI_MAIN_RULE_ID_BAD_UTF8              (20)

#define NAXSI_RULE_PREFIX_IDENTIFIER   "id:"
#define NAXSI_RULE_PREFIX_DESCRIPTION  "msg:"
#define NAXSI_RULE_PREFIX_MATCH_STRING "str:"
#define NAXSI_RULE_PREFIX_MATCH_REGEX  "rx:"
#define NAXSI_RULE_PREFIX_MATCHZONE    "mz:"
#define NAXSI_RULE_PREFIX_SCORES       "s:"
#define NAXSI_RULE_PREFIX_WHITELIST    "wl:"
#define NAXSI_RULE_NEGATIVE            "negative"
#define NAXSI_RULE_LIBINJECTION_XSS    "d:libinj_xss"
#define NAXSI_RULE_LIBINJECTION_SQL    "d:libinj_sql"
#define NAXSI_RULE_SCORE_BLOCK         "BLOCK"
#define NAXSI_RULE_SCORE_ALLOW         "ALLOW"
#define NAXSI_RULE_SCORE_DROP          "DROP"
#define NAXSI_RULE_SCORE_LOG           "LOG"
/* Matchzone modifier for BODY, HEADERS, ARGS */
#define NAXSI_RULE_MATCHZONE_MOD_NAME "NAME"
/* Matchzone global */
#define NAXSI_RULE_MATCHZONE_ARGS     "ARGS"
#define NAXSI_RULE_MATCHZONE_BODY     "BODY"
#define NAXSI_RULE_MATCHZONE_FILE_EXT "FILE_EXT"
#define NAXSI_RULE_MATCHZONE_HEADERS  "HEADERS"
#define NAXSI_RULE_MATCHZONE_RAW_BODY "RAW_BODY"
#define NAXSI_RULE_MATCHZONE_URL      "URL"
/* Matchzone specific case-insensitive string */
#define NAXSI_RULE_MATCHZONE_ARGS_S    "$ARGS_VAR:"
#define NAXSI_RULE_MATCHZONE_BODY_S    "$BODY_VAR:"
#define NAXSI_RULE_MATCHZONE_HEADERS_S "$HEADERS_VAR:"
#define NAXSI_RULE_MATCHZONE_URL_S     "$URL:"
/* Matchzone specific case-insensitive regex */
#define NAXSI_RULE_MATCHZONE_ARGS_X    "$ARGS_VAR_X:"
#define NAXSI_RULE_MATCHZONE_BODY_X    "$BODY_VAR_X:"
#define NAXSI_RULE_MATCHZONE_HEADERS_X "$HEADERS_VAR_X:"
#define NAXSI_RULE_MATCHZONE_URL_X     "$URL_X:"

typedef enum {
	NAXSI_MATCH_TYPE_UNSET = 0, ///< unused
	NAXSI_MATCH_TYPE_STRING, ///< matches case-insensitive strings when "str:foo" is set
	NAXSI_MATCH_TYPE_REGEX, ///< matches case-insensitive strings when "rx:foo|bar" is set
	NAXSI_MATCH_TYPE_XSS, ///< matches case-insensitive strings when "d:libinj_xss" is set
	NAXSI_MATCH_TYPE_SQL, ///< matches case-insensitive strings when "d:libinj_sql" is set
} naxsi_mtype_t;

typedef enum {
	NAXSI_MZ_TYPE_UNSET = 0, ///< used only when the matchzone variable is unset
	NAXSI_MZ_TYPE_GLOBAL, ///< describes a global matchzone: BODY (parsed), URL, ARGS, HEADERS, FILE_EXT or RAW_BODY
	NAXSI_MZ_TYPE_STRING, ///< describes a specific matchzone: $URL, $ARGS_VAR, $HEADERS_VAR, $BODY_VAR
	NAXSI_MZ_TYPE_REGEX, ///< describes a regex matchzone: $URL_X, $ARGS_VAR_X, $HEADERS_VAR_X, $BODY_VAR_X
} naxsi_mz_type_t;

typedef enum {
	NAXSI_RULE_ACTION_UNSET /**/ = (0 << 0), ///< used for invalid rule action
	NAXSI_RULE_ACTION_SCORE /**/ = (1 << 0), ///< increments the scores when the rules matches
	NAXSI_RULE_ACTION_BLOCK /**/ = (1 << 1), ///< directly blocks the request when the rule matches and is not in learning mode
	NAXSI_RULE_ACTION_ALLOW /**/ = (1 << 2), ///< directly allows the request when the rule matches
	NAXSI_RULE_ACTION_DROP /* */ = (1 << 3), ///< directly drop the request when the rule matches, regardless of the current mode
	NAXSI_RULE_ACTION_LOG /*  */ = (1 << 4), ///< directly logs the request when the rule matches
} naxsi_action_t;

typedef enum {
	NAXSI_CHECKRULE_COMPARE_INVALID = 0, ///< used for invalid checkrule action
	NAXSI_CHECKRULE_COMPARE_LT, ///< $FOO <  123
	NAXSI_CHECKRULE_COMPARE_LE, ///< $FOO <= 123
	NAXSI_CHECKRULE_COMPARE_GT, ///< $FOO >  123
	NAXSI_CHECKRULE_COMPARE_GE, ///< $FOO >= 123
} naxsi_cr_compare_t;

typedef enum {
	NAXSI_CHECKRULE_ACTION_UNSET = 0, ///< used for invalid checkrule action
	NAXSI_CHECKRULE_ACTION_BLOCK, ///< directly blocks the request when the rule matches and is not in learning mode
	NAXSI_CHECKRULE_ACTION_ALLOW, ///< directly allows the request when the rule matches
	NAXSI_CHECKRULE_ACTION_DROP, ///< directly drop the request when the rule matches, regardless of the current mode
	NAXSI_CHECKRULE_ACTION_LOG, ///< directly logs the request when the rule matches
} naxsi_cr_action_t;

typedef struct naxsi_score {
	naxsi_str_t name; ///< score name, for example: $ATTACK (s:$ATTACK:8)
	u32 value; ///< score value to add to checkrule score, example: 8 (s:$ATTACK:8)
} naxsi_score_t;

naxsi_pvector(scores, naxsi_score_t); // naxsi_pvector_t<naxsi_score_t*>

typedef struct naxsi_matchzone {
	bool name; ///< describes the NAME modifier for the matchzone

	u8 args; ///< naxsi_mz_type_t which describes ARGS, $ARGS_VAR or $ARGS_VAR_X matchzone
	u8 body; ///< naxsi_mz_type_t which describes BODY matchzone
	u8 file_ext; ///< naxsi_mz_type_t which describes FILE_EXT matchzone
	u8 headers; ///< naxsi_mz_type_t which describes HEADERS, $HEADERS_VAR or $HEADERS_VAR_X matchzone
	u8 raw_body; ///< naxsi_mz_type_t which describes RAW_BODY matchzone
	u8 url; ///< naxsi_mz_type_t which describes URL, $URL or $URL_X matchzone

	void *args_val; ///< describes $ARGS_VAR:string or $ARGS_VAR_X:regex matchzone value
	void *body_val; ///< describes $BODY_VAR:string or $BODY_VAR_X:regex matchzone value
	void *headers_val; ///< describes $ARGS_VAR:string or $ARGS_VAR_X:regex matchzone value
	void *url_val; ///< describes $URL:string or $URL_X:regex matchzone value
} naxsi_matchzone_t;

naxsi_vector(ids, u32); // naxsi_vector_t<u32>

typedef struct naxsi_whitelist {
	naxsi_ids_t *ids; ///< rule ids to whitelist, example: wl:10,32033,2000
	naxsi_matchzone_t matchzone; ///< matchzone to match when whitelisting, example: "mz:URL|BODY|ARGS"
	bool negative; ///< negates the match result, so matching becomes not matching; example: negative
} naxsi_whitelist_t;

typedef struct naxsi_rule {
	naxsi_str_t *description; ///< describes the rule description, example: "msg:GIT-Homedir-Access"
	u32 identifier; ///< describes the rule id, example: id:42000329
	naxsi_action_t action; ///< describes an action instead of a score, example: s:DROP
	naxsi_scores_t *scores; ///<  describes the scores of the rule, example: s:$ATTACK:8,$UWA:8
	void *match; ///< describes a case-insensitive of string to match, or a regex or the libinjection XSS/SQL; example: "rx:^[a-zA-Z\d-]+$" or "str:/.git/"
	naxsi_mtype_t mtype; ///< describes the type of match in the rule
	naxsi_matchzone_t matchzone; ///< describes the matchzone of the rule, example: "mz:URL|BODY|ARGS"
	bool negative; ///< negates the match result, so matching becomes not matching; example: negative
} naxsi_rule_t;

typedef struct naxsi_checkrule {
	naxsi_str_t name; ///< check rule name, for example: '$ATTACK' in 'CheckRule "$ATTACK >= 8" BLOCK'
	u32 value; ///<  check rule value to compare with the computed score, example: '8' in 'CheckRule "$ATTACK >= 8" BLOCK'
	naxsi_cr_compare_t compare; ///< check rule compare type, example: '>=' in 'CheckRule "$ATTACK >= 8" BLOCK'
	naxsi_cr_action_t action; ///< check rule action type, example: 'BLOCK' in 'CheckRule "$ATTACK >= 8" BLOCK'
} naxsi_checkrule_t;

naxsi_rule_t *naxsi_rule_new(const naxsi_mem_t *memory, naxsi_str_t *id_s, naxsi_str_t *descr_s, naxsi_str_t *match_s, naxsi_str_t *matchzone_s, naxsi_str_t *scores_s, bool negative);
void naxsi_rule_free(const naxsi_mem_t *memory, naxsi_rule_t *rule);
naxsi_whitelist_t *naxsi_whitelist_new(const naxsi_mem_t *memory, naxsi_str_t *whitelist_s, naxsi_str_t *matchzone_s, bool negative);
void naxsi_whitelist_free(const naxsi_mem_t *memory, naxsi_whitelist_t *whitelist);
naxsi_checkrule_t *naxsi_checkrule_new(const naxsi_mem_t *memory, naxsi_str_t *checkrule_s, naxsi_str_t *action_s);
void naxsi_checkrule_free(const naxsi_mem_t *memory, naxsi_checkrule_t *checkrule);

#endif /* NAXSI_RULE_H */
