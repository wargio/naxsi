// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_CONST_H
#define NAXSI_CONST_H

#include <naxsi_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NAXSI_VERSION        "1.4.0"
#define NAXSI_VERSION_MAJOR  (1)
#define NAXSI_VERSION_MINOR  (4)
#define NAXSI_VERSION_NUMBER (0)
#define NAXSI_VERSION_PATCH  ((1000000 * NAXSI_VERSION_MAJOR) + (1000 * NAXSI_VERSION_MINOR) + NAXSI_VERSION_PATCH)

#define NAXSI_KEYWORD_DENIED_URL       "DeniedUrl"
#define NAXSI_KEYWORD_IGNORE_IP        "IgnoreIP"
#define NAXSI_KEYWORD_IGNORE_CIDR      "IgnoreCIDR"
#define NAXSI_KEYWORD_LEARNING_FLAG    "LearningMode"
#define NAXSI_KEYWORD_ENABLED_FLAG     "SecRulesEnabled"
#define NAXSI_KEYWORD_DISABLED_FLAG    "SecRulesDisabled"
#define NAXSI_KEYWORD_CHECK_RULE       "CheckRule"
#define NAXSI_KEYWORD_BASIC_RULE       "BasicRule"
#define NAXSI_KEYWORD_MAIN_RULE        "MainRule"
#define NAXSI_KEYWORD_LIBINJECTION_SQL "LibInjectionSql"
#define NAXSI_KEYWORD_LIBINJECTION_XSS "LibInjectionXss"

#define NAXSI_MAIN_RULE_ID_WEIRD_REQUEST         (1)
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

#ifdef __cplusplus
}
#endif

#endif /* NAXSI_CONST_H */
