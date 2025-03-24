/* Bstr 2.0 -- A C dynamic strings library
 *
 * Copyright (c) 2006-2015, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2015, Oran Agra
 * Copyright (c) 2015, Redis Labs, Inc
 * Paul Hsieh in 2002-2015
 * Michael Pollind 2024-* <mpollind at gmail dot com>
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// this is a modified string library that implements features from both bstring and sds


#ifndef Q_STRING_H_INCLUDED
#define Q_STRING_H_INCLUDED

#include "he/core/types.h"

#define STR_LLSTR_SIZE 21
#define STR_LSTR_SIZE 16 

struct HeStr {
  size_t alloc;
  size_t len;
  char* buf;
};

struct HeStrSpan {
  char * buf;
  size_t len;
}; 


static inline struct HeStrSpan HeCToStrRef(const char* c) { 
  struct HeStrSpan result;
  result.buf = (char*)c;
  result.len = c ? (size_t)strlen(c) : 0;
  return result; 
}

static inline struct HeStrSpan HeToStrRef(struct HeStr str) { 
  struct HeStrSpan result;
  result.buf = (char*)str.buf;
  result.len = str.len;
  return result; 
}

static inline struct HeStrSpan HeSubStrSpan(struct HeStrSpan slice, size_t a, size_t b) {
    assert((b - a) <= slice.len);
    struct HeStrSpan result;
    result.buf = slice.buf + a;
    result.len = b - a;
    return result;
}

static inline size_t qStrAvailLen(struct HeStr str) { return str.alloc - str.len;}
static inline struct HeStrSpan qStrAvailSpan(struct HeStr str) { 
  struct HeStrSpan result;
  result.buf = str.buf + str.len;
  result.len = qStrAvailLen(str);
  return result;
}

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a string from a slice 
 **/
void qStrFree(struct HeStr* str);
void qStrUpper(struct HeStrSpan slice);
void qStrLower(struct HeStrSpan slice);
#define qStrEmpty(b) ((b).buf == NULL || (b).len == 0)

struct HeStrSpan qStrTrim(struct HeStrSpan slice);
struct HeStrSpan qStrRTrim(struct HeStrSpan  slice);
struct HeStrSpan qStrLTrim(struct HeStrSpan  slice);

/* Enlarge the free space at the end of the Str string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 *
 * Note: this does not change the *length* of the Str string as len 
 * but only the free buffer space we have. */
bool qStrMakeRoomFor(struct HeStr* str, size_t addlen);
/* 
 * set the length of the buffer to the length specified. this
 * will also trigger a realloction if the length is greater then the size
 * reserved. 
 *
 * Note: this does not set the null terminator for the string.
 * this will corrupt slices that are referencing a slice out of this buffer.
 **/
bool qStrSetLen(struct HeStr* str, size_t len);
/**
 * set the amount of memory reserved by the Str. will only ever increase
 * the size of the string 
 * 
 * A reserved string can be assigned with bstrAssign
 **/
bool qStrSetResv(struct HeStr* str, size_t reserveLen); 

/** 
 * Modify an Str string in-place to make it empty (zero length) set null terminator.
 * However all the existing buffer is not discarded but set as free space
 * so that next append operations will not require allocations up to the
 * number of bytes previously available. 
 **/
bool qStrClear(struct HeStr* str);

/**
 * takes a Str and duplicates the underlying buffer.
 *
 * the buffer is trimmed down to the length of the string.
 *
 * if the buffer fails to allocate then BSTR_IS_EMPTY(b) will be true
 **/
struct HeStr qStrDup(const struct HeStr* str);
bool qStrAppendSlice(struct HeStr* str, const struct HeStrSpan slice);
bool qStrAppendChar(struct HeStr* str, char b);
bool qStrInsertChar(struct HeStr* str, size_t i, char b);
bool qStrInsertSlice(struct HeStr* str, size_t i, const struct HeStrSpan slice);
bool qStrAssign(struct HeStr* str, struct HeStrSpan slice);
/**
 * resizes the allocation of the Str will truncate if the allocation is less then the size 
 *
 * the buffer is trimmed down to the length of the string.
 *
 * If the buffer fails to reallocate then false is returned
 **/
bool qStrResize(struct HeStr* str, size_t len);


struct qStrSplitIterable {
  const struct HeStrSpan buffer; // the buffer to iterrate over
  const struct HeStrSpan delim; // delim to split across 
  size_t cursor; // the current position in the buffer
};
//#define bstr_iter_has_more(it) (it.cursor < it.buffer.len)

/** 
 * splits a string using an iterator and returns a slice. a valid slice means there are 
 * are more slices.
 *
 * The the slice does not have a null terminator.
 *
 * struct bstr_split_iterator_s iterable = {qstr
 *     .delim = bstr_ref(" "),
 *     .buffer = bstr_ref("one two three four five"),
 *     .cursor = 0
 * };
 * for(struct bstr_slice_s it = bstrSplitIterate(&iterable); 
 *  bstr_iter_has_more(it); 
 *  it = bstrSplitIterate(&iterable)) {
 *   printf("Next substring: %.*s\n", slice.len, slice.buf); 
 * }
 *
 **/
struct HeStrSpan qStrSplitIter(struct qStrSplitIterable*);

/** 
 * splits a string using an iterator and returns a slice. a valid slice means there are 
 * are more slices.
 *
 * For the reverse case move the cursor to the end of the string
 *
 * The the slice does not have a null terminator.
 *
 * struct bstr_split_iterator_s iterable = {
 *     .delim = bstr_ref(" "),
 *     .buffer = bstr_ref("one two three four five"),
 *     .cursor = 0
 * };
 * for(struct bstr_slice_s it = bstrSplitIterate(&iterable); 
 *  !bstr_is_empty(it); 
 *  it = bstrSplitIterate(&iterable)) {
 *   printf("Next substring: %.*s\n", slice.len, slice.buf); 
 * }
 *
 **/
struct HeStrSpan qStrSplitRevIter(struct qStrSplitIterable*);

/* Set the Str string length to the length as obtained with strlen(), so
 * considering as content only up to the first null term character.
 *
 * This function is useful when the Str string has been changed where
 * the length is not correctly updated. using vsprintf for instance.
 *
 * After the call, slices are not valid if they reference this Str 
 * 
 * s = tfEmpty();
 * s[2] = '\0';
 * tfUpdateLen(s);
 * printf("%d\n", s.len);
 *
 * The output will be "2", but if we comment out the call to tfUpdateLen()
 * the output will be "6" as the string was modified but the logical length
 * remains 6 bytes. 
 ** */
bool qStrUpdateLen(struct HeStr* str);
/**
 * By default, the Str string is not null terminated. This function will set the null terminator
 * and ensure that enough space is reserved for the null terminator.
 */
void qStrSetNullTerm(struct HeStr* str);

/* Append to the Str string 's' a string obtained using printf-alike format
 * specifier.
 *
 * After the call, the modified Str string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = tfCreate("Sum is: ");
 * tfcatprintf(s,"%d+%d = %d",a,b,a+b)
 *
 * if valid true else false
 */
bool qstrcatprintf(struct HeStr* s, const char *fmt, ...); 
bool qstrcatvprintf(struct HeStr* str, const char* fmt, va_list ap);

int qstrsscanf(struct HeStrSpan slice, const char* fmt, ...);
int qstrvsscanf(struct HeStrSpan slice, const char* fmt, va_list ap);

/* This function is similar to tfcatprintf, but much faster as it does
 * not rely on sprintf() family functions implemented by the libc that
 * are often very slow. Moreover directly handling the Str as
 * new data is concatenated provides a performance improvement.
 *
 * However this function only handles an incompatible subset of printf-alike
 * format specifiers:
 *
 * %c - char
 * %s - C String
 * %S - struct tf_slice_s slice 
 * %i - signed int
 * %l - signed long
 * %I - 64 bit signed integer (long long, int64_t)
 * %u - unsigned int
 * %L - unsigned long
 * %U - 64 bit unsigned integer (unsigned long long, uint64_t)
 * %% - Verbatim "%" character.
 */
bool qstrcatfmt(struct HeStr*, char const *fmt, ...);

/*
 * join an array of slices and cat them to bstr. faster since the lengths are known ahead of time.
 * the buffer can be pre-reserved upfront.
 *
 * this modifies Str so slices that reference this Str can become invalid.
 **/
bool qstrcatjoin(struct HeStr*, struct HeStrSpan* slices, size_t numSlices, struct HeStrSpan sep);
/*
 * join an array of strings and cat them to Str 
 **/
bool qstrcatjoinCStr(struct HeStr*, const char** argv, size_t argc, struct HeStrSpan sep);

/**
 * this should fit safetly within BSTR_LLSTR_SIZE. 
 *
 * the number of bytes written to the slice is returned else -1 if the 
 * value is unable to be written or the length of the slice is greater
 *
 **/
int qstrfmtll(struct HeStrSpan slice, long long value); 
int qstrfmtull(struct HeStrSpan slice, unsigned long long value); 

/*  
 * Parse a string into a 64-bit integer.
 *
 * when base is 0, the function will try to determine the base from the string
 *   * if the string starts with 0x, the base will be 16
 *   * if the string starts with 0b, the base will be 2
 *   * if the string starts with 0o, the base will be 8
 *   * otherwise the base will be 10
 */
bool qStrReadll(struct HeStrSpan, long long* result);
bool qStrReadull(struct HeStrSpan, unsigned long long* result);

bool qStrReadFloat(struct HeStrSpan, float* result);
bool qStrReadDouble(struct HeStrSpan, double* result);
/* Scan/search functions */
/*  
 *  Compare two strings without differentiating between case. The return
 *  value is the difference of the values of the characters where the two
 *  strings first differ after lower case transformation, otherwise 0 is
 *  returned indicating that the strings are equal. If the lengths are
 *  different, if the first slice is longer 1 else -1. 
 */
int qStrCaselessCompare (const struct HeStrSpan b0, const struct HeStrSpan b1);
/*
 *  The return value is the difference of the values of the characters where the
 *  two strings first differ after lower case transformation, otherwise 0 is
 *  returned indicating that the strings are equal. If the lengths are
 *  different, if the first slice is longer 1 else -1.
 */
int qStrCompare  (const struct HeStrSpan b0, const struct HeStrSpan b1);
/**
*  Test if two strings are equal ignores case true else false.  
**/
bool qStrCaselessEqual (const struct HeStrSpan b0, const struct HeStrSpan b1);
/**
*  Test if two strings are equal return true else false.  
**/
bool qStrEqual (const struct HeStrSpan b0, const struct HeStrSpan b1);

int qStrIndexOfOffset(const struct HeStrSpan haystack, size_t offset, const struct HeStrSpan needle);
int qStrIndexOf(const struct HeStrSpan haystack, const struct HeStrSpan needle);
int qStrLastIndexOfOffset(const struct HeStrSpan str, size_t offset, const struct HeStrSpan needle);
int qStrLastIndexOf(const struct HeStrSpan str, const struct HeStrSpan needle);

int qStrIndexOfCaselessOffset(const struct HeStrSpan haystack, size_t offset, const struct HeStrSpan needle);
int qStrIndexOfCaseless(const struct HeStrSpan haystack, const struct HeStrSpan needle);
int qStrLastIndexOfCaseless(const struct HeStrSpan haystack, const struct HeStrSpan needle);
int qStrLastIndexOfCaselessOffset(const struct HeStrSpan haystack, size_t offset, const struct HeStrSpan needle);

int qStrIndexOfAny(const struct HeStrSpan haystack, const struct HeStrSpan characters);
int qStrLastIndexOfAny(const struct HeStrSpan haystack, const struct HeStrSpan characters);

int qPrettyPrintBytes(struct HeStrSpan slice, size_t numBytes);
int qPrettyPrintDuration(struct HeStrSpan slice,double nanoseconds);

#ifdef __cplusplus
}
#endif


#endif

