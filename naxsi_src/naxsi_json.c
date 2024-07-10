// SPDX-FileCopyrightText: 2016-2019, Thibault 'bui' Koechlin <tko@nbs-system.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <ngx_config.h>

#include <naxsi.h>
#include <naxsi_macros.h>

extern ngx_http_rule_t nx_int__invalid_json;

#define json_char(x) ((x)->src + (x)->off)

ngx_int_t
ngx_http_nx_json_forward(ngx_json_t* js)
{
  while (js->off < js->len && is_whitespace(*json_char(js))) {
    js->off++;
  }
  js->c = *json_char(js);
  return (NGX_OK);
}

/*
** used to fast forward in json POSTS,
** we skip whitespaces/tab/CR/LF
*/
ngx_int_t
ngx_http_nx_json_seek(ngx_json_t* js, unsigned char seek)
{
  ngx_http_nx_json_forward(js);
  if (js->c != seek) {
    return (NGX_ERROR);
  }
  return (NGX_OK);
}

/*
** extract a quoted strings,
** JSON spec only supports double-quoted strings,
** so do we.
*/
ngx_int_t
ngx_http_nx_json_quoted(ngx_json_t* js, ngx_str_t* ve)
{
  u_char *vn_start, *vn_end;

  vn_start = vn_end = NULL;

  return_value_if(*json_char(js) != '"', NGX_ERROR);
  js->off++;
  vn_start = json_char(js);
  /* extract varname inbetween "..."*/
  while (js->off < js->len) {
    /* skip next character if backslashed */
    if (*json_char(js) == '\\') {
      js->off += 2;
      if (js->off >= js->len)
        return (NGX_ERROR);
      continue;
    }
    if (*json_char(js) == '"') {
      vn_end = json_char(js);
      js->off++;
      break;
    }
    js->off++;
  }
  if (!vn_start || !vn_end) {
    return (NGX_ERROR);
  }
  if (!*vn_start || !*vn_end) {
    return (NGX_ERROR);
  }
  ve->data = vn_start;
  ve->len  = vn_end - vn_start;
  return (NGX_OK);
}

/*
** an array is values separated by ','
*/
ngx_int_t
ngx_http_nx_json_array(ngx_json_t* js)
{
  ngx_http_nx_json_forward(js);
  if (js->c != '[' || js->depth > JSON_MAX_DEPTH)
    return (NGX_ERROR);
  js->off++;
  ngx_http_nx_json_forward(js);
  if (js->c == ']') {
    /* empty array */
    js->off++;
    return (NGX_OK);
  }
  do {
    if (ngx_http_nx_json_val(js) != NGX_OK) {
      return (NGX_ERROR);
    }
    ngx_http_nx_json_forward(js);
    if (js->c != ',')
      break;
    js->off++;
  } while (js->off < js->len);

  ngx_http_nx_json_forward(js);
  if (js->c != ']') {
    return (NGX_ERROR);
  }
  js->off++;
  return (NGX_OK);
}

ngx_int_t
ngx_http_nx_json_val(ngx_json_t* js)
{
  ngx_str_t val;
  ngx_str_t empty = ngx_string("");

  val.data = NULL;
  val.len  = 0;

  ngx_http_nx_json_forward(js);
  if (js->c == '"') {
    if (ngx_http_nx_json_quoted(js, &val) != NGX_OK) {
      return (NGX_ERROR);
    }
    /* parse extracted values. */
    if (js->loc_cf->body_rules) {
      ngx_http_basestr_ruleset_n(
        js->r->pool, &js->ckey, &val, js->loc_cf->body_rules, js->r, js->ctx, BODY);
    }
    if (js->main_cf->body_rules) {
      ngx_http_basestr_ruleset_n(
        js->r->pool, &js->ckey, &val, js->main_cf->body_rules, js->r, js->ctx, BODY);
    }
    NX_DEBUG(_debug_json,
             NGX_LOG_DEBUG_HTTP,
             js->r->connection->log,
             0,
             "quoted-JSON '%V' : '%V'",
             &(js->ckey),
             &(val));
    return (NGX_OK);
  }
  if ((js->c >= '0' && js->c <= '9') || js->c == '-') {
    val.data = json_char(js);
    while (js->off < js->len &&
           ((*json_char(js) >= '0' && *json_char(js) <= '9') || *json_char(js) == '.' ||
            *json_char(js) == '+' || *json_char(js) == '-' || *json_char(js) == 'e' ||
            *json_char(js) == 'E')) {
      val.len++;
      js->off++;
    }
    /* parse extracted values. */
    if (js->loc_cf->body_rules) {
      ngx_http_basestr_ruleset_n(
        js->r->pool, &js->ckey, &val, js->loc_cf->body_rules, js->r, js->ctx, BODY);
    }
    if (js->main_cf->body_rules) {
      ngx_http_basestr_ruleset_n(
        js->r->pool, &js->ckey, &val, js->main_cf->body_rules, js->r, js->ctx, BODY);
    }
    NX_DEBUG(_debug_json,
             NGX_LOG_DEBUG_HTTP,
             js->r->connection->log,
             0,
             "JSON '%V' : '%V'",
             &(js->ckey),
             &(val));
    return (NGX_OK);
  }
  if (!strncasecmp((const char*)json_char(js), (const char*)"true", 4) ||
      !strncasecmp((const char*)json_char(js), (const char*)"false", 5) ||
      !strncasecmp((const char*)json_char(js), (const char*)"null", 4)) {
    js->c = *json_char(js);
    /* we don't check static values, do we ?! */
    val.data = json_char(js);
    if (js->c == 'F' || js->c == 'f') {
      js->off += 5;
      val.len = 5;
    } else {
      js->off += 4;
      val.len = 4;
    }
    /* parse extracted values. */
    if (js->loc_cf->body_rules) {
      ngx_http_basestr_ruleset_n(
        js->r->pool, &js->ckey, &val, js->loc_cf->body_rules, js->r, js->ctx, BODY);
    }
    if (js->main_cf->body_rules) {
      ngx_http_basestr_ruleset_n(
        js->r->pool, &js->ckey, &val, js->main_cf->body_rules, js->r, js->ctx, BODY);
    }
    NX_DEBUG(_debug_json,
             NGX_LOG_DEBUG_HTTP,
             js->r->connection->log,
             0,
             "JSON '%V' : '%V'",
             &(js->ckey),
             &(val));
    return (NGX_OK);
  }

  if (js->c == '[') {
    js->depth++;
    if (ngx_http_nx_json_array(js) != NGX_OK) {
      return (NGX_ERROR);
    }
    js->depth--;
    return (NGX_OK);
  }
  if (js->c == '{') {
    /*
    ** if sub-struct, parse key without value :
    ** "foobar" : { "bar" : [1,2,3]} => "foobar" parsed alone.
    ** this is to avoid "foobar" left unparsed, as we won't have
    ** key/value here with "foobar" as a key.
    */
    if (js->loc_cf->body_rules) {
      ngx_http_basestr_ruleset_n(
        js->r->pool, &js->ckey, &empty, js->loc_cf->body_rules, js->r, js->ctx, BODY);
    }
    if (js->main_cf->body_rules) {
      ngx_http_basestr_ruleset_n(
        js->r->pool, &js->ckey, &empty, js->main_cf->body_rules, js->r, js->ctx, BODY);
    }
    NX_DEBUG(_debug_json,
             NGX_LOG_DEBUG_HTTP,
             js->r->connection->log,
             0,
             "sub-struct-JSON '%V' : {...}",
             &(js->ckey));

    js->depth++;
    if (ngx_http_nx_json_obj(js) != NGX_OK) {
      return (NGX_ERROR);
    }
    js->depth--;
    return (NGX_OK);
  }
  return (NGX_ERROR);
}

ngx_int_t
ngx_http_nx_json_obj(ngx_json_t* js)
{
  ngx_http_nx_json_forward(js);
  if (js->c != '{' || js->depth > JSON_MAX_DEPTH)
    return (NGX_ERROR);
  js->off++;
  ngx_http_nx_json_forward(js);
  if (js->c == '}') {
    /* empty object */
    js->off++;
    return (NGX_OK);
  }
  do {
    if (ngx_http_nx_json_quoted(js, &(js->ckey)) != NGX_OK) {
      return (NGX_ERROR);
    }
    if (ngx_http_nx_json_seek(js, ':') != NGX_OK) {
      return (NGX_ERROR);
    }
    js->off++;
    ngx_http_nx_json_forward(js);
    if (ngx_http_nx_json_val(js) != NGX_OK) {
      return (NGX_ERROR);
    }
    ngx_http_nx_json_forward(js);
    /* another element ? */
    if (js->c != ',') {
      break;
    }
    js->off++;
    ngx_http_nx_json_forward(js);
  } while (js->off < js->len);

  ngx_http_nx_json_forward(js);
  if (js->c != '}') {
    return (NGX_ERROR);
  }
  js->off++;
  return (NGX_OK);
}

/*
** Parse a JSON request
*/
void
ngx_http_naxsi_json_parse(ngx_http_request_ctx_t* ctx,
                          ngx_http_request_t*     r,
                          u_char*                 src,
                          u_int                   len)
{
  ngx_json_t* js;

  js = ngx_pcalloc(r->pool, sizeof(ngx_json_t));
  if (!js)
    return;
  js->json.data = js->src = src;
  js->json.len = js->len = len;
  js->r                  = r;
  js->ctx                = ctx;
  js->loc_cf             = ngx_http_get_module_loc_conf(r, ngx_http_naxsi_module);
  js->main_cf            = ngx_http_get_module_main_conf(r, ngx_http_naxsi_module);

  if (ngx_http_nx_json_val(js) != NGX_OK) {
    ngx_http_apply_rulematch_v_n(&nx_int__invalid_json, ctx, r, NULL, NULL, BODY, 1, 0);
    NX_DEBUG(_debug_json,
             NGX_LOG_DEBUG_HTTP,
             js->r->connection->log,
             0,
             "nx_json_val returned error, apply invalid_json.");
  }
  ngx_http_nx_json_forward(js);
  if (js->off != js->len) {
    ngx_http_apply_rulematch_v_n(&nx_int__invalid_json, ctx, r, NULL, NULL, BODY, 1, 0);
  }
  return;
}
