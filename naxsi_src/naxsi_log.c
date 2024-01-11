// SPDX-FileCopyrightText: 2024 Giovanni Dante Grazioli <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include <naxsi.h>
#include <naxsi_macros.h>

// fail if end of buffer
#define fail_if_eob(buf, need, end, val)                                                           \
  if ((buf + need) >= end)                                                                         \
  return val

const char* hex_dict = "0123456789abcdef";

// writes 6 bytes
inline char*
json_encode_as_code(char ch, char* buffer)
{
  *buffer++ = '\\';
  *buffer++ = 'u';
  *buffer++ = '0';
  *buffer++ = '0';
  *buffer++ = hex_dict[ch >> 4 & 0xf];
  *buffer++ = hex_dict[ch & 0xf];
  return buffer;
}

inline char*
json_encode_char(const char ch, char* curr, const char* end)
{
  switch (ch) {
    case '\n':
      fail_if_eob(curr, 2, end, NULL);
      *curr++ = '\\';
      *curr++ = 'n';
      return curr;
    case '\r':
      fail_if_eob(curr, 2, end, NULL);
      *curr++ = '\\';
      *curr++ = 'r';
      return curr;
    case '\\':
      fail_if_eob(curr, 2, end, NULL);
      *curr++ = '\\';
      *curr++ = '\\';
      return curr;
    case '\t':
      fail_if_eob(curr, 2, end, NULL);
      *curr++ = '\\';
      *curr++ = 't';
      return curr;
    case '"':
      fail_if_eob(curr, 2, end, NULL);
      *curr++ = '\\';
      *curr++ = '"';
      return curr;
    case '\f':
      fail_if_eob(curr, 2, end, NULL);
      *curr++ = '\\';
      *curr++ = 'f';
      return curr;
    case '\b':
      fail_if_eob(curr, 2, end, NULL);
      *curr++ = '\\';
      *curr++ = 'b';
      return curr;
    default:
      // just a char.
      break;
  }

  if (is_printable(ch)) {
    fail_if_eob(curr, 1, end, NULL);
    *curr++ = ch;
    return curr;
  }

  fail_if_eob(curr, 6, end, NULL);
  return json_encode_as_code(ch, curr);
}

// encodes url-encoded strings into json structures
int
naxsi_json_string_encode(const char* str, size_t str_len, char* buffer, const size_t buffer_size)
{
  if ((str_len + 2) > buffer_size) {
    // we always consume 2 bytes to produce "".
    return 0;
  }

  char* curr = buffer;
  char* end  = (curr + buffer_size) - 2;

  *curr++ = '"';

  size_t j;
  for (j = 0; curr < end; j++) {
    if (!(curr = json_encode_char(str[j], curr, end))) {
      return 0;
    }
  }

  if ((curr + 2) >= end) {
    return 0;
  }

  *curr++ = '"';
  *curr++ = 0;
  return 1;
}

// encodes url-encoded strings into json structures
int
naxsi_json_log(const ngx_str_t* elem, char* buffer, const size_t size)
{
  if (elem->len < 1 || (elem->len + 4) > size) {
    // don't attempt to create a json string
    // we always consume 4 bytes to produce `{"xx"}`.
    return 0;
  }

  // line only
  const char* line = (const char*)elem->data;
  char*       curr = buffer;
  char*       end  = (curr + size) - 4;

  *curr++ = '{';
  *curr++ = '"';

  size_t j;
  char   ch;
  int    equals = 1;
  for (j = 0; (ch = line[j]) && curr < end; j++) {
    if (equals && ch == '=') {
      // this is the char that splits variable from value
      // example: ?foo=bar
      fail_if_eob(curr, 3, end, 0);
      *curr++ = '"';
      *curr++ = ':';
      *curr++ = '"';
      equals  = 0;
      continue;
    } else if (!equals && ch == '&') {
      // this is the char that splits previous value to new variable
      // example: &foo=bar
      fail_if_eob(curr, 3, end, 0);
      *curr++ = '"';
      *curr++ = ':';
      *curr++ = '"';
      equals  = 1;
      continue;
    }
    if (!(curr = json_encode_char(ch, curr, end))) {
      return 0;
    }
  }

  if ((curr + 3) >= end) {
    return 0;
  }

  *curr++ = '"';
  *curr++ = '}';
  *curr++ = 0;
  return 1;
}
