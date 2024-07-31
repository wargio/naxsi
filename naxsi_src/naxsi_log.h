// SPDX-FileCopyrightText: 2024, Giovanni Dante Grazioli <wargio@libero.it>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NAXSI_LOG_H
#define NAXSI_LOG_H

#include <naxsi.h>
#include <naxsi_const.h>

typedef struct naxsi_log_s
{
  char                buffer[NGX_MAX_ERROR_STR]; ///< log buffer
  size_t              length;                    ///< log length
  const char*         rid;                       ///< request id
  ngx_http_request_t* r;                         ///< request to log
  int                 as_json;                   ///< log is json
  int                 as_extensive;              ///< log is extensive
} naxsi_log_t;

void
naxsi_log_begin(naxsi_log_t* log, ngx_http_request_t* r, int as_json, int as_extensive);

void
naxsi_log_end(naxsi_log_t* log);

void
naxsi_log_string_ex(naxsi_log_t* log, const char* key, size_t key_len, u_char* val, size_t val_len);
void
naxsi_log_cstring_ex(naxsi_log_t* log,
                     const char*  key,
                     size_t       key_len,
                     const char*  val,
                     size_t       val_len);

void
naxsi_log_number(naxsi_log_t* log, const char* key, ngx_int_t number);

#define naxsi_log_string(log, key, val, val_len)                                                   \
  naxsi_log_string_ex(log, key, strlen(key), val, val_len)
#define naxsi_log_ngx_string(log, key, val)                                                        \
  naxsi_log_string_ex(log, key, strlen(key), val->data, val->len)
#define naxsi_log_cstring(log, key, val)                                                           \
  naxsi_log_cstring_ex(log, key, strlen(key), val, strlen(val))

#endif /* NAXSI_LOG_H */
