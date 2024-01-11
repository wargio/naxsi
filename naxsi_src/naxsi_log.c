// SPDX-FileCopyrightText: 2024 Giovanni Dante Grazioli <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include <naxsi.h>
#include <naxsi_macros.h>

#define NAXSI_LOG_MAX_SIZE (NGX_MAX_ERROR_STR)

#define LOG_INIT(log) (log.used = 0)
#define LOG_FINI(log)                                                                              \
  (log.buffer[log.used >= NAXSI_LOG_MAX_SIZE ? NAXSI_LOG_MAX_SIZE - 1 : log.used] = 0)
#define LOG_STRING(log)      ((const char*)log.buffer)
#define LOG_LEFTOVERS(log)   (NAXSI_LOG_MAX_SIZE - log->used)
#define LOG_HAS(log, needed) ((log->used + needed) < NAXSI_LOG_MAX_SIZE)

#define LOG_CONST_STRING(l, s) log_string(l, (const u_char*)(s), strlen(s))

#define URL_AMP(l)                  log_char(l, '&')
#define URL_KEY_STRING_CST(l, k, s) url_key_string(l, k, (const u_char*)(s), strlen(s))

#define JSON_OBJECT_BEGIN(l)         log_char(l, '{')
#define JSON_OBJECT_END(l)           log_char(l, '}')
#define JSON_ARRAY_BEGIN(l)          log_char(l, '[')
#define JSON_ARRAY_END(l)            log_char(l, ']')
#define JSON_COMMA(l)                log_char(l, ',')
#define JSON_KEY_NUMBER(l, k, n)     (json_key(l, k) && log_number(l, n))
#define JSON_KEY_STRING_CST(l, k, s) json_key_string(l, k, (const u_char*)(s), strlen(s))
#define JSON_KEY_STRING_NGX(l, k, s) json_key_string(l, k, s->data, s->len)
#define JSON_KEY_ARRAY(l, k)         (json_key(l, k) && JSON_ARRAY_BEGIN(l))
#define JSON_KEY_OBJECT(l, k)        (json_key(l, k) && JSON_OBJECT_BEGIN(l))

typedef struct
{
  size_t used;
  char   buffer[NAXSI_LOG_MAX_SIZE];
} log_t;

static const char* hex_dict = "0123456789abcdef";

static const char* naxsi_match_zones[] = { to_str(HEADERS), to_str(URL),      to_str(ARGS),
                                           to_str(BODY),    to_str(RAW_BODY), to_str(FILE_EXT),
                                           to_str(ANY),     to_str(UNKNOWN),  NULL };

static int
log_char(log_t* log, const char ch)
{
  if (!LOG_HAS(log, 1)) {
    return 0;
  }
  char* buffer = &log->buffer[log->used];
  *buffer      = ch;
  log->used++;
  return 1;
}

static int
log_string(log_t* log, const u_char* string, size_t size)
{
  if (!LOG_HAS(log, size)) {
    return 0;
  }
  if (size < 1) {
    return 1;
  }
  char* buf = &log->buffer[log->used];
  memcpy(buf, string, size);
  log->used += size;
  return 1;
}

static int
log_number(log_t* log, ngx_uint_t value)
{
  int needed = snprintf(0, 0, "%zu", value);
  if (needed < 1 || !LOG_HAS(log, needed)) {
    return 0;
  }
  char*  buf  = &log->buffer[log->used];
  size_t size = LOG_LEFTOVERS(log);
  snprintf(buf, size, "%zu", value);
  log->used += needed;
  return 1;
}

static int
json_escape_unicode(log_t* log, const u_char byte)
{
  if (!LOG_HAS(log, 6)) {
    return 0;
  }
  char* buffer = &log->buffer[log->used];
  *buffer++    = '\\';
  *buffer++    = 'u';
  *buffer++    = '0';
  *buffer++    = '0';
  *buffer++    = hex_dict[(byte >> 4) & 0xf];
  *buffer++    = hex_dict[byte & 0xf];
  log->used += 6;
  return 1;
}

static int
json_escape_char(log_t* log, const char ch)
{
  if (!LOG_HAS(log, 2)) {
    return 0;
  }
  char* buffer = &log->buffer[log->used];
  buffer[0]    = '\\';
  buffer[1]    = ch;
  log->used += 2;
  return 1;
}

static int
json_string(log_t* log, const u_char* val, size_t length)
{
  if (!LOG_HAS(log, 3)) {
    return 0;
  }

  const size_t leftovers = LOG_LEFTOVERS(log) - 3;
  if (length > leftovers) {
    length = leftovers;
  }

  log_char(log, '"');
  size_t i;
  for (i = 0; i < length; i++) {
    const u_char ch = val[i];
    if (ch == '\n') {
      // new line
      return_value_if(!json_escape_char(log, 'n'), 0);
    } else if (ch == '\r') {
      // return carriage
      return_value_if(!json_escape_char(log, 'r'), 0);
    } else if (ch == '\\') {
      // slash
      return_value_if(!json_escape_char(log, '\\'), 0);
    } else if (ch == '\t') {
      // tabulation
      return_value_if(!json_escape_char(log, 't'), 0);
    } else if (ch == '"') {
      // double quote
      return_value_if(!json_escape_char(log, '"'), 0);
    } else if (ch == '\f') {
      // form feed
      return_value_if(!json_escape_char(log, 'f'), 0);
    } else if (ch == '\b') {
      // backspace
      return_value_if(!json_escape_char(log, 'b'), 0);
    } else if (!is_printable(ch)) {
      // escape char as unicode
      return_value_if(!json_escape_unicode(log, ch), 0);
    } else {
      // printable, we add it.
      return_value_if(!log_char(log, (char)ch), 0);
    }
  }
  return log_char(log, '"');
}

static int
json_key(log_t* log, const char* key)
{
  int needed = snprintf(0, 0, "\"%s\":", key);
  if (needed < 1 || !LOG_HAS(log, needed)) {
    return 0;
  }
  char*  buf  = &log->buffer[log->used];
  size_t size = LOG_LEFTOVERS(log);
  snprintf(buf, size, "\"%s\":", key);
  log->used += needed;
  return 1;
}

static int
json_key_string(log_t* log, const char* key, const u_char* val, size_t length)
{
  if (length < 1) {
    return 1;
  }
  return json_key(log, key) && json_string(log, val, length);
}

static int
json_matchzone(log_t* log, naxsi_match_zone_t zone, ngx_int_t target_name)
{
  const char* czone = naxsi_match_zones[zone];
  const char* cname = target_name ? "|NAME" : "";
  return json_key(log, "zone") && log_char(log, '"') && LOG_CONST_STRING(log, czone) &&
         LOG_CONST_STRING(log, cname) && log_char(log, '"');
}

static int
url_matchzone(log_t* log, naxsi_match_zone_t zone, ngx_int_t target_name)
{
  const char* czone = naxsi_match_zones[zone];
  const char* cname = target_name ? "|NAME" : "";
  return LOG_CONST_STRING(log, "zone") && log_char(log, '=') && LOG_CONST_STRING(log, czone) &&
         LOG_CONST_STRING(log, cname);
}

static int
url_key_number(log_t* log, const char* key, ngx_uint_t value)
{
  return LOG_CONST_STRING(log, key) && log_char(log, '=') && log_number(log, value);
}

static int
url_key_idx_number(log_t* log, const char* key, ngx_uint_t idx, ngx_uint_t value)
{
  return LOG_CONST_STRING(log, key) && log_number(log, idx) && log_char(log, '=') &&
         log_number(log, value);
}

static int
url_key_idx_string(log_t* log, const char* key, ngx_uint_t idx, ngx_str_t* value)
{
  if (value->len < 1) {
    return 1;
  }
  return LOG_CONST_STRING(log, key) && log_number(log, idx) && log_char(log, '=') &&
         log_string(log, value->data, value->len);
}

static int
url_key_string(log_t* log, const char* key, const u_char* value, size_t size)
{
  if (size < 1) {
    return 1;
  }
  return LOG_CONST_STRING(log, key) && log_char(log, '=') && log_string(log, value, size);
}

static int
url_safe_key_string_ngx(log_t* log, const char* key, ngx_str_t* value)
{
  if (value->len < 1) {
    return 1;
  }
  return LOG_CONST_STRING(log, key) && log_char(log, '=') &&
         log_string(log, value->data, value->len);
}

static int
url_unsafe_key_string_ngx(log_t* log, const char* key, ngx_str_t* value)
{
  if (value->len < 1) {
    return 1;
  }
  size_t needed =
    value->len + (2 * ngx_escape_uri(NULL, value->data, value->len, NGX_ESCAPE_URI_COMPONENT));
  if (!(LOG_CONST_STRING(log, key) && log_char(log, '=') && LOG_HAS(log, needed))) {
    return 0;
  }
  u_char* buf = (u_char*)&log->buffer[log->used];
  ngx_escape_uri(buf, value->data, value->len, NGX_ESCAPE_URI_COMPONENT);
  log->used += needed;
  return 1;
}

static void
naxsi_log_offending_extra_as_json(ngx_http_request_ctx_t* ctx,
                                  ngx_http_request_t*     req,
                                  ngx_str_t*              name,
                                  ngx_str_t*              val,
                                  ngx_http_rule_t*        rule,
                                  naxsi_match_zone_t      zone,
                                  ngx_int_t               target_name)
{
  log_t                      log;
  ngx_str_t*                 str = NULL;
  ngx_http_naxsi_loc_conf_t* cf  = NULL;
  u_char                     req_id[NAXSI_REQUEST_ID_STRLEN];

  // get request id.
  ngx_hex_dump(req_id, ctx->request_id, NAXSI_REQUEST_ID_SIZE);

  // init log data.
  LOG_INIT(log);

  JSON_OBJECT_BEGIN(&log);

  // ip address
  str = &req->connection->addr_text;
  if (!(JSON_KEY_STRING_NGX(&log, "ip", str) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // server
  str = &req->headers_in.server;
  if (!(JSON_KEY_STRING_NGX(&log, "server", str) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // request id
  if (!(json_key_string(&log, "rid", req_id, NAXSI_REQUEST_ID_STRLEN - 1) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // uri
  str = &req->uri;
  if (!(JSON_KEY_STRING_NGX(&log, "uri", str) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // rule id
  if (!(JSON_KEY_NUMBER(&log, "id", rule->rule_id) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // zone
  if (!(json_matchzone(&log, zone, target_name))) {
    goto log_fail;
  }

  // var_name
  if (name->len > 0 && !(JSON_COMMA(&log) && JSON_KEY_STRING_NGX(&log, "var_name", name))) {
    goto log_fail;
  }

  // content
  if (val->len > 0 && !(JSON_COMMA(&log) && JSON_KEY_STRING_NGX(&log, "content", val))) {
    goto log_fail;
  }

  JSON_OBJECT_END(&log);

log_fail:
  // always ensure last byte is zero
  LOG_FINI(log);

  cf = ngx_http_get_module_loc_conf(req, ngx_http_naxsi_module);
  ngx_log_error(NGX_LOG_ERR, cf->log ? cf->log : req->connection->log, 0, "%s", LOG_STRING(log));
}

static void
naxsi_log_offending_extra_as_urlencode(ngx_http_request_ctx_t* ctx,
                                       ngx_http_request_t*     req,
                                       ngx_str_t*              name,
                                       ngx_str_t*              val,
                                       ngx_http_rule_t*        rule,
                                       naxsi_match_zone_t      zone,
                                       ngx_int_t               target_name)
{

  log_t                      log;
  ngx_str_t*                 str = NULL;
  ngx_http_naxsi_loc_conf_t* cf  = NULL;
  u_char                     req_id[NAXSI_REQUEST_ID_STRLEN];

  // get request id.
  ngx_hex_dump(req_id, ctx->request_id, NAXSI_REQUEST_ID_SIZE);

  // init log data.
  LOG_INIT(log);

  // ip address
  str = &req->connection->addr_text;
  if (!(url_safe_key_string_ngx(&log, "ip", str) && URL_AMP(&log))) {
    goto log_fail;
  }

  // server
  str = &req->headers_in.server;
  if (!(url_safe_key_string_ngx(&log, "server", str) && URL_AMP(&log))) {
    goto log_fail;
  }

  // request id
  if (!(url_key_string(&log, "rid", req_id, NAXSI_REQUEST_ID_STRLEN - 1) && URL_AMP(&log))) {
    goto log_fail;
  }

  // uri
  str = &req->uri;
  if (!(url_unsafe_key_string_ngx(&log, "uri", str) && URL_AMP(&log))) {
    goto log_fail;
  }

  // rule id
  if (!(url_key_number(&log, "id", rule->rule_id) && URL_AMP(&log))) {
    goto log_fail;
  }

  // zone
  if (!url_matchzone(&log, zone, target_name)) {
    goto log_fail;
  }

  // var_name
  if (name->len > 0 && !(URL_AMP(&log) && url_unsafe_key_string_ngx(&log, "var_name", name))) {
    goto log_fail;
  }

  // content
  if (val->len > 0 && !(URL_AMP(&log) && url_unsafe_key_string_ngx(&log, "content", val))) {
    goto log_fail;
  }

log_fail:
  // always ensure last byte is zero
  LOG_FINI(log);

  cf = ngx_http_get_module_loc_conf(req, ngx_http_naxsi_module);
  ngx_log_error(
    NGX_LOG_ERR, cf->log ? cf->log : req->connection->log, 0, "NAXSI_EXLOG: %s", LOG_STRING(log));
}

static const char*
naxsi_log_get_mode(ngx_http_request_ctx_t* ctx)
{

  if (ctx->learning) {
    return ctx->drop ? "learning-drop" : "learning";
  } else if (ctx->drop) {
    return "drop";
  } else if (ctx->block) {
    return "block";
  } else if (ctx->ignore) {
    return "ignore";
  }
  return "allow";
}

void
naxsi_log_offending_as_json(ngx_http_request_ctx_t* ctx, ngx_http_request_t* req)
{
  log_t                      log;
  ngx_str_t*                 str  = NULL;
  ngx_http_naxsi_loc_conf_t* cf   = NULL;
  const char*                mode = NULL;
  u_char                     req_id[NAXSI_REQUEST_ID_STRLEN];

  // get mode name
  mode = naxsi_log_get_mode(ctx);

  // get request id.
  ngx_hex_dump(req_id, ctx->request_id, NAXSI_REQUEST_ID_SIZE);

  // init log data.
  LOG_INIT(log);

  JSON_OBJECT_BEGIN(&log);

  // ip address
  str = &req->connection->addr_text;
  if (!(JSON_KEY_STRING_NGX(&log, "ip", str) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // server
  str = &req->headers_in.server;
  if (!(JSON_KEY_STRING_NGX(&log, "server", str) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // request id
  if (!(json_key_string(&log, "rid", req_id, NAXSI_REQUEST_ID_STRLEN - 1) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // request uri
  str = &req->uri;
  if (!(JSON_KEY_STRING_NGX(&log, "uri", str) && JSON_COMMA(&log))) {
    goto log_fail;
  }

  // naxsi mode
  if (!(JSON_KEY_STRING_CST(&log, "mode", mode))) {
    goto log_fail;
  }

  // scores
  if (ctx->special_scores) {
    if (!(JSON_COMMA(&log) && JSON_KEY_OBJECT(&log, "score"))) {
      goto log_fail;
    }

    ngx_http_special_score_t* sc = ctx->special_scores->elts;
    size_t                    i, k;
    for (i = 0, k = 0; i < ctx->special_scores->nelts; i++) {
      if (!sc[i].sc_score) {
        continue;
      }
      if (k > 0 && !JSON_COMMA(&log)) {
        goto log_fail;
      }
      if (!JSON_KEY_NUMBER(&log, (const char*)sc[i].sc_tag->data, sc[i].sc_score)) {
        goto log_fail;
      }
      k++;
    }
    if (!JSON_OBJECT_END(&log)) {
      goto log_fail;
    }
  }

  // rules matched
  if (ctx->matched) {
    if (!(JSON_COMMA(&log) && JSON_KEY_ARRAY(&log, "match"))) {
      goto log_fail;
    }

    ngx_http_matched_rule_t* mr = ctx->matched->elts;
    size_t                   i, k;
    for (i = 0, k = 0; i < ctx->matched->nelts; i++) {
      if (k > 0 && !JSON_COMMA(&log)) {
        goto log_fail;
      }
      if (!JSON_OBJECT_BEGIN(&log)) {
        goto log_fail;
      }

      // matchzone
      if (mr[i].body_var &&
          !JSON_KEY_STRING_CST(&log, "zone", mr[i].target_name ? "BODY|NAME" : "BODY")) {
        goto log_fail;
      } else if (mr[i].args_var &&
                 !JSON_KEY_STRING_CST(&log, "zone", mr[i].target_name ? "ARGS|NAME" : "ARGS")) {
        goto log_fail;
      } else if (mr[i].headers_var &&
                 !JSON_KEY_STRING_CST(
                   &log, "zone", mr[i].target_name ? "HEADERS|NAME" : "HEADERS")) {
        goto log_fail;
      } else if (mr[i].url && !JSON_KEY_STRING_CST(&log, "zone", "URL")) {
        goto log_fail;
      } else if (mr[i].file_ext && !JSON_KEY_STRING_CST(&log, "zone", "FILE_EXT")) {
        goto log_fail;
      }

      // rule id
      if (!(JSON_COMMA(&log) && JSON_KEY_NUMBER(&log, "id", mr[i].rule->rule_id))) {
        goto log_fail;
      }

      // add comma only if the var_name value is not empty.
      if (mr[i].name->len > 0 && !JSON_COMMA(&log)) {
        goto log_fail;
      }

      // var_name
      if (!(JSON_KEY_STRING_NGX(&log, "var_name", mr[i].name) && JSON_OBJECT_END(&log))) {
        goto log_fail;
      }
      k++;
    }

    if (!JSON_ARRAY_END(&log)) {
      goto log_fail;
    }
  }

  JSON_OBJECT_END(&log);

log_fail:
  // always ensure last byte is zero
  LOG_FINI(log);

  cf = ngx_http_get_module_loc_conf(req, ngx_http_naxsi_module);
  ngx_log_error(NGX_LOG_ERR, cf->log ? cf->log : req->connection->log, 0, "%s", LOG_STRING(log));
}

void
naxsi_log_offending_as_urlencode(ngx_http_request_ctx_t* ctx, ngx_http_request_t* req)
{
  log_t                      log;
  ngx_str_t*                 str  = NULL;
  ngx_http_naxsi_loc_conf_t* cf   = NULL;
  const char*                mode = NULL;
  u_char                     req_id[NAXSI_REQUEST_ID_STRLEN];

  // get mode name
  mode = naxsi_log_get_mode(ctx);

  // get request id.
  ngx_hex_dump(req_id, ctx->request_id, NAXSI_REQUEST_ID_SIZE);

  // init log data.
  LOG_INIT(log);

  // ip address
  str = &req->connection->addr_text;
  if (!(url_safe_key_string_ngx(&log, "ip", str) && URL_AMP(&log))) {
    goto log_fail;
  }

  // server
  str = &req->headers_in.server;
  if (!(url_safe_key_string_ngx(&log, "server", str) && URL_AMP(&log))) {
    goto log_fail;
  }

  // request id
  if (!(url_key_string(&log, "rid", req_id, NAXSI_REQUEST_ID_STRLEN - 1) && URL_AMP(&log))) {
    goto log_fail;
  }

  // uri
  str = &req->uri;
  if (!(url_unsafe_key_string_ngx(&log, "uri", str) && URL_AMP(&log))) {
    goto log_fail;
  }

  // naxsi mode
  if (!(URL_KEY_STRING_CST(&log, "mode", mode))) {
    goto log_fail;
  }

  // scores
  if (ctx->special_scores) {
    ngx_http_special_score_t* sc = ctx->special_scores->elts;
    ngx_uint_t                i;
    for (i = 0; i < ctx->special_scores->nelts; i++) {
      if (!sc[i].sc_score) {
        continue;
      }
      if (!(URL_AMP(&log) && url_key_idx_string(&log, "cscore", i, sc[i].sc_tag) && URL_AMP(&log) &&
            url_key_idx_number(&log, "score", i, sc[i].sc_score))) {
        goto log_fail;
      }
    }
  }

  // rules matched
  if (ctx->matched) {
    ngx_http_matched_rule_t* mr = ctx->matched->elts;
    ngx_uint_t               i;
    for (i = 0; i < ctx->matched->nelts; i++) {
      if (!(URL_AMP(&log))) {
        goto log_fail;
      }

      // match zone
      if (mr[i].body_var &&
          !(URL_KEY_STRING_CST(&log, "zone", mr[i].target_name ? "BODY|NAME" : "BODY"))) {
        goto log_fail;
      } else if (mr[i].args_var &&
                 !(URL_KEY_STRING_CST(&log, "zone", mr[i].target_name ? "ARGS|NAME" : "ARGS"))) {
        goto log_fail;
      } else if (mr[i].headers_var &&
                 !(URL_KEY_STRING_CST(
                   &log, "zone", mr[i].target_name ? "HEADERS|NAME" : "HEADERS"))) {
        goto log_fail;
      } else if (mr[i].url && !(URL_KEY_STRING_CST(&log, "zone", "URL"))) {
        goto log_fail;
      } else if (mr[i].file_ext && !(URL_KEY_STRING_CST(&log, "zone", "FILE_EXT"))) {
        goto log_fail;
      }

      // rule id
      if (!(URL_AMP(&log) && url_key_number(&log, "id", mr[i].rule->rule_id))) {
        goto log_fail;
      }

      // add the var_name if the value is not empty.
      if (mr[i].name->len < 1) {
        continue;
      }

      // var_name
      if (!(URL_AMP(&log) && url_unsafe_key_string_ngx(&log, "var_name", mr[i].name))) {
        goto log_fail;
      }
    }
  }

log_fail:
  // always ensure last byte is zero
  LOG_FINI(log);

  cf = ngx_http_get_module_loc_conf(req, ngx_http_naxsi_module);
  ngx_log_error(
    NGX_LOG_ERR, cf->log ? cf->log : req->connection->log, 0, "NAXSI_FMT: %s", LOG_STRING(log));
}

void
naxsi_log_offending(ngx_http_request_ctx_t* ctx, ngx_http_request_t* req)
{
  if (ctx->json_log) {
    naxsi_log_offending_as_json(ctx, req);
    return;
  }

  naxsi_log_offending_as_urlencode(ctx, req);
}

/*
  function used for intensive log if dynamic flag is set.
  Output format :
  ip=<ip>&server=<server>&uri=<uri>&id=<id>&zone=<zone>&content=<content>
  or
  {"ip":"<ip>","server":"<server>","uri":"<uri>","id":<id>,"zone":"<zone>","content":"<content>"}
 */
void
naxsi_log_offending_extra(ngx_http_request_ctx_t* ctx,
                          ngx_http_request_t*     req,
                          ngx_str_t*              name,
                          ngx_str_t*              val,
                          ngx_http_rule_t*        rule,
                          naxsi_match_zone_t      zone,
                          ngx_int_t               target_name)
{

  if (ctx->json_log) {
    naxsi_log_offending_extra_as_json(ctx, req, name, val, rule, zone, target_name);
    return;
  }

  naxsi_log_offending_extra_as_urlencode(ctx, req, name, val, rule, zone, target_name);
}
