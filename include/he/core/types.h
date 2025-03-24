/*
* Copyright (C) 1997-2001 Id Software, Inc.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* 
* See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/

/*
 * Copyright (c) 2017-2024 The Forge Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _Q_TYPES_H
#define _Q_TYPES_H

#include <stdint.h>
#include <assert.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#ifndef _MSC_VER
#include <strings.h>
#endif
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
#include <type_traits>
#endif

#if (defined(_WINDOWS_) || defined(_WINDOWS_H))
  #define HE_TARGET_WINDOWS
  #define THREAD_LOCAL __declspec(thread)
#elif defined(__APPLE__)
  #define HE_TARGET_APPLE
  #define THREAD_LOCAL __thread
#else
  #define HE_TARGET_UNIX
#endif

#define KB_TO_BYTE (1024)
#define MB_TO_BYTE (1024 * KB_TO_BYTE)
#define GB_TO_BYTE (1024 * MB_TO_BYTE)

#if INTPTR_MAX == 0x7FFFFFFFFFFFFFFFLL
  #define HE_PTR_SIZE 8
#elif INTPTR_MAX == 0x7FFFFFFF
  #define HE_PTR_SIZE 4
#else
  #error unsupported platform
#endif

#define HE_ALIGN_TO(size, alignment) (((size) + (alignment)-1) & ~((alignment)-1))
#define HE_ARRAY_COUNT(array) (sizeof(array) / (sizeof(array[0]) * (sizeof(array) != HE_PTR_SIZE || sizeof(array[0]) <= HE_PTR_SIZE)))
#define HE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define HE_MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef __cplusplus
#define HE_CONSTEXPR constexpr
#define HE_EXTERN_C  extern "C"
#else
#define HE_CONSTEXPR
#define HE_EXTERN_C
#endif

#ifdef __cplusplus
#define Q_ENUM_FLAG(TYPE, ENUM_TYPE)                                                                                      \
    inline HE_CONSTEXPR ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) { return ENUM_TYPE(((TYPE)a) | ((TYPE)b)); }        \
    inline ENUM_TYPE&                operator|=(ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)(((TYPE&)a) |= ((TYPE)b)); } \
    inline HE_CONSTEXPR ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) { return ENUM_TYPE(((TYPE)a) & ((TYPE)b)); }        \
    inline ENUM_TYPE&                operator&=(ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)(((TYPE&)a) &= ((TYPE)b)); } \
    inline HE_CONSTEXPR ENUM_TYPE operator~(ENUM_TYPE a) { return ENUM_TYPE(~((TYPE)a)); }                                \
    inline HE_CONSTEXPR ENUM_TYPE operator^(ENUM_TYPE a, ENUM_TYPE b) { return ENUM_TYPE(((TYPE)a) ^ ((TYPE)b)); }        \
    inline ENUM_TYPE&                operator^=(ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)(((TYPE&)a) ^= ((TYPE)b)); }
#else
#define Q_ENUM_FLAG(TYPE, ENUM_TYPE)
#endif

#if defined(_MSC_VER)

#define HE_COMPILE_ASSERT(exp)
#define HE_COMPILE_ASSERT_MSG(exp, msg)

#else

#if !defined(__cplusplus)
#define HE_COMPILE_ASSERT(exp) _Static_assert(exp, #exp)
#else
#define HE_COMPILE_ASSERT(exp) static_assert(exp, #exp)
#endif

#if !defined(__cplusplus)
#define HE_COMPILE_ASSERT_MSG(exp, msg) _Static_assert(exp, msg)
#else
#define HE_COMPILE_ASSERT_MSG(exp, msg) static_assert(exp, msg)
#endif

#endif

#define Q_SAME_TYPE(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#ifdef __GNUC__
#define Q_MEMBER_TYPE(type, member) __typeof__ (((type *)0)->member)
#else
#define Q_MEMBER_TYPE(type, member) const void
#endif

#ifdef __GNUC__
#define HE_CONTAINER_OF(ptr, type, member) ({				\
	void *__mptr = (void *)(ptr);					\
	HE_COMPILE_ASSERT_MSG(Q_SAME_TYPE(*(ptr), ((type *)0)->member) ||	\
		      Q_SAME_TYPE(*(ptr), void),			\
		      "pointer type mismatch in container_of()");	\
	((type *)(__mptr - offsetof(type, member))); })
#else
#define HE_CONTAINER_OF(ptr, type, member) ((type *)((char *)(Q_MEMBER_TYPE(type, member) *){ ptr } - offsetof(type, member)))
#endif




#if defined(_MSC_VER)
#define HE_EXPORT __declspec(dllexport)
#define HE_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) // clang & gcc
#define HE_EXPORT __attribute__((visibility("default")))
#define HE_IMPORT
#endif

#endif

