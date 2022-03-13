// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only
#include "minunit_mem.h"
#include "naxsi_rule.h"

#define test_rule(id, descr, match, matchzone, scores, neg) \
	do { \
		naxsi_str_init(&mem, &id_s, id, strlen(id), false); \
		naxsi_str_init(&mem, &descr_s, descr, strlen(descr), false); \
		naxsi_str_init(&mem, &match_s, match, strlen(match), false); \
		naxsi_str_init(&mem, &matchzone_s, matchzone, strlen(matchzone), false); \
		naxsi_str_init(&mem, &scores_s, scores, strlen(scores), false); \
		rule = naxsi_rule_new(&mem, &id_s, &descr_s, &match_s, &matchzone_s, &scores_s, neg); \
	} while (0)

#define test_rule_nodesc(id, match, matchzone, scores, neg) \
	do { \
		naxsi_str_init(&mem, &id_s, id, strlen(id), false); \
		naxsi_str_init(&mem, &match_s, match, strlen(match), false); \
		naxsi_str_init(&mem, &matchzone_s, matchzone, strlen(matchzone), false); \
		naxsi_str_init(&mem, &scores_s, scores, strlen(scores), false); \
		rule = naxsi_rule_new(&mem, &id_s, NULL, &match_s, &matchzone_s, &scores_s, neg); \
	} while (0)

#define test_whitelist(wl, matchzone, neg) \
	do { \
		naxsi_str_init(&mem, &wl_s, wl, strlen(wl), false); \
		naxsi_str_init(&mem, &matchzone_s, matchzone, strlen(matchzone), false); \
		whitelist = naxsi_whitelist_new(&mem, &wl_s, &matchzone_s, neg); \
	} while (0)

bool test_naxsi_rule_failures(void) {
	naxsi_str_t id_s = { 0 }, descr_s = { 0 }, match_s = { 0 }, matchzone_s = { 0 }, scores_s = { 0 };
	naxsi_rule_t *rule = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	rule = naxsi_rule_new(NULL, &id_s, &descr_s, &match_s, &matchzone_s, &scores_s, false);
	mu_assert_null(rule, "naxsi_rule_new(NULL, &id_s, &descr_s, &match_s, &matchzone_s, &scores_s, false) shall return NULL");

	rule = naxsi_rule_new(&mem, NULL, &descr_s, &match_s, &matchzone_s, &scores_s, false);
	mu_assert_null(rule, "naxsi_rule_new(&mem, NULL, &descr_s, &match_s, &matchzone_s, &scores_s, false) shall return NULL");

	rule = naxsi_rule_new(&mem, &id_s, &descr_s, NULL, &matchzone_s, &scores_s, false);
	mu_assert_null(rule, "naxsi_rule_new(&mem, &id_s, &descr_s, NULL, &matchzone_s, &scores_s, false) shall return NULL");

	rule = naxsi_rule_new(&mem, &id_s, &descr_s, &match_s, NULL, &scores_s, false);
	mu_assert_null(rule, "naxsi_rule_new(&mem, &id_s, &descr_s, &match_s, NULL, &scores_s, false) shall return NULL");

	rule = naxsi_rule_new(&mem, &id_s, &descr_s, &match_s, &matchzone_s, NULL, false);
	mu_assert_null(rule, "naxsi_rule_new(&mem, &id_s, &descr_s, &match_s, &matchzone_s, NULL, false) shall return NULL");

	test_rule("id:999", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has id < 1000");

	test_rule("id:1", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has id < 1000");

	test_rule_nodesc("id:42000329", "str:", "mz:URL|BODY|ARGS", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has empty string match");

	test_rule_nodesc("id:42000329", "rx:", "mz:URL|BODY|ARGS", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has empty regex match");

	test_rule_nodesc("id:42000329", "str:foo", "mz:", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has empty matchzone");

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL |BODY|ARGS", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone with a space");

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL| BODY|ARGS", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone with a space 2");

	test_rule_nodesc("id:42000329", "str:foo", "mz: ARGS", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone with a space 3");

	test_rule_nodesc("id:42000329", "str:foo", "mz:ARGS ", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone with a space 4");

	test_rule_nodesc("id:42000329", "str:foo", "mz:toto2192", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone has an invalid value");

	test_rule_nodesc("id:42000329", "str:foo", "mz:ARGS|$ARGS_VAR:foo", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone cannot set ARGS and $ARGS_VAR");

	test_rule_nodesc("id:42000329", "str:foo", "mz:HEADERS|$HEADERS_VAR:foo", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone cannot set HEADERS and $HEADERS_VAR");

	test_rule_nodesc("id:42000329", "str:foo", "mz:BODY|$BODY_VAR:foo", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone cannot set BODY and $BODY_VAR");

	test_rule_nodesc("id:42000329", "str:foo", "mz:ARGS|$ARGS_VAR_X:^foo$", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone cannot set ARGS and $ARGS_VAR_X");

	test_rule_nodesc("id:42000329", "str:foo", "mz:HEADERS|$HEADERS_VAR_X:^foo$", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone cannot set HEADERS and $HEADERS_VAR_X");

	test_rule_nodesc("id:42000329", "str:foo", "mz:BODY|$BODY_VAR_X:^foo$", "s:$FOO:8", true);
	mu_assert_null(rule, "naxsi rule has matchzone cannot set BODY and $BODY_VAR_X");

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL|BODY|ARGS", "s:", true);
	mu_assert_null(rule, "naxsi rule has empty score");

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL|BODY|ARGS", "s:$FOO:", true);
	mu_assert_null(rule, "naxsi rule has score but without value 1");

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL|BODY|ARGS", "s:$FOO", true);
	mu_assert_null(rule, "naxsi rule has score but without value 2");

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL|BODY|ARGS", "s:FOO:6", true);
	mu_assert_null(rule, "naxsi rule has score but invalid name");

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL|BODY|ARGS", "s:$:6", true);
	mu_assert_null(rule, "naxsi rule has score but empty name");

	mu_end;
}

bool test_naxsi_rule_successes(void) {
	naxsi_str_t id_s = { 0 }, match_s = { 0 }, matchzone_s = { 0 }, scores_s = { 0 };
	naxsi_rule_t *rule = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL|$URL:foo", "s:$FOO:8", true);
	mu_assert_notnull(rule, "naxsi rule has matchzone shall allow URL and $URL");
	naxsi_rule_free(&mem, rule);

	test_rule_nodesc("id:42000329", "str:foo", "mz:URL|$URL_X:^foo$", "s:$FOO:8", true);
	mu_assert_notnull(rule, "naxsi rule has matchzone shall allow URL and $URL_X");
	naxsi_rule_free(&mem, rule);

	test_rule_nodesc("id:42000329", "str:foo", "mz:$URL_X:^foo$|URL", "s:$FOO:8", true);
	mu_assert_notnull(rule, "naxsi rule has matchzone shall allow URL and $URL_X (inverted)");
	naxsi_rule_free(&mem, rule);

	test_rule_nodesc("id:42000329", "str:foo", "mz:$URL:foo|URL", "s:$FOO:8", true);
	mu_assert_notnull(rule, "naxsi rule has matchzone shall allow URL and $URL (inverted)");
	naxsi_rule_free(&mem, rule);

	mu_end;
}

bool test_naxsi_rule_match_str_with_one_score(void) {
	naxsi_score_t *score = NULL;
	naxsi_str_t *match = NULL;
	naxsi_str_t id_s = { 0 }, descr_s = { 0 }, match_s = { 0 }, matchzone_s = { 0 }, scores_s = { 0 };
	naxsi_rule_t *rule = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	test_rule("id:42000329", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:$FOO:8", true);
	mu_assert_notnull(rule, "naxsi_rule_new(&mem, \"id:42000329\", \"msg:.git access\", \"str:/.git/\", \"mz:URL|BODY|ARGS\", \"s:$FOO:8\", true) shall NOT return NULL");

	/* check rule data */
	mu_assert_eq(rule->identifier, 42000329u, "rule id shall be 42000329");
	mu_assert_notnull(rule->description, "rule description shall NOT be NULL");
	mu_assert_streq(rule->description->data, ".git access", "rule description shall be '.git access'");
	mu_assert_eq(rule->action, NAXSI_RULE_ACTION_SCORE, "rule action shall be NAXSI_RULE_ACTION_SCORE");
	mu_assert_notnull(rule->scores, "rule scores shall NOT be NULL");
	mu_assert_eq(rule->scores->length, 1, "rule scores length shall be 1");
	mu_assert_eq(rule->mtype, NAXSI_MATCH_TYPE_STRING, "rule match type shall be NAXSI_MATCH_TYPE_STRING");
	mu_assert_true(rule->negative, "rule negative shall be true");

	/* verify that match is /.git/ */
	match = rule->match;
	mu_assert_notnull(match, "rule match shall NOT be NULL");
	mu_assert_streq(match->data, "/.git/", "rule match shall be '/.git/'");

	/* verify that score is $FOO:8 */
	score = naxsi_scores_get_at(rule->scores, 0, NULL);
	mu_assert_notnull(score, "score shall NOT be NULL");
	mu_assert_eq(score->value, 8, "score value shall be 8");
	mu_assert_notnull(score->name.data, "score name shall NOT be NULL");
	mu_assert_streq(score->name.data, "$FOO", "score name shall be $FOO");

	/* verify that matchzone is URL|BODY|ARGS */
	mu_assert_false(rule->matchzone.name, "matchzone name false");
	mu_assert_eq(rule->matchzone.args, NAXSI_MZ_TYPE_GLOBAL, "matchzone args shall be NAXSI_MZ_TYPE_GLOBAL");
	mu_assert_eq(rule->matchzone.body, NAXSI_MZ_TYPE_GLOBAL, "matchzone body shall be NAXSI_MZ_TYPE_GLOBAL");
	mu_assert_eq(rule->matchzone.file_ext, NAXSI_MZ_TYPE_UNSET, "matchzone file_ext shall be NAXSI_MZ_TYPE_UNSET");
	mu_assert_eq(rule->matchzone.headers, NAXSI_MZ_TYPE_UNSET, "matchzone headers shall be NAXSI_MZ_TYPE_UNSET");
	mu_assert_eq(rule->matchzone.raw_body, NAXSI_MZ_TYPE_UNSET, "matchzone raw_body shall be NAXSI_MZ_TYPE_UNSET");
	mu_assert_eq(rule->matchzone.url, NAXSI_MZ_TYPE_GLOBAL, "matchzone url shall be NAXSI_MZ_TYPE_GLOBAL");
	mu_assert_null(rule->matchzone.args_val, "matchzone args_val shall be NULL");
	mu_assert_null(rule->matchzone.body_val, "matchzone body_val shall be NULL");
	mu_assert_null(rule->matchzone.headers_val, "matchzone headers_val shall be NULL");
	mu_assert_null(rule->matchzone.url_val, "matchzone url_val shall be NULL");

	naxsi_rule_free(&mem, rule);

	mu_end;
}

bool test_naxsi_rule_match_regex_with_two_scores(void) {
	naxsi_score_t *score = NULL;
	naxsi_str_t *match = NULL;
	naxsi_str_t id_s = { 0 }, match_s = { 0 }, matchzone_s = { 0 }, scores_s = { 0 };
	naxsi_rule_t *rule = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	test_rule_nodesc("id:42000329", "rx:^query_args\\[.*\\]", "mz:$HEADERS_VAR:foo|$URL_X:/target.+$|NAME", "s:$FOO:8,$BAR:455", false);
	mu_assert_notnull(rule, "naxsi_rule_new(&mem, \"id:42000329\", NULL, \"str:/.git/\", \"mz:URL|BODY|ARGS\", \"s:$FOO:8\", false) shall NOT return NULL");

	/* check rule data */
	mu_assert_eq(rule->identifier, 42000329u, "rule id shall be 42000329");
	mu_assert_null(rule->description, "rule description shall be NULL");
	mu_assert_eq(rule->action, NAXSI_RULE_ACTION_SCORE, "rule action shall be NAXSI_RULE_ACTION_SCORE");
	mu_assert_notnull(rule->scores, "rule scores shall NOT be NULL");
	mu_assert_eq(rule->scores->length, 2, "rule scores length shall be 2");
	mu_assert_eq(rule->mtype, NAXSI_MATCH_TYPE_REGEX, "rule match type shall be NAXSI_MATCH_TYPE_REGEX");
	mu_assert_false(rule->negative, "rule negative shall be false");

	/* verify that match is a compiled regex */
	mu_assert_notnull(rule->match, "rule match shall NOT be NULL");

	/* verify that score is $FOO:8,$BAR:4 */
	score = naxsi_scores_get_at(rule->scores, 0, NULL);
	mu_assert_notnull(score, "score shall NOT be NULL");
	mu_assert_eq(score->value, 8, "score value shall be 8");
	mu_assert_notnull(score->name.data, "score name shall NOT be NULL");
	mu_assert_streq(score->name.data, "$FOO", "score name shall be $FOO");

	score = naxsi_scores_get_at(rule->scores, 1, NULL);
	mu_assert_notnull(score, "score shall NOT be NULL");
	mu_assert_eq(score->value, 455, "score value shall be 455");
	mu_assert_notnull(score->name.data, "score name shall NOT be NULL");
	mu_assert_streq(score->name.data, "$BAR", "score name shall be $BAR");

	/* verify that matchzone is $HEADERS_VAR:foo|$URL_X:/target.+$ */
	mu_assert_true(rule->matchzone.name, "matchzone name true");
	mu_assert_eq(rule->matchzone.args, NAXSI_MZ_TYPE_UNSET, "matchzone args shall be NAXSI_MZ_TYPE_UNSET");
	mu_assert_eq(rule->matchzone.body, NAXSI_MZ_TYPE_UNSET, "matchzone body shall be NAXSI_MZ_TYPE_UNSET");
	mu_assert_eq(rule->matchzone.file_ext, NAXSI_MZ_TYPE_UNSET, "matchzone file_ext shall be NAXSI_MZ_TYPE_UNSET");
	mu_assert_eq(rule->matchzone.headers, NAXSI_MZ_TYPE_STRING, "matchzone headers shall be NAXSI_MZ_TYPE_STRING");
	mu_assert_eq(rule->matchzone.raw_body, NAXSI_MZ_TYPE_UNSET, "matchzone raw_body shall be NAXSI_MZ_TYPE_UNSET");
	mu_assert_eq(rule->matchzone.url, NAXSI_MZ_TYPE_REGEX, "matchzone url shall be NAXSI_MZ_TYPE_REGEX");
	mu_assert_null(rule->matchzone.args_val, "matchzone args_val shall be NULL");
	mu_assert_null(rule->matchzone.body_val, "matchzone body_val shall be NULL");
	mu_assert_notnull(rule->matchzone.headers_val, "matchzone headers_val shall NOT be NULL");
	match = rule->matchzone.headers_val;
	mu_assert_streq(match->data, "foo", "$HEADERS_VAR shall be 'foo'");
	mu_assert_notnull(rule->matchzone.url_val, "matchzone url_val shall NOT be NULL");

	naxsi_rule_free(&mem, rule);

	mu_end;
}

bool test_naxsi_rule_match_str_all_scores(void) {
	naxsi_str_t id_s = { 0 }, descr_s = { 0 }, match_s = { 0 }, matchzone_s = { 0 }, scores_s = { 0 };
	naxsi_rule_t *rule = NULL;
	naxsi_score_t *score = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	test_rule("id:42000329", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:DROP", true);
	mu_assert_notnull(rule, "naxsi_rule_new(&mem, \"id:42000329\", \"msg:.git access\", \"str:/.git/\", \"mz:URL|BODY|ARGS\", \"s:DROP\", true) shall NOT return NULL");
	mu_assert_eq(rule->action, NAXSI_RULE_ACTION_DROP, "rule action shall be NAXSI_RULE_ACTION_SCORE");
	mu_assert_null(rule->scores, "rule scores shall NOT be NULL");
	naxsi_rule_free(&mem, rule);

	test_rule("id:42000329", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:LOG", true);
	mu_assert_notnull(rule, "naxsi_rule_new(&mem, \"id:42000329\", \"msg:.git access\", \"str:/.git/\", \"mz:URL|BODY|ARGS\", \"s:LOG\", true) shall NOT return NULL");
	mu_assert_eq(rule->action, NAXSI_RULE_ACTION_LOG, "rule action shall be NAXSI_RULE_ACTION_SCORE");
	mu_assert_null(rule->scores, "rule scores shall NOT be NULL");
	naxsi_rule_free(&mem, rule);

	test_rule("id:42000329", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:BLOCK", true);
	mu_assert_notnull(rule, "naxsi_rule_new(&mem, \"id:42000329\", \"msg:.git access\", \"str:/.git/\", \"mz:URL|BODY|ARGS\", \"s:BLOCK\", true) shall NOT return NULL");
	mu_assert_eq(rule->action, NAXSI_RULE_ACTION_BLOCK, "rule action shall be NAXSI_RULE_ACTION_SCORE");
	mu_assert_null(rule->scores, "rule scores shall NOT be NULL");
	naxsi_rule_free(&mem, rule);

	test_rule("id:42000329", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:ALLOW", true);
	mu_assert_notnull(rule, "naxsi_rule_new(&mem, \"id:42000329\", \"msg:.git access\", \"str:/.git/\", \"mz:URL|BODY|ARGS\", \"s:ALLOW\", true) shall NOT return NULL");
	mu_assert_eq(rule->action, NAXSI_RULE_ACTION_ALLOW, "rule action shall be NAXSI_RULE_ACTION_SCORE");
	mu_assert_null(rule->scores, "rule scores shall NOT be NULL");
	naxsi_rule_free(&mem, rule);

	test_rule("id:42000329", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:ALLOW,LOG", true);
	mu_assert_notnull(rule, "naxsi_rule_new(&mem, \"id:42000329\", \"msg:.git access\", \"str:/.git/\", \"mz:URL|BODY|ARGS\", \"s:ALLOW,LOG\", true) shall NOT return NULL");
	mu_assert_eq(rule->action, NAXSI_RULE_ACTION_ALLOW | NAXSI_RULE_ACTION_LOG, "rule action shall be NAXSI_RULE_ACTION_SCORE");
	mu_assert_null(rule->scores, "rule scores shall NOT be NULL");
	naxsi_rule_free(&mem, rule);

	test_rule("id:42000329", "msg:.git access", "str:/.git/", "mz:URL|BODY|ARGS", "s:$FOO:8,LOG", true);
	mu_assert_notnull(rule, "naxsi_rule_new(&mem, \"id:42000329\", \"msg:.git access\", \"str:/.git/\", \"mz:URL|BODY|ARGS\", \"s:$FOO:8,LOG\", true) shall NOT return NULL");
	mu_assert_eq(rule->action, NAXSI_RULE_ACTION_SCORE | NAXSI_RULE_ACTION_LOG, "rule action shall be NAXSI_RULE_ACTION_SCORE");
	mu_assert_notnull(rule->scores, "rule scores shall NOT be NULL");

	/* verify that score is $FOO:8 */
	score = naxsi_scores_get_at(rule->scores, 0, NULL);
	mu_assert_notnull(score, "score shall NOT be NULL");
	mu_assert_eq(score->value, 8, "score value shall be 8");
	mu_assert_notnull(score->name.data, "score name shall NOT be NULL");
	mu_assert_streq(score->name.data, "$FOO", "score name shall be $FOO");

	naxsi_rule_free(&mem, rule);

	mu_end;
}

bool test_naxsi_whitelist_failures(void) {
	naxsi_str_t whitelist_s = { 0 }, matchzone_s = { 0 };
	naxsi_whitelist_t *whitelist = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	whitelist = naxsi_whitelist_new(NULL, &whitelist_s, &matchzone_s, true);
	mu_assert_null(whitelist, "naxsi_whitelist_new(NULL, &whitelist_s, &matchzone_s, true) shall return NULL");

	whitelist = naxsi_whitelist_new(&mem, NULL, &matchzone_s, true);
	mu_assert_null(whitelist, "naxsi_whitelist_new(&mem, NULL, &matchzone_s, true) shall return NULL");

	whitelist = naxsi_whitelist_new(&mem, &whitelist_s, NULL, true);
	mu_assert_null(whitelist, "naxsi_whitelist_new(&mem, &whitelist_s, NULL, true) shall return NULL");

	mu_end;
}

bool test_naxsi_whitelist_successes(void) {
	naxsi_str_t wl_s = { 0 }, matchzone_s = { 0 };
	naxsi_whitelist_t *whitelist = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	test_whitelist("wl:1000", "mz:$URL:/modules/update/update.css|URL", false);
	mu_assert_notnull(whitelist, "naxsi_whitelist_new(\"wl:1000\", \"mz:$URL:/modules/update/update.css|URL\", false) shall NOT return NULL");
	mu_assert_notnull(whitelist->ids, "whitelist ids shall NOT be NULL");
	mu_assert_eq(whitelist->ids->length, 1, "whitelist ids length shall be 1");
	mu_assert_eq(naxsi_ids_get_at(whitelist->ids, 0, 8888), 1000, "whitelist ids[0] shall be 1000");
	naxsi_whitelist_free(&mem, whitelist);

	test_whitelist("wl:1000,1002", "mz:$URL_X:^foo$|URL", true);
	mu_assert_notnull(whitelist, "naxsi_whitelist_new(\"wl:1000,1002\", \"mz:$URL_X:^foo$|URL\", true) shall NOT return NULL");
	mu_assert_notnull(whitelist->ids, "whitelist ids shall NOT be NULL");
	mu_assert_eq(whitelist->ids->length, 2, "whitelist ids length shall be 2");
	mu_assert_eq(naxsi_ids_get_at(whitelist->ids, 0, 8888), 1000, "whitelist ids[0] shall be 1000");
	mu_assert_eq(naxsi_ids_get_at(whitelist->ids, 1, 8888), 1002, "whitelist ids[1] shall be 1002");
	naxsi_whitelist_free(&mem, whitelist);

	mu_end;
}

int all_tests() {
	mu_run_test(test_naxsi_rule_failures);
	mu_run_test(test_naxsi_rule_successes);
	mu_run_test(test_naxsi_rule_match_str_with_one_score);
	mu_run_test(test_naxsi_rule_match_regex_with_two_scores);
	mu_run_test(test_naxsi_rule_match_str_all_scores);
	mu_run_test(test_naxsi_whitelist_failures);
	mu_run_test(test_naxsi_whitelist_successes);
	return tests_passed != tests_run;
}

mu_main(all_tests)
