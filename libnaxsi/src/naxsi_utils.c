// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include "naxsi_utils.h"
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * @brief      Search for the first occurrence of the given char (case sensitive)
 *
 * @param[in]  string      The input string
 * @param[in]  ch          char to search
 * @param[in]  string_len  The string length
 *
 * @return     Pointer to the found char on the input string
 */
const char *naxsi_strnchr(const char *string, int ch, size_t string_len) {
	for (size_t i = 0; i < string_len && string[i]; i++) {
		if (string[i] == ch) {
			return string + i;
		}
	}
	return NULL;
}

/**
 * @brief      Search for the first occurrence of the given char (case insensitive)
 *
 * @param[in]  string      The input string
 * @param[in]  ch          char to search
 * @param[in]  string_len  The string length
 *
 * @return     Pointer to the found char on the input string
 */
static const char *strncasechr(const char *string, int ch, size_t string_len) {
	for (size_t i = 0; i < string_len && string[i]; i++) {
		if (tolower(string[i]) == ch) {
			return string + i;
		}
	}
	return NULL;
}

/**
 * @brief      Checks if a string contains a certain substring
 *
 * @param[in]  string         The input string
 * @param[in]  string_len     The string length
 * @param[in]  substring      The substring to search
 * @param[in]  substring_len  The substring length
 *
 * @return     Pointer to the matched sequence or NULL if not found
 */
const char *naxsi_strstr_offset(naxsi_str_t *string, size_t offset, naxsi_str_t *substring) {
	if (!string || !substring || string->length < 1 || substring->length < 1 || string->length < substring->length) {
		return NULL;
	} else if (substring->length == 1) {
		return strncasechr(string->data, substring->data[0], string->length);
	}
	for (size_t i = offset; i < string->length; ++i) {
		size_t leftover = string->length - i;
		const char *found = strncasechr(string->data + i, substring->data[0], leftover);
		if (!found) {
			return NULL;
		} else if (!strncasecmp(found + 1, substring->data + 1, substring->length - 1)) {
			return found;
		}
	}
	return NULL;
}

/**
 * @brief      Converts all '\0' chars to '0' contained in a naxsi_str_t
 *
 * @param      string  The string to escape
 *
 * @return     true if escapes any null bytes, otherwise false
 */
bool naxsi_escape_nullbytes(naxsi_str_t *string) {
	bool found_null_byte = false;
	for (size_t i = 0; i < string->length; i++) {
		if (string->data[i] == 0) {
			string->data[i] = '0';
			found_null_byte = true;
		}
	}
	return found_null_byte;
}

/**
 * @brief      Checks if naxsi_str_t contains a valid utf8 sequence
 * Source: https://www.cl.cam.ac.uk/~mgk25/ucs/utf8_check.c
 *
 * @param      string  Pointer to naxsi_str_t containing string to test
 *
 * @return     true when the string is a valid utf8 sequence, otherwise false
 */
bool naxsi_is_invalid_utf8(const naxsi_str_t *string) {
	size_t offset = 0;
	const unsigned char *s = (const unsigned char *)string->data;
	size_t length = string->length;
	while (offset < length && *s) {
		if (*s < 0x80) {
			/* 0xxxxxxx */
			s++;
			offset++;
		} else if ((s[0] & 0xe0) == 0xc0) {
			if (offset + 1 >= length) {
				// not enough bytes
				return true;
			}
			/* 110XXXXx 10xxxxxx */
			if ((s[1] & 0xc0) != 0x80 ||
				(s[0] & 0xfe) == 0xc0) { /* overlong? */
				return true;
			} else {
				s += 2;
				offset += 2;
			}
		} else if ((s[0] & 0xf0) == 0xe0) {
			if (offset + 2 >= length) {
				// not enough bytes
				return true;
			}
			/* 1110XXXX 10Xxxxxx 10xxxxxx */
			if ((s[1] & 0xc0) != 0x80 ||
				(s[2] & 0xc0) != 0x80 ||
				(s[0] == 0xe0 && (s[1] & 0xe0) == 0x80) || /* overlong? */
				(s[0] == 0xed && (s[1] & 0xe0) == 0xa0) || /* surrogate? */
				(s[0] == 0xef && s[1] == 0xbf &&
					(s[2] & 0xfe) == 0xbe)) { /* U+FFFE or U+FFFF? */
				return true;
			} else {
				s += 3;
			}
		} else if ((s[0] & 0xf8) == 0xf0) {
			if (offset + 3 >= length) {
				// not enough bytes
				return true;
			}
			/* 11110XXX 10XXxxxx 10xxxxxx 10xxxxxx */
			if ((s[1] & 0xc0) != 0x80 ||
				(s[2] & 0xc0) != 0x80 ||
				(s[3] & 0xc0) != 0x80 ||
				(s[0] == 0xf0 && (s[1] & 0xf0) == 0x80) || /* overlong? */
				(s[0] == 0xf4 && s[1] > 0x8f) || s[0] > 0xf4) { /* > U+10FFFF? */
				return true;
			} else {
				s += 4;
			}
		} else {
			return true;
		}
	}
	return false;
}