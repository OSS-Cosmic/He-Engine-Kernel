/*
 * This source file is part of the bstring string library.  This code was
 * written by Paul Hsieh in 2002-2015, and is covered by the BSD open source
 * license and the GPL. Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef _QSTR_UTF8_H_
#define _QSTR_UTF8_H_ 1

#include "he/core/str.h"

#define HE_IS_LEGAL_UNICODE_POINT(v)  ((((v) < 0xD800L) || ((v) > 0xDFFFL)) && (((unsigned long)(v)) <= 0x0010FFFFL) && (((v)|0x1F0001) != 0x1FFFFFL))

struct he_str_utf_iterable {
  const struct he_str_span buffer; // the buffer to iterrate over
  size_t cursor;
};

struct he_str_utf_result {
  uint32_t codePoint;
  uint8_t invalid: 1;
  uint8_t finished: 1; // marks the final character in the sequence
};

#ifdef __cplusplus
extern "C" {
#endif

struct he_str_utf_result str_utf8_next_code_point(struct he_str_utf_iterable* iter); 

// https://datatracker.ietf.org/doc/html/rfc2781
struct he_str_utf_result str_utf16_next_code_point(struct he_str_utf_iterable* iter);
#ifdef __cplusplus
}
#endif


#endif
