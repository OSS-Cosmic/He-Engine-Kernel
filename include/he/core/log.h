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

#pragma once

#include "he/config.h"
#include "he/core/arch.h"
#include "he/core/filesystem.h"

struct he_str_span;

#ifndef FILENAME_NAME_LENGTH_LOG
#define FILENAME_NAME_LENGTH_LOG 23
#endif

#ifndef INDENTATION_SIZE_LOG
#define INDENTATION_SIZE_LOG 4
#endif

#ifndef LEVELS_LOG
#define LEVELS_LOG 6
#endif

#define CONCAT_STR_LOG_IMPL(a, b) a##b
#define CONCAT_STR_LOG(a, b)      CONCAT_STR_LOG_IMPL(a, b)

#ifndef ANONIMOUS_VARIABLE_LOG
#define ANONIMOUS_VARIABLE_LOG(str) CONCAT_STR_LOG(str, __LINE__)
#endif

// If you add more levels don't forget to change LOG_LEVELS macro to the actual number of levels
enum HeLogLevel_e
{
    eNONE = 0,
    eRAW = 1,
    eDEBUG = 2,
    eINFO = 4,
    eWARNING = 8,
    eERROR = 16,
    eMEMORY_ALLOC = 32,
    eALL = ~0
};

typedef void (*log_callback_fn)(void* user_data, struct he_str_span* message);
typedef void (*log_close_fn)(void* user_data);
typedef void (*log_flush_fn)(void* user_data);

#ifdef HE_TARGET_WINDOWS
#if defined(HE_DEBUG)
#define HE_RAW_ASSERT(x) ((x) == false ? __debugbreak() : false)
#else
#define HE_RAW_ASSERT(x) do {} while (0)
#endif
#elif defined(HE_TARGET_UNIX)
#if defined(HE_DEBUG)
#define HE_RAW_ASSERT(x) assert(x) 
#else
#define HE_RAW_ASSERT(x) do {} while (0)
#endif
#endif

#if defined(HE_DEBUG)

// Checks that an expression if true. Becomes a no-op in release.
#define HE_ASSERTMSG(b, msgFmt, ...)                                                                                \
    do                                                                                                           \
    {                                                                                                            \
        (void)sizeof(b); /* This sizeof must compile, otherwise when disabling ASSERTs the code won't compile */ \
        if (!(b))                                                                                                \
        {                                                                                                        \
            __he_failed_assert(__FILE__, __LINE__, #b, "" msgFmt, ##__VA_ARGS__);                                     \
        }                                                                                                        \
    } while (0)

// Version of ASSERT intended to be used in if statements.
// Expression is evaluated in both debug and release. Assertion only happens in debug.
#define HE_VERIFYMSG(b, msgFmt, ...) ((b) || (__he_failed_assert(__FILE__, __LINE__, #b, "" msgFmt, ##__VA_ARGS__), false))

#else

#define HE_ASSERTMSG(b, msgFmt, ...) \
    do                            \
    {                             \
        ((void)sizeof(b));        \
    } while (0)
#define HE_VERIFYMSG(b, msgFmt, ...) ((b) || (LOGF(eERROR, "VERIFY(" #b ") failed. " msgFmt, ##__VA_ARGS__), false))

#endif

#define HE_ASSERT(b) HE_ASSERTMSG((b), "")
#define HE_VERIFY(b) HE_VERIFYMSG((b), "")
#define HE_ASSERTFAIL(msgFmt, ...) HE_ASSERTMSG(false, msgFmt, ##__VA_ARGS__)

// Usage: LOGF(LogLevel::eINFO | LogLevel::eDEBUG, "Whatever string %s, this is an int %d", "This is a string", 1)
#define HE_LOGF(log_level, ...) he_write_log((log_level), __FILE__, __LINE__, __VA_ARGS__)
// Usage: LOGF_IF(LogLevel::eINFO | LogLevel::eDEBUG, boolean_value && integer_value == 5, "Whatever string %s, this is an int %d", "This is
// a string", 1)
#define HE_LOGF_IF(log_level, condition, ...) \
    ((condition) ? he_write_log((log_level), __FILE__, __LINE__, __VA_ARGS__) : (void)sizeof(condition)) //-V568
//
// #define LOGF_SCOPE(log_level, ...) LogLogScope ANONIMOUS_VARIABLE_LOG(scope_log_){ (log_level), __FILE__, __LINE__, __VA_ARGS__ }

// Usage: RAW_LOGF(LogLevel::eINFO | LogLevel::eDEBUG, "Whatever string %s, this is an int %d", "This is a string", 1)
#define HE_RAW_LOGF(log_level, ...) he_write_raw_log((log_level), false, __VA_ARGS__)
// Usage: RAW_LOGF_IF(LogLevel::eINFO | LogLevel::eDEBUG, boolean_value && integer_value == 5, "Whatever string %s, this is an int %d",
// "This is a string", 1)
#define HE_RAW_LOGF_IF(log_level, condition, ...) \
    ((condition) ? he_write_raw_log((log_level), false, __VA_ARGS__) : (void)sizeof(condition)) //-V568

#if defined(HE_DEBUG)

// Usage: DLOGF(LogLevel::eINFO | LogLevel::eDEBUG, "Whatever string %s, this is an int %d", "This is a string", 1)
#define HE_DLOGF(log_level, ...)                   LOGF(log_level, __VA_ARGS__)
// Usage: DLOGF_IF(LogLevel::eINFO | LogLevel::eDEBUG, boolean_value && integer_value == 5, "Whatever string %s, this is an int %d", "This
// is a string", 1)
#define HE_DLOGF_IF(log_level, condition, ...)     LOGF_IF(log_level, condition, __VA_ARGS__)

// Usage: DRAW_LOGF(LogLevel::eINFO | LogLevel::eDEBUG, "Whatever string %s, this is an int %d", "This is a string", 1)
#define HE_DRAW_LOGF(log_level, ...)               RAW_LOGF(log_level, __VA_ARGS__)
// Usage: DRAW_LOGF_IF(LogLevel::eINFO | LogLevel::eDEBUG, boolean_value && integer_value == 5, "Whatever string %s, this is an int %d",
// "This is a string", 1)
#define HE_DRAW_LOGF_IF(log_level, condition, ...) RAW_LOGF_IF(log_level, condition, __VA_ARGS__)

#else

// Usage: DLOGF(LogLevel::eINFO | LogLevel::eDEBUG, "Whatever string %s, this is an int %d", "This is a string", 1)
#define HE_DLOGF(log_value, ...) \
    do                        \
    {                         \
    } while (0)
// Usage: DLOGF_IF(LogLevel::eINFO | LogLevel::eDEBUG, boolean_value && integer_value == 5, "Whatever string %s, this is an int %d", "This
// is a string", 1)
#define HE_DLOGF_IF(log_value, condition, ...) \
    do                                      \
    {                                       \
        ((void)sizeof(condition));          \
    } while (0)

// Usage: DRAW_LOGF(LogLevel::eINFO | LogLevel::eDEBUG, "Whatever string %s, this is an int %d", "This is a string", 1)
#define HE_DRAW_LOGF(log_level, ...) \
    do                            \
    {                             \
    } while (0)
// Usage: DRAW_LOGF_IF(LogLevel::eINFO | LogLevel::eDEBUG, boolean_value && integer_value == 5, "Whatever string %s, this is an int %d",
// "This is a string", 1)
#define HE_DRAW_LOGF_IF(log_level, condition, ...) \
    do                                          \
    {                                           \
        ((void)sizeof(condition));              \
    } while (0)
#endif

#ifdef __cplusplus
extern "C"
{

#endif
    // Initialization/Exit functions are thread unsafe
    // appName   used to create appName.log. Pass NULL to disable
    // level     mask of LogLevel bits. Log is ignored if its level is missing in mask. Use eALL to enable full log
    void he_init_log(const char* appName, enum HeLogLevel_e level);
    void he_exit_log(void);

    void he_add_log_file(const char* filename, enum he_file_mode file_mode, enum HeLogLevel_e log_level);
    void he_add_log_callback(uint32_t log_level, void* user_data, log_callback_fn callback, log_close_fn close,
                                  log_flush_fn flush);

    void he_write_log_va_list(uint32_t level, const char* filename, int line_number, const char* message, va_list args);
    void he_write_log(uint32_t level, const char* filename, int line_number, const char* message, ...);
    void he_write_raw_log(uint32_t level, bool error, const char* message, ...);
    void __he_failed_assert(const char* file, int line, const char* statement, const char* msg, ...);

#ifdef __cplusplus
} // extern "C"
#endif

