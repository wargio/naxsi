// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_UTILS_H
#define NAXSI_UTILS_H
#include <naxsi_types.h>

bool naxsi_is_invalid_utf8(const naxsi_str_t *string);
bool naxsi_escape_nullbytes(naxsi_str_t *str);
const char *naxsi_strstr_offset(naxsi_str_t *string, size_t offset, naxsi_str_t *substring);
const char *naxsi_strnchr(const char *string, int ch, size_t string_len);

#endif /* NAXSI_UTILS_H */
