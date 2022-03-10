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

#ifdef __cplusplus
}
#endif

#endif /* NAXSI_CONST_H */
