// SPDX-FileCopyrightText: 2024 Giovanni Dante Grazioli <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef NAXSI_LOG_H
#define NAXSI_LOG_H

#include <naxsi.h>

int
naxsi_json_string_encode(const char* str, size_t str_len, char* buffer, const size_t buffer_size);

int
naxsi_json_log(const ngx_str_t* elem, char* buffer, const size_t size);

#endif /* NAXSI_LOG_H */
