// SPDX-FileCopyrightText: 2016-2019, Thibault 'bui' Koechlin <tko@nbs-system.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <ngx_config.h>

#include <naxsi.h>
#include <naxsi_macros.h>
#include <naxsi_net.h>

char*
strnchr(const char* s, int c, int len)
{
  int cpt;
  for (cpt = 0; cpt < len; cpt++) {
    if (s[cpt] == c) {
      return ((char*)s + cpt);
    }
  }
  return (NULL);
}

static char*
strncasechr(const char* s, int c, int len)
{
  int cpt;
  for (cpt = 0; cpt < len; cpt++) {
    if (tolower(s[cpt]) == c) {
      return ((char*)s + cpt);
    }
  }
  return (NULL);
}

/*
** strstr: faster, stronger, harder
** (because strstr from libc is very slow)
*/
char*
strfaststr(const unsigned char* haystack,
           unsigned int         hl,
           const unsigned char* needle,
           unsigned int         nl)
{
  char *cpt, *found, *end;
  if (hl < nl || !haystack || !needle || !nl || !hl)
    return (NULL);
  cpt = (char*)haystack;
  end = (char*)haystack + hl;
  while (cpt < end) {
    found = strncasechr((const char*)cpt, (int)needle[0], hl);
    if (!found) {
      return (NULL);
    }
    if (nl == 1) {
      return (found);
    }
    if (!strncasecmp((const char*)found + 1, (const char*)needle + 1, nl - 1)) {
      return ((char*)found);
    } else {
      if (found + nl >= end) {
        break;
      }
      if (found + nl < end) {
        /* the haystack is shrinking */
        cpt = found + 1;
        hl  = (unsigned int)(end - cpt);
      }
    }
  }
  return (NULL);
}

u_int
naxsi_escape_nullbytes(ngx_str_t* str)
{

  size_t i         = 0;
  u_int  nullbytes = 0;

  for (i = 0; i < str->len; i++) {
    if (str->data[i] == 0) {
      str->data[i] = '0';
      nullbytes++;
    }
  }
  return nullbytes;
}

/*
** Patched ngx_unescape_uri :
** The original one does not care if the character following % is in valid
*range.
** For example, with the original one :
** '%uff' -> 'uff'
*/
static int
naxsi_unescape_uri(u_char** dst, u_char** src, size_t size, ngx_uint_t type)
{
  u_char *d, *s, ch, c, decoded;
  int     bad = 0;

  enum
  {
    sw_usual = 0,
    sw_quoted,
    sw_quoted_second
  } state;

  d = *dst;
  s = *src;

  state   = sw_usual;
  decoded = 0;

  while (size--) {

    ch = *s++;

    switch (state) {
      case sw_usual:
        if (ch == '%') {
          state = sw_quoted;
          break;
        }

        *d++ = ch;
        break;

      case sw_quoted:

        if (ch >= '0' && ch <= '9') {
          decoded = (u_char)(ch - '0');
          state   = sw_quoted_second;
          break;
        }

        c = (u_char)(ch | 0x20);
        if (c >= 'a' && c <= 'f') {
          decoded = (u_char)(c - 'a' + 10);
          state   = sw_quoted_second;
          break;
        }

        /* the invalid quoted character */
        bad++;
        state = sw_usual;
        *d++  = '%';
        *d++  = ch;
        break;

      case sw_quoted_second:

        state = sw_usual;

        if (ch >= '0' && ch <= '9') {
          ch = (u_char)((decoded << 4) + ch - '0');

          *d++ = ch;

          break;
        }

        c = (u_char)(ch | 0x20);
        if (c >= 'a' && c <= 'f') {
          ch = (u_char)((decoded << 4) + c - 'a' + 10);

          *d++ = ch;

          break;
        }
        /* the invalid quoted character */
        /* as it happened in the 2nd part of quoted character,
           we need to restore the decoded char as well. */
        *d++ = '%';
        *d++ = *(s - 2);
        *d++ = *(s - 1);
        bad++;
        break;
    }
  }

  *dst = d;
  *src = s;

  return (bad);
}

/*
   unescape routine returns a sum of :
 - number of nullbytes present
 - number of invalid url-encoded characters
*/
int
naxsi_unescape(ngx_str_t* str)
{
  u_char *dst, *src;
  u_int   nullbytes = 0, bad = 0, i;

  dst = str->data;
  src = str->data;

  bad      = naxsi_unescape_uri(&src, &dst, str->len, 0);
  str->len = src - str->data;
  // tmp hack fix, avoid %00 & co (null byte) encoding :p
  for (i = 0; i < str->len; i++) {
    if (str->data[i] == 0x0) {
      nullbytes++;
      str->data[i] = '0';
    }
  }
  return (nullbytes + bad);
}

/* push rule into disabled rules. */
static ngx_int_t
ngx_http_wlr_push_disabled(ngx_conf_t* cf, ngx_http_naxsi_loc_conf_t* dlc, ngx_http_rule_t* curr)
{
  ngx_http_rule_t** dr;

  if (!dlc->disabled_rules)
    dlc->disabled_rules = ngx_array_create(cf->pool, 4, sizeof(ngx_http_rule_t*));
  if (!dlc->disabled_rules)
    return (NGX_ERROR);
  dr = ngx_array_push(dlc->disabled_rules);
  if (!dr)
    return (NGX_ERROR);
  *dr = (ngx_http_rule_t*)curr;
  return (NGX_OK);
}

/* merge the two rules into father_wl, meaning
   ids. Not locations, as we are getting rid of it */
static ngx_int_t
ngx_http_wlr_merge(ngx_conf_t* cf, ngx_http_whitelist_rule_t* father_wl, ngx_http_rule_t* curr)
{
  ngx_uint_t i;
  ngx_int_t* tmp_ptr;

  NX_LOG_DEBUG(_debug_whitelist,
               NGX_LOG_EMERG,
               cf,
               0,
               "[naxsi] %s wl(s)",
               father_wl->ids ? "merging similar" : "adding");
  if (!father_wl->ids) {
    father_wl->ids = ngx_array_create(cf->pool, 3, sizeof(ngx_int_t));
    if (!father_wl->ids) {
      return (NGX_ERROR);
    }
  }

  if (curr->wlid_array->nelts < 1) {
    return (NGX_ERROR);
  }

  ngx_int_t* wl_ids = (ngx_int_t*)curr->wlid_array->elts;
  for (i = 0; i < curr->wlid_array->nelts; i++) {
    tmp_ptr = ngx_array_push(father_wl->ids);
    if (!tmp_ptr) {
      return (NGX_ERROR);
    }
    *tmp_ptr = wl_ids[i];
  }
  return (NGX_OK);
}

/*check rule, returns associed zone, as well as location index.
  location index refers to $URL:bla or $ARGS_VAR:bla */
#define custloc_array(x) ((ngx_http_custom_rule_location_t*)x)

static ngx_int_t
ngx_http_wlr_identify(ngx_conf_t*                cf,
                      ngx_http_naxsi_loc_conf_t* dlc,
                      ngx_http_rule_t*           curr,
                      int*                       zone,
                      int*                       uri_idx,
                      int*                       name_idx)
{

  ngx_uint_t i;

  /*
    identify global match zones (|ARGS|BODY|HEADERS|URL|FILE_EXT)
   */
  if (curr->br->any) {
    *zone = ANY;
  } else if (curr->br->body || curr->br->body_var) {
    *zone = BODY;
  } else if (curr->br->headers || curr->br->headers_var) {
    *zone = HEADERS;
  } else if (curr->br->args || curr->br->args_var) {
    *zone = ARGS;
  } else if (curr->br->url) { /*don't assume that named $URL means zone is URL.*/
    *zone = URL;
  } else if (curr->br->file_ext) {
    *zone = FILE_EXT;
  }
  /*
    if we're facing a WL in the style $URL:/bla|ARGS (or any other zone),
    push it to
   */
  for (i = 0; i < curr->br->custom_locations->nelts; i++) {
    ngx_http_custom_rule_location_t* loc = custloc_array(curr->br->custom_locations->elts);
    /*
      locate target URL if exists ($URL:/bla|ARGS) or ($URL:/bla|$ARGS_VAR:foo)
     */
    if (loc[i].specific_url) {
      NX_LOG_DEBUG(
        _debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "whitelist has URI %V", &(loc[i].target));

      *uri_idx = i;
    }
    /*
      identify named match zones ($ARGS_VAR:bla|$HEADERS_VAR:bla|$BODY_VAR:bla)
    */
    if (loc[i].body_var) {
      NX_LOG_DEBUG(_debug_whitelist_heavy,
                   NGX_LOG_EMERG,
                   cf,
                   0,
                   "whitelist has body_var %V",
                   &(loc[i].target));

      /*#217 : scream on incorrect rules*/
      if (*name_idx != -1) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "whitelist can't target more than one BODY item.");
        return (NGX_ERROR);
      }
      *name_idx = i;
      *zone     = BODY;
    }
    if (loc[i].headers_var) {
      NX_LOG_DEBUG(_debug_whitelist_heavy,
                   NGX_LOG_EMERG,
                   cf,
                   0,
                   "whitelist has header_var %V",
                   &(loc[i].target));

      /*#217 : scream on incorrect rules*/
      if (*name_idx != -1) {
        ngx_conf_log_error(
          NGX_LOG_EMERG, cf, 0, "whitelist can't target more than one HEADER item.");
        return (NGX_ERROR);
      }
      *name_idx = i;
      *zone     = HEADERS;
    }
    if (loc[i].args_var) {
      NX_LOG_DEBUG(
        _debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "whitelist has arg_var %V", &(loc[i].target));

      /*#217 : scream on incorrect rules*/
      if (*name_idx != -1) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "whitelist can't target more than one ARGS item.");
        return (NGX_ERROR);
      }

      *name_idx = i;
      *zone     = ARGS;
    }
  }
  if (*zone == -1) {
    NX_LOG_DEBUG(_debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "whitelist has missing zone");
    return (NGX_ERROR);
  }
  return (NGX_OK);
}

ngx_http_whitelist_rule_t*
ngx_http_wlr_find(ngx_conf_t*                cf,
                  ngx_http_naxsi_loc_conf_t* dlc,
                  ngx_http_rule_t*           curr,
                  int                        zone,
                  int                        uri_idx,
                  int                        name_idx,
                  char**                     fullname)
{
  ngx_uint_t i;

  /* Create unique string for rule, and try to find it in existing rules.*/
  /*name AND uri*/

  ngx_http_custom_rule_location_t* loc = custloc_array(curr->br->custom_locations->elts);

  if (uri_idx != -1 && name_idx != -1) {
    NX_LOG_DEBUG(_debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "whitelist has uri + name");

    /* allocate one extra byte in case curr->br->target_name is set. */
    *fullname = ngx_pcalloc(cf->pool, loc[name_idx].target.len + loc[uri_idx].target.len + 3);
    /* if WL targets variable name instead of content, prefix hash with '#' */
    if (curr->br->target_name) {
      NX_LOG_DEBUG(_debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "whitelist targets |NAME");

      strcat(*fullname, (const char*)"#");
    }
    strncat(*fullname, (const char*)loc[uri_idx].target.data, loc[uri_idx].target.len);
    strcat(*fullname, (const char*)"#");
    strncat(*fullname, (const char*)loc[name_idx].target.data, loc[name_idx].target.len);
  }
  /* only uri */
  else if (uri_idx != -1 && name_idx == -1) {
    // XXX set flag only_uri
    *fullname = ngx_pcalloc(cf->pool, loc[uri_idx].target.len + 3);
    if (curr->br->target_name) {
      NX_LOG_DEBUG(_debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "whitelist targets |NAME");

      strcat(*fullname, (const char*)"#");
    }

    strncat(*fullname, (const char*)loc[uri_idx].target.data, loc[uri_idx].target.len);
  }
  /* only name */
  else if (name_idx != -1) {
    NX_LOG_DEBUG(_debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "whitelist is name only");

    *fullname = ngx_pcalloc(cf->pool, loc[name_idx].target.len + 2);
    if (curr->br->target_name) {
      strcat(*fullname, (const char*)"#");
    }
    strncat(*fullname, (const char*)loc[name_idx].target.data, loc[name_idx].target.len);
  }
  /* problem houston */
  else {
    return (NULL);
  }

  ngx_http_whitelist_rule_t* wlr = (ngx_http_whitelist_rule_t*)dlc->tmp_wlr->elts;
  for (i = 0; i < dlc->tmp_wlr->nelts; i++)
    if (!strcmp((const char*)*fullname, (const char*)wlr[i].name->data) &&
        wlr[i].zone == (ngx_uint_t)zone) {
      NX_LOG_DEBUG(
        _debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "found existing 'same' WL : %V", wlr[i].name);

      return (&wlr[i]);
    }
  return (NULL);
}

static ngx_int_t
push_entry_with_whitelist_rule(ngx_array_t* array, ngx_http_whitelist_rule_t* wlr)
{
  if (!array) {
    // ignore the array if NULL.
    return (NGX_OK);
  }

  ngx_hash_key_t* key = (ngx_hash_key_t*)ngx_array_push(array);
  if (!key) {
    return (NGX_ERROR);
  }

  ngx_memset(key, 0, sizeof(ngx_hash_key_t));
  key->key      = *wlr->name;
  key->key_hash = ngx_hash_key_lc(wlr->name->data, wlr->name->len);
  key->value    = wlr;
  return (NGX_OK);
}

static ngx_int_t
ngx_http_wlr_finalize_hashtables(ngx_conf_t* cf, ngx_http_naxsi_loc_conf_t* dlc)
{
  int             get_sz = 0, headers_sz = 0, body_sz = 0, uri_sz = 0;
  ngx_array_t *   get_ar = NULL, *headers_ar = NULL, *body_ar = NULL, *uri_ar = NULL;
  ngx_hash_init_t hash_init;
  ngx_uint_t      i;

  NX_LOG_DEBUG(_debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "finalizing hashtables");

  if (dlc->whitelist_rules) {
    ngx_http_whitelist_rule_t* wlr = (ngx_http_whitelist_rule_t*)dlc->tmp_wlr->elts;
    for (i = 0; i < dlc->tmp_wlr->nelts; i++) {
      switch (wlr[i].zone) {
        case ANY:
          get_sz++;
          uri_sz++;
          headers_sz++;
          body_sz++;
          break;
        case FILE_EXT:
        case BODY:
          body_sz++;
          break;
        case HEADERS:
          headers_sz++;
          break;
        case URL:
          uri_sz++;
          break;
        case ARGS:
          get_sz++;
          break;
        case UNKNOWN:
        default:
          return (NGX_ERROR);
      }
    }
    NX_LOG_DEBUG(_debug_whitelist_heavy,
                 NGX_LOG_EMERG,
                 cf,
                 0,
                 "nb items : body:%d headers:%d uri:%d get:%d",
                 body_sz,
                 headers_sz,
                 uri_sz,
                 get_sz);

    if (get_sz) {
      get_ar = ngx_array_create(cf->pool, get_sz, sizeof(ngx_hash_key_t));
    }
    if (headers_sz) {
      headers_ar = ngx_array_create(cf->pool, headers_sz, sizeof(ngx_hash_key_t));
    }
    if (body_sz) {
      body_ar = ngx_array_create(cf->pool, body_sz, sizeof(ngx_hash_key_t));
    }
    if (uri_sz) {
      uri_ar = ngx_array_create(cf->pool, uri_sz, sizeof(ngx_hash_key_t));
    }

    for (i = 0; i < dlc->tmp_wlr->nelts; i++) {
      switch (wlr[i].zone) {
        case ANY: {
          if (push_entry_with_whitelist_rule(body_ar, &wlr[i]) == NGX_ERROR ||
              push_entry_with_whitelist_rule(headers_ar, &wlr[i]) == NGX_ERROR ||
              push_entry_with_whitelist_rule(uri_ar, &wlr[i]) == NGX_ERROR ||
              push_entry_with_whitelist_rule(get_ar, &wlr[i]) == NGX_ERROR) {
            return (NGX_ERROR);
          }
        } break;
        case FILE_EXT:
          /* fall-thru */
        case BODY: {
          if (push_entry_with_whitelist_rule(body_ar, &wlr[i]) == NGX_ERROR) {
            return (NGX_ERROR);
          }
        } break;
        case HEADERS: {
          if (push_entry_with_whitelist_rule(headers_ar, &wlr[i]) == NGX_ERROR) {
            return (NGX_ERROR);
          }
        } break;
        case URL: {
          if (push_entry_with_whitelist_rule(uri_ar, &wlr[i]) == NGX_ERROR) {
            return (NGX_ERROR);
          }
        } break;
        case ARGS: {
          if (push_entry_with_whitelist_rule(get_ar, &wlr[i]) == NGX_ERROR) {
            return (NGX_ERROR);
          }
        } break;
        default:
          return (NGX_ERROR);
      }
    }
    hash_init.key         = &ngx_hash_key_lc;
    hash_init.pool        = cf->pool;
    hash_init.temp_pool   = NULL;
    hash_init.max_size    = 1024;
    hash_init.bucket_size = 512;

    if (body_ar) {
      dlc->wlr_body_hash = (ngx_hash_t*)ngx_pcalloc(cf->pool, sizeof(ngx_hash_t));
      hash_init.hash     = dlc->wlr_body_hash;
      hash_init.name     = "wlr_body_hash";
      if (ngx_hash_init(&hash_init, (ngx_hash_key_t*)body_ar->elts, body_ar->nelts) != NGX_OK) {
        ngx_conf_log_error(
          NGX_LOG_EMERG, cf, 0, "$BODY hashtable init failed"); /* LCOV_EXCL_LINE */
        return (NGX_ERROR);                                     /* LCOV_EXCL_LINE */
      } else {
        NX_LOG_DEBUG(_debug_whitelist, NGX_LOG_EMERG, cf, 0, "$BODY hashtable init successed !");
      }
    }
    if (uri_ar) {
      dlc->wlr_url_hash = (ngx_hash_t*)ngx_pcalloc(cf->pool, sizeof(ngx_hash_t));
      if (dlc->wlr_url_hash) {
        hash_init.hash = dlc->wlr_url_hash;
        hash_init.name = "wlr_url_hash";
      }
      if (ngx_hash_init(&hash_init, (ngx_hash_key_t*)uri_ar->elts, uri_ar->nelts) != NGX_OK) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "$URL hashtable init failed"); /* LCOV_EXCL_LINE */
        return (NGX_ERROR);                                                     /* LCOV_EXCL_LINE */
      } else {
        NX_LOG_DEBUG(_debug_whitelist, NGX_LOG_EMERG, cf, 0, "$URL hashtable init successed !");
      }
    }
    if (get_ar) {
      dlc->wlr_args_hash = (ngx_hash_t*)ngx_pcalloc(cf->pool, sizeof(ngx_hash_t));
      hash_init.hash     = dlc->wlr_args_hash;
      hash_init.name     = "wlr_args_hash";
      if (ngx_hash_init(&hash_init, (ngx_hash_key_t*)get_ar->elts, get_ar->nelts) !=
          NGX_OK) { /* LCOV_EXCL_LINE */
        ngx_conf_log_error(
          NGX_LOG_EMERG, cf, 0, "$ARGS hashtable init failed"); /* LCOV_EXCL_LINE */
        return (NGX_ERROR);
      } else {
        NX_LOG_DEBUG(_debug_whitelist,
                     NGX_LOG_EMERG,
                     cf,
                     0,
                     "$ARGS hashtable init successed %d !",
                     dlc->wlr_args_hash->size);
      }
    }
    if (headers_ar) {
      dlc->wlr_headers_hash = (ngx_hash_t*)ngx_pcalloc(cf->pool, sizeof(ngx_hash_t));
      if (dlc->wlr_headers_hash) {
        hash_init.hash = dlc->wlr_headers_hash;
        hash_init.name = "wlr_headers_hash";
      }
      if (ngx_hash_init(&hash_init, (ngx_hash_key_t*)headers_ar->elts, headers_ar->nelts) !=
          NGX_OK) {
        ngx_conf_log_error(
          NGX_LOG_EMERG, cf, 0, "$HEADERS hashtable init failed"); /* LCOV_EXCL_LINE */
        return (NGX_ERROR);                                        /* LCOV_EXCL_LINE */
      } else {
        NX_LOG_DEBUG(_debug_whitelist,
                     NGX_LOG_EMERG,
                     cf,
                     0,
                     "$HEADERS hashtable init successed %d !",
                     dlc->wlr_headers_hash->size);
      }
    }
  }

  if (dlc->ignore_ips) {
    hash_init.key         = &ngx_hash_key;
    hash_init.pool        = cf->pool;
    hash_init.temp_pool   = NULL;
    hash_init.max_size    = 1024 * 128;
    hash_init.bucket_size = 4096;
    hash_init.name        = "ignore_ips";
    hash_init.hash        = dlc->ignore_ips;
    if (ngx_hash_init(&hash_init, dlc->ignore_ips_ha.keys.elts, dlc->ignore_ips_ha.keys.nelts) !=
        NGX_OK) {
      ngx_conf_log_error(
        NGX_LOG_EMERG, cf, 0, "IPs whitelist hashtable init failed"); // LCOV_EXCL_LINE //
      return (NGX_ERROR);                                             // LCOV_EXCL_LINE //
    } else {
      NX_LOG_DEBUG(_debug_whitelist, NGX_LOG_EMERG, cf, 0, "IPs whitelist hashtable initialized!");
    }
  }

  return (NGX_OK);
}

/*
** This function will take the whitelist basicrules generated during the
*configuration
** parsing phase, and aggregate them to build hashtables according to the
*matchzones.
**
** As whitelist can be in the form :
** "mz:$URL:bla|$ARGS_VAR:foo"
** "mz:$URL:bla|ARGS"
** "mz:$HEADERS_VAR:Cookie"
** ...
**
** So, we will aggregate all the rules that are pointing to the same URL
*together,
** as well as rules targetting the same argument name / zone.
*/

ngx_int_t
ngx_http_naxsi_create_hashtables_n(ngx_http_naxsi_loc_conf_t* dlc, ngx_conf_t* cf)
{
  int                        zone, uri_idx, name_idx, ret;
  ngx_http_rule_t*           curr_r /*, *father_r*/;
  ngx_http_whitelist_rule_t* father_wlr;
  ngx_http_rule_t**          rptr;
  ngx_regex_compile_t*       rgc;
  char*                      fullname;
  ngx_uint_t                 i;

  NX_LOG_DEBUG(_debug_whitelist_heavy && (!dlc->whitelist_rules || dlc->whitelist_rules->nelts < 1),
               NGX_LOG_EMERG,
               cf,
               0,
               "No whitelist registred, but it's your call.");

  if (dlc->whitelist_rules) {

    NX_LOG_DEBUG(_debug_whitelist_heavy,
                 NGX_LOG_EMERG,
                 cf,
                 0,
                 "Building whitelist hashtables, %d items in list",
                 dlc->whitelist_rules->nelts);

    dlc->tmp_wlr =
      ngx_array_create(cf->pool, dlc->whitelist_rules->nelts, sizeof(ngx_http_whitelist_rule_t));
    /* iterate through each stored whitelist rule. */
    for (i = 0; i < dlc->whitelist_rules->nelts; i++) {
      uri_idx = name_idx = zone = -1;
      /*a whitelist is in fact just another basic_rule_t */
      curr_r = &(((ngx_http_rule_t*)(dlc->whitelist_rules->elts))[i]);
      NX_LOG_DEBUG(_debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "Processing wl %d/%p", i, curr_r);

      /*no custom location at all means that the rule is disabled */
      if (!curr_r->br->custom_locations) {
        NX_LOG_DEBUG(_debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "WL %d is a disable rule.", i);

        if (ngx_http_wlr_push_disabled(cf, dlc, curr_r) == NGX_ERROR) {
          return (NGX_ERROR);
        }
        continue;
      }
      ret = ngx_http_wlr_identify(cf, dlc, curr_r, &zone, &uri_idx, &name_idx);
      if (ret != NGX_OK) { /* LCOV_EXCL_START */
        ngx_conf_log_error(
          NGX_LOG_EMERG, cf, 0, "Following whitelist doesn't target any zone or is incorrect :");
        if (name_idx != -1) {
          ngx_conf_log_error(NGX_LOG_EMERG,
                             cf,
                             0,
                             "bad whitelist target name : %V",
                             &(custloc_array(curr_r->br->custom_locations->elts)[name_idx].target));
        } else {
          ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "whitelist has no target name.");
        }
        if (uri_idx != -1) {
          ngx_conf_log_error(NGX_LOG_EMERG,
                             cf,
                             0,
                             "bad whitelist target uri : %V",
                             &(custloc_array(curr_r->br->custom_locations->elts)[uri_idx].target));
        } else {
          ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "whitelists has no target uri.");
        }
        return (NGX_ERROR);
      } /* LCOV_EXCL_STOP */
      curr_r->br->zone = zone;
      /*
      ** Handle regular-expression-matchzone rules :
      ** Store them in a separate linked list, parsed
      ** at runtime.
      */
      if (curr_r->br->rx_mz) {
        if (!dlc->rxmz_wlr) {
          dlc->rxmz_wlr = ngx_array_create(cf->pool, 1, sizeof(ngx_http_rule_t*));
          if (!dlc->rxmz_wlr) {
            return (NGX_ERROR); /* LCOV_EXCL_LINE */
          }
        }
        ngx_http_custom_rule_location_t* cust_loc =
          custloc_array(curr_r->br->custom_locations->elts);
        if (name_idx != -1 && !cust_loc[name_idx].target_rx) {
          cust_loc[name_idx].target_rx = rgc = ngx_pcalloc(cf->pool, sizeof(ngx_regex_compile_t));
          if (rgc) {
            rgc->options  = NAXSI_REGEX_OPTIONS;
            rgc->pattern  = cust_loc[name_idx].target;
            rgc->pool     = cf->pool;
            rgc->err.len  = 0;
            rgc->err.data = NULL;
          }
          // cust_loc[name_idx].target;
          if (!rgc || ngx_regex_compile(rgc) != NGX_OK) {
            return (NGX_ERROR);
          }
        }
        if (uri_idx != -1 && !cust_loc[uri_idx].target_rx) {
          cust_loc[uri_idx].target_rx = rgc = ngx_pcalloc(cf->pool, sizeof(ngx_regex_compile_t));
          if (rgc) {
            rgc->options  = NAXSI_REGEX_OPTIONS;
            rgc->pattern  = cust_loc[uri_idx].target;
            rgc->pool     = cf->pool;
            rgc->err.len  = 0;
            rgc->err.data = NULL;
          }
          if (!rgc || ngx_regex_compile(rgc) != NGX_OK) {
            return (NGX_ERROR);
          }
        }

        rptr = ngx_array_push(dlc->rxmz_wlr);
        if (!rptr) {
          return (NGX_ERROR);
        }
        *rptr = curr_r;
        continue;
      }
      /*
      ** Handle static match-zones for hashtables
      */
      father_wlr = ngx_http_wlr_find(cf, dlc, curr_r, zone, uri_idx, name_idx, (char**)&fullname);
      if (!father_wlr) {
        NX_LOG_DEBUG(_debug_whitelist_heavy,
                     NGX_LOG_EMERG,
                     cf,
                     0,
                     "creating fresh WL [%s] uri_idx(%d) name_idx(%d).",
                     fullname,
                     uri_idx,
                     name_idx);

        /* creates a new whitelist rule in the right place.
           setup name and zone, create a new (empty) whitelist_location, as well
           as a new (empty) id aray. */
        father_wlr = ngx_array_push(dlc->tmp_wlr);
        if (!father_wlr) {
          return (NGX_ERROR);
        }
        memset(father_wlr, 0, sizeof(ngx_http_whitelist_rule_t));
        father_wlr->name = ngx_pcalloc(cf->pool, sizeof(ngx_str_t));
        if (!father_wlr->name) {
          return (NGX_ERROR);
        }
        father_wlr->name->len  = strlen((const char*)fullname);
        father_wlr->name->data = (unsigned char*)fullname;
        father_wlr->zone       = zone;

        NX_LOG_DEBUG(
          _debug_whitelist_heavy, NGX_LOG_EMERG, cf, 0, "father_wlr '%V'.", father_wlr->name);

        /* If there is URI and no name idx, specify it,
           so that WL system won't get fooled by an argname like an URL */
        if (uri_idx != -1 && name_idx == -1) {
          father_wlr->uri_only = 1;
        }
        /* If target_name is present in son, report it. */
        if (curr_r->br->target_name) {
          father_wlr->target_name = curr_r->br->target_name;
        }
      }
      /*merges the two whitelist rules together, including custom_locations. */
      if (ngx_http_wlr_merge(cf, father_wlr, curr_r) != NGX_OK) {
        return (NGX_ERROR);
      }
    }
  }

  /* and finally, build the hashtables for various zones. */
  if (ngx_http_wlr_finalize_hashtables(cf, dlc) != NGX_OK)
    return (NGX_ERROR);
  /* TODO : Free old whitelist_rules (dlc->whitelist_rules)*/
  return (NGX_OK);
}

static const char* json_hex = "0123456789abcdef";

static char*
naxsi_escape_json_string(char* out, const char* end, const u_char* val, size_t length)
{
  size_t max_len = end - out;
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
  if (out < end) {
    *out++ = '"';
  }
  return out;
}

char*
naxsi_log_as_json_string(char*         out,
                         const char*   end,
                         const char*   key,
                         const u_char* val,
                         size_t        val_len)
{
  int offset = snprintf(out, (end - out), "\"%s\":", key);
  if (offset < 1) {
    return out;
  }
  return naxsi_escape_json_string(out + offset, end, val, val_len);
}

char*
naxsi_log_as_json_number(char* out, const char* end, const char* key, int number)
{
  int offset = snprintf(out, (end - out), "\"%s\":%d", key, number);
  return offset < 1 ? out : (out + offset);
}

/*
  function used for intensive log if dynamic flag is set.
  Output format :
  ip=<ip>&server=<server>&uri=<uri>&id=<id>&zone=<zone>&content=<content>
 */

static const char* naxsi_match_zones[] = { "HEADERS",  "URL", "ARGS",    "BODY",
                                           "FILE_EXT", "ANY", "UNKNOWN", NULL };

static void
naxsi_log_offending_as_json(ngx_http_request_ctx_t* ctx,
                            ngx_http_request_t*     req,
                            ngx_str_t*              name,
                            ngx_str_t*              val,
                            ngx_http_rule_t*        rule,
                            naxsi_match_zone_t      zone,
                            ngx_int_t               target_name)
{

  ngx_str_t*                 str = NULL;
  ngx_http_naxsi_loc_conf_t* cf  = NULL;

  char   json[NAXSI_LOG_JSON_STRLEN];
  char * out = json + 1, *end = (json + sizeof(json)) - 2;
  u_char req_id[NAXSI_REQUEST_ID_STRLEN];

  ngx_hex_dump(req_id, ctx->request_id, NAXSI_REQUEST_ID_SIZE);

  // json object begin
  json[0] = '{';

  // ip address
  str    = &req->connection->addr_text;
  out    = naxsi_log_as_json_string(out, end, "ip", str->data, str->len);
  *out++ = ',';
  if (out >= end) {
    goto log_json;
  }

  // server
  str    = &req->headers_in.server;
  out    = naxsi_log_as_json_string(out, end, "server", str->data, str->len);
  *out++ = ',';
  if (out >= end) {
    goto log_json;
  }

  // request id
  out    = naxsi_log_as_json_string(out, end, "rid", req_id, NAXSI_REQUEST_ID_STRLEN - 1);
  *out++ = ',';
  if (out >= end) {
    goto log_json;
  }

  // uri
  str    = &req->uri;
  out    = naxsi_log_as_json_string(out, end, "uri", str->data, str->len);
  *out++ = ',';
  if (out >= end) {
    goto log_json;
  }

  // rule id
  out    = naxsi_log_as_json_number(out, end, "id", rule->rule_id);
  *out++ = ',';
  if (out >= end) {
    goto log_json;
  }

  // zone
  int offset = snprintf(
    out, (end - out), "\"zone\":\"%s%s\",", naxsi_match_zones[zone], target_name ? "|NAME" : "");
  if (offset < 1 || (out + offset) >= end) {
    goto log_json;
  }
  out += offset;

  // var_name
  out    = naxsi_log_as_json_string(out, end, "var_name", name->data, name->len);
  *out++ = ',';
  if (out >= end) {
    goto log_json;
  }

  // content
  out = naxsi_log_as_json_string(out, end, "content", val->data, val->len);
  if (out < end) {
    // json object end
    *out++ = '}';
  }

log_json:
  // always ensure last byte is zero
  if (out < end) {
    *out = 0;
  } else {
    *end = 0;
  }

  cf = ngx_http_get_module_loc_conf(req, ngx_http_naxsi_module);
  ngx_log_error(NGX_LOG_ERR, cf->log ? cf->log : req->connection->log, 0, "%s", json);
}

static int
naxsi_log_escape_string(ngx_http_request_t* req,
                        ngx_str_t*          escaped,
                        ngx_str_t*          string,
                        ngx_str_t*          empty)
{
  if (string->len < 1) {
    *escaped = *empty;
    return 1;
  }

  escaped->len =
    string->len + (2 * ngx_escape_uri(NULL, string->data, string->len, NGX_ESCAPE_URI_COMPONENT));
  escaped->data = ngx_pcalloc(req->pool, escaped->len + 1);
  if (escaped->data == NULL) {
    return 0;
  }
  ngx_escape_uri(escaped->data, string->data, string->len, NGX_ESCAPE_URI_COMPONENT);
  return 1;
}

void
naxsi_log_offending(ngx_http_request_ctx_t* ctx,
                    ngx_http_request_t*     req,
                    ngx_str_t*              name,
                    ngx_str_t*              val,
                    ngx_http_rule_t*        rule,
                    naxsi_match_zone_t      zone,
                    ngx_int_t               target_name)
{

  if (ctx->json_log) {
    naxsi_log_offending_as_json(ctx, req, name, val, rule, zone, target_name);
    return;
  }

  ngx_http_naxsi_loc_conf_t* cf;
  ngx_str_t                  tmp_uri = { 0 }, tmp_val = { 0 }, tmp_name = { 0 };
  ngx_str_t                  empty                               = ngx_string("");
  u_char                     req_id[NAXSI_REQUEST_ID_STRLEN + 1] = { 0 };

  ngx_hex_dump(req_id, ctx->request_id, NAXSI_REQUEST_ID_SIZE);

  cf = ngx_http_get_module_loc_conf(req, ngx_http_naxsi_module);

  if (naxsi_log_escape_string(req, &tmp_uri, &req->uri, &empty) == 0 ||
      naxsi_log_escape_string(req, &tmp_val, val, &empty) == 0 ||
      naxsi_log_escape_string(req, &tmp_name, name, &empty) == 0) {
    goto end;
  }

  ngx_log_error(NGX_LOG_ERR,
                cf->log ? cf->log : req->connection->log,
                0,
                "NAXSI_EXLOG: "
                "ip=%V&server=%V&rid=%s&uri=%V&id=%d&zone=%s%s&var_name=%V&content=%V",
                &(req->connection->addr_text),
                &(req->headers_in.server),
                (char*)req_id,
                &(tmp_uri),
                rule->rule_id,
                naxsi_match_zones[zone],
                target_name ? "|NAME" : "",
                &(tmp_name),
                &(tmp_val));

end:
  if (tmp_val.len > 0) {
    ngx_pfree(req->pool, tmp_val.data);
  }
  if (tmp_name.len > 0) {
    ngx_pfree(req->pool, tmp_name.data);
  }
  if (tmp_uri.len > 0) {
    ngx_pfree(req->pool, tmp_uri.data);
  }
}

/*
** Used to check matched rule ID against wl IDs
** Returns 1 if rule is whitelisted, 0 else
*/
int
nx_check_ids(ngx_int_t match_id, ngx_array_t* wl_ids)
{

  int          negative = 0;
  unsigned int i;

  for (i = 0; i < wl_ids->nelts; i++) {
    if (((ngx_int_t*)wl_ids->elts)[i] == match_id)
      return (1);
    if (((ngx_int_t*)wl_ids->elts)[i] == 0)
      return (1);
    /* manage negative whitelists, except for internal rules */
    if (((ngx_int_t*)wl_ids->elts)[i] < 0 && match_id >= 1000) {
      negative = 1;
      /* negative wl excludes this one.*/
      if (match_id == -((ngx_int_t*)wl_ids->elts)[i]) {
        return (0);
      }
    }
  }
  return (negative == 1);
}

static const cidr_t _illegal_cidrs_ipv4[] = {
  /* 0.0.0.0/8 */
  c_cidr4(0xFF000000u, 0u),
  /* 255.255.255.255/32 */
  c_cidr4(U32_MAX, U32_MAX),
};

static const cidr_t _illegal_cidrs_ipv6[] = {
  /* 0000:0000:0000:0000:0000:0000:0000:0000/128 */
  c_cidr6(U64_MAX, U64_MAX, 0, 0),
  /* ff00:0000:0000:0000:0000:0000:0000:0000/8 */
  c_cidr6(0xFF00000000000000ull, 0, 0xFF00000000000000ull, 0),
};

/*
** Checks if the host name is illegal
** Returns 1 if the ip is illegal
*/
int
naxsi_is_illegal_host_name(const ngx_str_t* host_name)
{
  if (host_name->len < 1) {
    return (0);
  }

  const size_t plen = host_name->len;
  const char*  ptr  = (const char*)host_name->data;

  if (!isalnum(ptr[0])) {
    // RFC says host must start with a num or letter
    return (1);
  }

  for (size_t i = 1; i < plen; ++i) {
    if (!isalnum(ptr[i]) && ptr[i] != '-' && ptr[i] != '.' && ptr[i] != ':') {
      // RFC only allows letters, numbers, dashes, dots and colon in the Host header
      return (1);
    }
  }

  // check if the host name is an ip and if it is in the reserved cidrs
  ip_t ip = ip_zero;
  if (!naxsi_parse_ip(host_name, &ip, NULL)) {
    // ignore if is not an ip.
    return (0);
  }

  const cidr_t* i_cidrs = NULL;
  size_t        n_cidrs = 0;

  if (ip.version == IPv4) {
    i_cidrs = _illegal_cidrs_ipv4;
    n_cidrs = array_size(_illegal_cidrs_ipv4);
  } else {
    i_cidrs = _illegal_cidrs_ipv6;
    n_cidrs = array_size(_illegal_cidrs_ipv6);
  }

  // must check if the ip against illegal ip ranges
  for (size_t i = 0; i < n_cidrs; ++i) {
    const cidr_t* cidr = &i_cidrs[i];
    if (naxsi_is_in_subnet(cidr, &ip)) {
      return (1);
    }
  }

  return (0);
}

/*
** Creates a random request id and writes it into bytes
*/
void
naxsi_generate_request_id(u_char* bytes)
{
#if (NGX_OPENSSL)
  if (RAND_bytes(bytes, NAXSI_REQUEST_ID_SIZE) == 1) {
    return;
  }
#endif
  uint32_t*    bytes32 = (uint32_t*)bytes;
  const size_t len     = (NAXSI_REQUEST_ID_SIZE / sizeof(uint32_t));
  for (size_t i = 0; i < len; i++) {
    bytes32[i] = (uint32_t)ngx_random();
  }
}
