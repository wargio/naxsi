// SPDX-FileCopyrightText: 2024, Giovanni Dante Grazioli <wargio@libero.it>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <ngx_config.h>

#include <naxsi_log.h>
#include <naxsi_macros.h>

void
naxsi_log_begin(naxsi_log_t* log, ngx_http_request_t* r, int as_json, int as_extensive)
{
  log->buffer[0]    = 0;
  log->length       = 0;
  log->r            = r;
  log->rid          = (const char*)naxsi_request_id(r);
  log->as_json      = as_json;
  log->as_extensive = as_extensive;
}

static void
naxsi_log_as_text_flush(naxsi_log_t* log, ngx_int_t extend)
{
  if (log->length > 0) {
    ngx_http_naxsi_loc_conf_t* cf = ngx_http_get_module_loc_conf(log->r, ngx_http_naxsi_module);
    ngx_log_t*                 ngx_log = cf->log ? cf->log : log->r->connection->log;
    ngx_log_error(NGX_LOG_ERR,
                  ngx_log,
                  0,
                  "%s: %s",
                  log->as_extensive ? "NAXSI_EXLOG" : "NAXSI_FMT",
                  log->buffer);
  }
  if (extend) {
    log->length = snprintf(log->buffer, NAXSI_LOG_MAX, "rid=%s", log->rid);
  }
}

static void
naxsi_log_as_text_string_ex(naxsi_log_t* log,
                            const char*  key,
                            size_t       key_len,
                            u_char*      val,
                            size_t       val_len)
{
  if (val_len >= (NGX_MAX_UINT32_VALUE / 4) - 1) {
    val_len /= 4;
  }

  int truncate = 0;
  while (val_len >= (NAXSI_LOG_MAX >> 1)) {
    truncate = 1;
    val_len /= 2;
  }

  size_t len = val_len + (2 * ngx_escape_uri(NULL, val, val_len, NGX_ESCAPE_URI_COMPONENT));
  if (len >= NAXSI_LOG_MAX)
    return;

  // we always add 2 chars "&<key>="
  if ((log->length + val_len + key_len + 2) >= NAXSI_LOG_MAX) {
    naxsi_log_as_text_flush(log, 1);
  }

  log->length += snprintf(
    log->buffer + log->length, NAXSI_LOG_MAX - log->length, log->length < 1 ? "%s=" : "&%s=", key);

  ngx_escape_uri((u_char*)(log->buffer + log->length), val, val_len, NGX_ESCAPE_URI_COMPONENT);

  log->length += len;
  log->buffer[log->length] = 0;

  if (truncate) {
    log->length += snprintf(log->buffer + log->length, NAXSI_LOG_MAX - log->length, "...");
  }
}

static void
naxsi_log_as_text_cstring_ex(naxsi_log_t* log,
                             const char*  key,
                             size_t       key_len,
                             const char*  val,
                             size_t       val_len)
{
  // we always add 2 chars "&<key>=<value>"
  if ((log->length + key_len + val_len + 2) >= NAXSI_LOG_MAX) {
    naxsi_log_as_text_flush(log, 1);
  }
  log->length +=
    snprintf(log->buffer + log->length, NAXSI_LOG_MAX - log->length, "&%s=%s", key, val);
}

static void
naxsi_log_as_text_number(naxsi_log_t* log, const char* key, ngx_int_t number)
{
  int num_len = snprintf(NULL, 0, "&%s=%zu", key, number);
  if ((log->length + num_len) >= NAXSI_LOG_MAX) {
    naxsi_log_as_text_flush(log, 1);
  }
  log->length +=
    snprintf(log->buffer + log->length, NAXSI_LOG_MAX - log->length, "&%s=%zu", key, number);
}

/* JSON functions */

static const char* json_hex = "0123456789abcdef";

static void
naxsi_escape_json_string(naxsi_log_t* log, const u_char* val, size_t length, int truncate)
{
  char*       out     = log->buffer + log->length;
  const char* end     = log->buffer + NAXSI_LOG_MAX;
  size_t      max_len = end - out;
  if (length > max_len) {
    length = max_len;
  }

  *out++ = '"';
  size_t i;
  for (i = 0; i < length && out < end; i++) {
    if (val[i] == '\n') {
      // new line
      break_if(out + 2 >= end);
      *out++ = '\\';
      *out++ = 'n';
    } else if (val[i] == '\r') {
      // return carriage
      break_if(out + 2 >= end);
      *out++ = '\\';
      *out++ = 'r';
    } else if (val[i] == '\\') {
      // slash
      break_if(out + 2 >= end);
      *out++ = '\\';
      *out++ = '\\';
    } else if (val[i] == '\t') {
      // tabulation
      break_if(out + 2 >= end);
      *out++ = '\\';
      *out++ = 't';
    } else if (val[i] == '"') {
      // double quote
      break_if(out + 2 >= end);
      *out++ = '\\';
      *out++ = '"';
    } else if (val[i] == '\f') {
      // form feed
      break_if(out + 2 >= end);
      *out++ = '\\';
      *out++ = 'f';
    } else if (val[i] == '\b') {
      // backspace
      break_if(out + 2 >= end);
      *out++ = '\\';
      *out++ = 'b';
    } else if (!is_printable(val[i])) {
      break_if(out + 4 >= end);
      *out++ = '\\';
      *out++ = 'u';
      *out++ = '0';
      *out++ = '0';
      *out++ = json_hex[(val[i] >> 4) & 0x0f];
      *out++ = json_hex[val[i] & 0x0f];
    } else {
      *out++ = val[i];
    }
  }
  if (truncate && out + 4 < end) {
    *out++ = '.';
    *out++ = '.';
    *out++ = '.';
  }
  if (out < end) {
    *out++ = '"';
  }
  *out++      = 0;
  log->length = out - log->buffer - 1;
}

static void
naxsi_log_as_json_flush(naxsi_log_t* log, ngx_int_t extend)
{
  if (log->length > 0) {
    ngx_http_naxsi_loc_conf_t* cf = ngx_http_get_module_loc_conf(log->r, ngx_http_naxsi_module);
    ngx_log_t*                 ngx_log = cf->log ? cf->log : log->r->connection->log;
    ngx_log_error(NGX_LOG_ERR, ngx_log, 0, "{%s}", log->buffer);
  }
  if (extend) {
    log->length = snprintf(log->buffer, NAXSI_LOG_MAX, "\"rid\":\"%s\"", log->rid);
  }
}

static void
naxsi_log_as_json_string_ex(naxsi_log_t* log,
                            const char*  key,
                            size_t       key_len,
                            u_char*      val,
                            size_t       val_len)
{
  if (val_len >= NAXSI_LOG_MAX) {
    val_len = NAXSI_LOG_MAX - (log->length + 6);
  }

  int truncate = 0;
  while (val_len >= (NAXSI_LOG_MAX >> 1)) {
    truncate = 1;
    val_len /= 2;
  }

  // we always add 5 chars ,"<key>":"<value>"
  if ((log->length + key_len + val_len + 6) >= NAXSI_LOG_MAX) {
    naxsi_log_as_json_flush(log, 1);
  }
  if (log->length < 1) {
    log->length += snprintf(log->buffer + log->length, NAXSI_LOG_MAX - log->length, "\"%s\":", key);
  } else {
    log->length +=
      snprintf(log->buffer + log->length, NAXSI_LOG_MAX - log->length, ",\"%s\":", key);
  }
  naxsi_escape_json_string(log, val, val_len, truncate);
}

static void
naxsi_log_as_json_cstring_ex(naxsi_log_t* log,
                             const char*  key,
                             size_t       key_len,
                             const char*  val,
                             size_t       val_len)
{
  // we always add 5 chars ,"<key>":"<value>"
  if ((log->length + key_len + val_len + 6) >= NAXSI_LOG_MAX) {
    naxsi_log_as_json_flush(log, 1);
  }
  log->length +=
    snprintf(log->buffer + log->length, NAXSI_LOG_MAX - log->length, ",\"%s\":\"%s\"", key, val);
}

static void
naxsi_log_as_json_number(naxsi_log_t* log, const char* key, ngx_int_t number)
{
  int num_len = snprintf(NULL, 0, "\"%s\":%zu", key, number);
  if ((log->length + num_len) >= NAXSI_LOG_MAX) {
    naxsi_log_as_text_flush(log, 1);
  }
  log->length +=
    snprintf(log->buffer + log->length, NAXSI_LOG_MAX - log->length, ",\"%s\":%zu", key, number);
}

void
naxsi_log_end(naxsi_log_t* log)
{
  if (log->as_json) {
    naxsi_log_as_json_flush(log, 0);
  } else {
    naxsi_log_as_text_flush(log, 0);
  }
}

void
naxsi_log_string_ex(naxsi_log_t* log, const char* key, size_t key_len, u_char* val, size_t val_len)
{
  if (log->as_json) {
    naxsi_log_as_json_string_ex(log, key, key_len, val, val_len);
  } else {
    naxsi_log_as_text_string_ex(log, key, key_len, val, val_len);
  }
}

void
naxsi_log_cstring_ex(naxsi_log_t* log,
                     const char*  key,
                     size_t       key_len,
                     const char*  val,
                     size_t       val_len)
{
  if (log->as_json) {
    naxsi_log_as_json_cstring_ex(log, key, key_len, val, val_len);
  } else {
    naxsi_log_as_text_cstring_ex(log, key, key_len, val, val_len);
  }
}

void
naxsi_log_number(naxsi_log_t* log, const char* key, ngx_int_t number)
{
  if (log->as_json) {
    naxsi_log_as_json_number(log, key, number);
  } else {
    naxsi_log_as_text_number(log, key, number);
  }
}
