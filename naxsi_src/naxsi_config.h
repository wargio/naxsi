// SPDX-FileCopyrightText: 2016-2019, Thibault 'bui' Koechlin <tko@nbs-system.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NAXSI_CONFIG_H
#define NAXSI_CONFIG_H

/* custom match  zones */
#define MZ_GET_VAR_T      "$ARGS_VAR:"
#define MZ_HEADER_VAR_T   "$HEADERS_VAR:"
#define MZ_POST_VAR_T     "$BODY_VAR:"
#define MZ_SPECIFIC_URL_T "$URL:"

/* add support for regex-style match zones.
** this whole function should be rewritten as it's getting
** messy as hell
*/
#define MZ_GET_VAR_X      "$ARGS_VAR_X:"
#define MZ_HEADER_VAR_X   "$HEADERS_VAR_X:"
#define MZ_POST_VAR_X     "$BODY_VAR_X:"
#define MZ_SPECIFIC_URL_X "$URL_X:"

#endif /* NAXSI_CONFIG_H */
