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

struct HeStrUTFIterable_s {
  const struct HeStrSpan buffer; // the buffer to iterrate over
  size_t cursor;
};


struct HeStrUtfResult_s {
  uint32_t codePoint;
  uint8_t invalid: 1;
  uint8_t finished: 1; // marks the final character in the sequence
};

#ifdef __cplusplus
extern "C" {
#endif

struct HeStrUtfResult_s HeStrUtf8NextCodePoint(struct HeStrUTFIterable_s* iter); 

// https://datatracker.ietf.org/doc/html/rfc2781
struct HeStrUtfResult_s HeStrUtf16NextCodePoint(struct HeStrUTFIterable_s* iter);
#ifdef __cplusplus
}
#endif


#endif
