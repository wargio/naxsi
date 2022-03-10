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

#define naxsi_rule_has_flag(f, y) (f & y)

typedef enum {
	NAXSI_MATCH_TYPE_STRING /**/ = (1 << 0), ///< matches case-insensitive strings when "str:foo" is set
	NAXSI_MATCH_TYPE_REGEX /* */ = (1 << 1), ///< matches case-insensitive strings when "rx:foo|bar" is set
	NAXSI_MATCH_TYPE_XSS /*   */ = (1 << 2), ///< matches case-insensitive strings when "d:libinj_xss" is set
	NAXSI_MATCH_TYPE_SQL /*   */ = (1 << 3), ///< matches case-insensitive strings when "d:libinj_sql" is set
} naxsi_mtype_t;

typedef enum {
	NAXSI_MZ_TYPE_UNSET /* */ = (0 << 0), ///< used only when the matchzone variable is unset
	NAXSI_MZ_TYPE_GLOBAL /**/ = (1 << 0), ///< describes a global matchzone: BODY (parsed), URL, ARGS, HEADERS, FILE_EXT or RAW_BODY
	NAXSI_MZ_TYPE_STRING /**/ = (1 << 1), ///< describes a specific matchzone: $URL, $ARGS_VAR, $HEADERS_VAR, $BODY_VAR
	NAXSI_MZ_TYPE_REGEX /* */ = (1 << 2), ///< describes a regex matchzone: $URL_X, $ARGS_VAR_X, $HEADERS_VAR_X, $BODY_VAR_X
} naxsi_mz_type_t;

typedef enum {
	NAXSI_RULE_ACTION_SCORE /**/ = (0 << 0), ///< increments the scores when the rules matches
	NAXSI_RULE_ACTION_BLOCK /**/ = (1 << 0), ///< directly blocks the request when the rule matches and is not in learning mode
	NAXSI_RULE_ACTION_ALLOW /**/ = (1 << 1), ///< directly allows the request when the rule matches
	NAXSI_RULE_ACTION_DROP /* */ = (1 << 2), ///< directly drop the request when the rule matches, regardless of the current mode
	NAXSI_RULE_ACTION_LOG /*  */ = (1 << 3), ///< directly logs the request when the rule matches
} naxsi_action_t;

typedef struct naxsi_score {
	naxsi_str_t name; ///< score name, for example: $ATTACK (s:$ATTACK:8)
	u32 score; ///< score value to add to checkrule score, example: 8 (s:$ATTACK:8)
} naxsi_score_t;

naxsi_svector(scores, naxsi_score_t); // naxsi_svector_t<naxsi_score_t>

typedef struct naxsi_matchzone {
	u8 args; ///< naxsi_mz_type_t which describes ARGS, $ARGS_VAR or $ARGS_VAR_X matchzone
	u8 body; ///< naxsi_mz_type_t which describes BODY matchzone
	u8 file_ext; ///< naxsi_mz_type_t which describes FILE_EXT matchzone
	u8 headers; ///< naxsi_mz_type_t which describes ARGS, $ARGS_VAR or $ARGS_VAR_X matchzone
	u8 raw_body; ///< naxsi_mz_type_t which describes RAW_BODY matchzone
	u8 url; ///< naxsi_mz_type_t which describes URL, $URL or $URL_X matchzone

	void *args_val; ///< describes $ARGS_VAR:string or $ARGS_VAR_X:regex matchzone value
	void *headers_val; ///< describes $ARGS_VAR:string or $ARGS_VAR_X:regex matchzone value
	void *url_val; ///< describes $URL:string or $URL_X:regex matchzone value
} naxsi_matchzone_t;

naxsi_vector(ids, u32); // naxsi_vector_t<u32>

typedef struct naxsi_whitelist {
	naxsi_ids_t *ids; ///< rule ids to whitelist, example: wl:10,32033,2000
	naxsi_matchzone_t matchzone; ///< matchzone to match when whitelisting, example: "mz:URL|BODY|ARGS"
	bool negate; ///< negates the match result, so matching becomes not matching; example: negative
} naxsi_whitelist_t;

typedef struct naxsi_rule {
	naxsi_str_t *description; ///< describes the rule description, example: "msg:GIT-Homedir-Access"
	u32 rule_id; ///< describes the rule id, example: id:42000329
	naxsi_action_t action; ///< describes an action instead of a score, example: s:DROP
	naxsi_scores_t *scores; ///<  describes the scores of the rule, example: s:$ATTACK:8,$UWA:8
	void *match; ///< describes a case-insensitive of string to match, or a regex or the libinjection XSS/SQL; example: "rx:^[a-zA-Z\d-]+$" or "str:/.git/"
	naxsi_mtype_t mtype; ///< describes the type of match in the rule
	naxsi_matchzone_t matchzone; ///< describes the matchzone of the rule, example: "mz:URL|BODY|ARGS"
	bool negate; ///< negates the match result, so matching becomes not matching; example: negative
} naxsi_rule_t;

#define naxsi_rule_new(memory) naxsi_mem_new0(memory, naxsi_rule_t)
void naxsi_rule_free(const naxsi_mem_t *memory, naxsi_rule_t *rule);
#define naxsi_whitelist_new(memory) naxsi_mem_new0(memory, naxsi_rule_t)
void naxsi_whitelist_free(const naxsi_mem_t *memory, naxsi_whitelist_t *whitelist);

#endif /* NAXSI_RULE_H */
