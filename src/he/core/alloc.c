// ---------------------------------------------------------------------------------------------------------------------------------
//
//
//  _ __ ___  _ __ ___   __ _ _ __      ___ _ __  _ ___
// | '_ ` _ \| '_ ` _ \ / _` | '__|    / __| '_ \| '_  |
// | | | | | | | | | | | (_| | |    _ | (__| |_) | |_) |
// |_| |_| |_|_| |_| |_|\__, |_|   (_) \___| .__/| .__/
//                       __/ |             | |   | |
//                      |___/              |_|   |_|
//
// Memory manager & tracking software
//
// Best viewed with 8-character tabs and (at least) 132 columns
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//  * This software is 100% free
//  * If you use this software (in part or in whole) you must credit the author.
//  * This software may not be re-distributed (in part or in whole) in a modified
//    form without clear documentation on how to obtain a copy of the original work.
//  * You may not use this software to directly or indirectly cause harm to others.
//  * This software is provided as-is and without warrantee. Use at your own risk.
//
// For more information, visit HTTP://www.FluidStudios.com
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/22/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
//
// !!IMPORTANT!!
//
// This software is self-documented with periodic comments. Before you start using this software, perform a search for the string
// "-DOC-" to locate pertinent information about how to use this software.
//
// You are also encouraged to read the comment blocks throughout this source file. They will help you understand how this memory
// tracking software works, so you can better utilize it within your applications.
//
// NOTES:
//
// 1. If you get compiler errors having to do with set_new_handler, then go through this source and search/replace
//    "std::set_new_handler" with "set_new_handler".
//
// 2. This code purposely uses no external routines that allocate RAM (other than the raw allocation routines, such as malloc). We
//    do this because we want this to be as self-contained as possible. As an example, we don't use assert, because when running
//    under WIN32, the assert brings up a dialog box, which allocates RAM. Doing this in the middle of an allocation would be bad.
//
// 3. When trying to override new/delete under MFC (which has its own version of global new/delete) the linker will complain. In
//    order to fix this error, use the compiler option: /FORCE, which will force it to build an executable even with linker errors.
//    Be sure to check those errors each time you compile, otherwise, you may miss a valid linker error.
//
// 4. If you see something that looks odd to you or seems like a strange way of going about doing something, then consider that this
//    code was carefully thought out. If something looks odd, then just assume I've got a good reason for doing it that way (an
//    example is the use of the class MemStaticTimeTracker.)
//
// 5. With MFC applications, you will need to comment out any occurance of "#define new DEBUG_NEW" from all source files.
//
// 6. Include file dependencies are _very_important_ for getting the MMGR to integrate nicely into your application. Be careful if
//    you're including standard includes from within your own project includes; that will break this very specific dependency order.
//    It should look like this:
//
//		#include <stdio.h>   // Standard includes MUST come first
//		#include <stdlib.h>  //
//		#include <streamio>  //
//
//		#include "mmgr.h"    // mmgr.h MUST come next
//
//		#include "myfile1.h" // Project includes MUST come last
//		#include "myfile2.h" //
//		#include "myfile3.h" //
//
// ---------------------------------------------------------------------------------------------------------------------------------

// #include "stdafx.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "he/core/thread.h"
#include "he/core/alloc.h"
#include "he/core/str.h"
#include "he/core/filesystem.h"
#include "he/core/log.h"

#if !defined(WIN32) && !defined(XBOX)
#include <unistd.h>
#endif

static unsigned int calc_unused_alloc(const sAllocUnit* allocUnit);

#ifdef HE_ALLOC_BACKTRACE
#ifdef HE_TARGET_WINDOWS
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <execinfo.h>
#endif
#endif

#define HE_BACKTRACE_SIZE (2048)

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- If you're like me, it's hard to gain trust in foreign code. This memory manager will try to INDUCE your code to crash (for
// very good reasons... like making bugs obvious as early as possible.) Some people may be inclined to remove this memory tracking
// software if it causes crashes that didn't exist previously. In reality, these new crashes are the BEST reason for using this
// software!
//
// Whether this software causes your application to crash, or if it reports errors, you need to be able to TRUST this software. To
// this end, you are given some very simple debugging tools.
//
// The quickest way to locate problems is to enable the STRESS_TEST macro (below.) This should catch 95% of the crashes before they
// occur by validating every allocation each time this memory manager performs an allocation function. If that doesn't work, keep
// reading...
//
// If you enable the TEST_MEMORY_MANAGER #define (below), this memory manager will log an entry in the memory.log file each time it
// enters and exits one of its primary allocation handling routines. Each call that succeeds should place an "ENTER" and an "EXIT"
// into the log. If the program crashes within the memory manager, it will log an "ENTER", but not an "EXIT". The log will also
// report the name of the routine.
//
// Just because this memory manager crashes does not mean that there is a bug here! First, an application could inadvertantly damage
// the heap, causing malloc(), realloc() or free() to crash. Also, an application could inadvertantly damage some of the memory used
// by this memory tracking software, causing it to crash in much the same way that a damaged heap would affect the standard
// allocation routines.
//
// In the event of a crash within this code, the first thing you'll want to do is to locate the actual line of code that is
// crashing. You can do this by adding log() entries throughout the routine that crashes, repeating this process until you narrow
// in on the offending line of code. If the crash happens in a standard C allocation routine (i.e. malloc, realloc or free) don't
// bother contacting me, your application has damaged the heap. You can help find the culprit in your code by enabling the
// STRESS_TEST macro (below.)
//
// If you truely suspect a bug in this memory manager (and you had better be sure about it! :) you can contact me at
// midnight@FluidStudios.com. Before you do, however, check for a newer version at:
//
//	http://www.FluidStudios.com/publications.html
//
// When using this debugging aid, make sure that you are NOT setting the alwaysLogAll variable on, otherwise the log could be
// cluttered and hard to read.
// ---------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//
//This file contains a log of all memory operations performed during the last run.
//
//Interrogate this file to track errors or to help track down memory-related
//issues. You can do this by tracing the allocations performed by a specific owner
//or by tracking a specific address through a series of allocations and
//reallocations.
//");
//There is a lot of useful information here which, when used creatively, can be
//extremely helpful.
//
//Note that the following guides are used throughout this file:
//
//   [!] - Error
//   [+] - Allocation
//   [~] - Reallocation
//   [-] - Deallocation
//   [I] - Generic information
//   [F] - Failure induced for the purpose of stress-testing your application
//   [D] - Information used for debugging this memory manager
//
//...so, to find all errors in the file, search for \!]
//
//--------------------------------------------------------------------------------");

static bool he_validate_alloc_unit(const sAllocUnit* allocUnit);
static void he_dump_alloc_unit(const sAllocUnit* allocUnit, const char* prefix);


#if defined(HE_ALLOC_ASAN)
#include <sanitizer/asan_interface.h>
#define ASAN_POISON(addr, size)   __asan_poison_memory_region(addr, size)
#define ASAN_UNPOISON(addr, size) __asan_unpoison_memory_region(addr, size)
#else
#define ASAN_POISON(...)
#define ASAN_UNPOISON(...)
#endif

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Get to know these values. They represent the values that will be used to fill unused and deallocated RAM.
// ---------------------------------------------------------------------------------------------------------------------------------

const static unsigned int prefixPattern = 0xbaadf00d;   // Fill pattern for bytes preceeding allocated blocks
const static unsigned int postfixPattern = 0xdeadc0de;  // Fill pattern for bytes following allocated blocks
const static unsigned int unusedPattern = 0xfeedface;   // Fill pattern for freshly allocated blocks
const static unsigned int releasedPattern = 0xdeadbeef; // Fill pattern for deallocated blocks

// ---------------------------------------------------------------------------------------------------------------------------------
// Other locals
// ---------------------------------------------------------------------------------------------------------------------------------

#define hashSize (1u << HE_ALLOC_HASH_BITS)
static sAllocUnit*  hashTable[hashSize];
static sAllocUnit*  reservoir;
static unsigned int currentAllocationCount = 0;
static unsigned int breakOnAllocationCount = 0;
static sMStats      stats;
static const char*  sourceFile = "??";
static const char*  sourceFunc = "??";
static unsigned int sourceLine = 0;
static sAllocUnit** reservoirBuffer = NULL;
static unsigned int reservoirBufferSize = 0;
static struct he_mutex alloc_mutex;

#ifdef HE_ALLOC_BACKTRACE
static int stackSkipCount = 0;
#ifdef _WIN32
// A unique handle to use for backtrace symbols, must be closed before exit
static HANDLE gProcessHandle;
#endif
#endif

void he_init_alloc() {
  he_init_mutex(&DefaultHeMutexDesc, &alloc_mutex);
#ifdef HE_ALLOC_BACKTRACE
#ifdef _WIN32
    // SymInitialize specifies that is should never be used with GetCurrentProcess and the handle should
    // be unique to avoid issues with sharing. A few libraries do it anyway though and it seems to work fine.
    // However avoid any undefined behaviour we create a real handle from GetCurrentProcess that we can
    // later use with SymInitialize.
    HANDLE currentProcess = GetCurrentProcess();
    DuplicateHandle(currentProcess, currentProcess, currentProcess, &gProcessHandle, 0, true, DUPLICATE_SAME_ACCESS);
#endif
#endif

} 


// ---------------------------------------------------------------------------------------------------------------------------------
// Local functions only
// ---------------------------------------------------------------------------------------------------------------------------------

#define BUFFER_SIZE 2048

typedef struct tm tm;

static const struct he_str_span __source_file_stripper(const struct he_str_span souce_file_span)
{
    int idx = str_last_index_of(souce_file_span, c_to_str_span("\\"));
    if (idx > 0)
        return sub_str_span(souce_file_span, idx + 1, souce_file_span.len);
    idx = str_last_index_of(souce_file_span, c_to_str_span("\\"));
    if (idx > 0)
        return sub_str_span(souce_file_span, idx + 1, souce_file_span.len);
    return souce_file_span;
}

static size_t owning_string(struct he_str_span buf, const struct he_str_span source_file, uint32_t source_line, const struct he_str_span source_func) {
    struct he_str_span stripped_file = __source_file_stripper(source_file);
    int len = snprintf(buf.buf, buf.len, "%*.s(%05d)::%*.s", (int)stripped_file.len, stripped_file.buf, source_line, (int)source_func.len, source_func.buf);
    HE_RAW_ASSERT(len > 0);
    if(len < 0) {
        struct he_str_span unknown = c_to_str_span("??");
        memcpy(buf.buf, unknown.buf, unknown.len);
        return unknown.len;
    }
    return len;
}

static const int memory_size_string(struct he_str_span buf, uint32_t size) {
  int res = str_fmt_ull_comma_seperated(buf, size);
  if (res < 0)
    return res;

  const struct he_str_span strs[] = {
      c_to_str_span("B"),  c_to_str_span("KB"), c_to_str_span("MB"),
      c_to_str_span("GB"), c_to_str_span("TB"),
  };
  double value = (double)size;
  size_t i = 0;
  while ((value < 0 ? -value : value) >= 1024 &&
         i < sizeof(strs) / sizeof(*strs) - 1) {
    value /= 1024;
    ++i;
  }
  buf = sub_str_span(buf, res, buf.len);
  int res2 = snprintf(buf.buf, buf.len, " (%7.2f%*.s)", value, (int)strs[i].len,
                      strs[i].buf);
  if (res2 < 0)
    return -1;
  return res + res2;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static sAllocUnit *find_alloc_unit(const void *reportedAddress) {
  // Just in case...
  HE_RAW_ASSERT(reportedAddress != NULL);

  // Use the address to locate the hash index. Note that we shift off the lower
  // four bits. This is because most allocated addresses will be on four-,
  // eight- or even sixteen-byte boundaries. If we didn't do this, the hash
  // index would not have very good coverage.

  size_t hashIndex = (((size_t)reportedAddress) >> 4) & (hashSize - 1);
  sAllocUnit *ptr = hashTable[hashIndex];
  while (ptr) {
    if (ptr->reported_address == reportedAddress)
      return ptr;
    ptr = ptr->next;
  }

  return NULL;
}

static void *calculate_reported_address(const void *actual_address) {
  if (!actual_address)
    return NULL;

  // Just account for the padding
  return (void *)(((const char *)actual_address) +
                  sizeof(uint32_t) * HE_ALLOC_PADDING_SIZE);
}

static void wipe_with_pattern(sAllocUnit *allocUnit, uint32_t pattern,
                              const unsigned int originalReportedSize) {
  // For a serious test run, we use wipes of random a random value. However, if
  // this causes a crash, we don't want it to crash in a differnt place each
  // time, so we specifically DO NOT call srand. If, by chance your program
  // calls srand(), you may wish to disable that when running with a random wipe
  // test. This will make any crashes more consistent so they can be tracked
  // down easier.

  if (HE_ALLOC_RANDOM_WIPE) {
    pattern = ((rand() & 0xff) << 24) | ((rand() & 0xff) << 16) |
              ((rand() & 0xff) << 8) | (rand() & 0xff);
  }

  // -DOC- We should wipe with 0's if we're not in debug mode, so we can help
  // hide bugs if possible when we release the product. So uncomment the
  // following line for releases.
  //
  // Note that the "alwaysWipeAll" should be turned on for this to have effect,
  // otherwise it won't do much good. But we'll leave it this way (as an option)
  // because this does slow things down.
  //	pattern = 0;

  // This part of the operation is optional

  if (HE_ALLOC_ALWAYS_WIPE_ALL &&
      allocUnit->reportedSize > originalReportedSize) {
    // Fill the bulk

    uint32_t *lptr = (uint32_t *)(((char *)allocUnit->reported_address) +
                                  originalReportedSize);
    int length = (int)(allocUnit->reportedSize - originalReportedSize);
    int i;
    for (i = 0; i < (length >> 2); i++, lptr++) {
      *lptr = pattern;
    }

    // Fill the remainder

    unsigned int shiftCount = 0;
    char *cptr = (char *)(lptr);
    for (i = 0; i < (length & 0x3); i++, cptr++, shiftCount += 8) {
      *cptr = (char)((pattern & (0xff << shiftCount)) >> shiftCount);
    }
  }

  // Write in the prefix/postfix bytes

  // Calculate the correct start addresses for pre and post patterns relative to
  // allocUnit->reportedAddress, since it may have been offset due to alignment
  // requirements
  uint8_t *pre = (uint8_t *)allocUnit->reported_address -
                 HE_ALLOC_PADDING_SIZE * sizeof(uint32_t);
  uint8_t *post =
      (uint8_t *)allocUnit->reported_address + allocUnit->reportedSize;

  const size_t paddingBytes = HE_ALLOC_PADDING_SIZE * sizeof(uint32_t);
  for (size_t i = 0; i < paddingBytes; i++, pre++, post++) {
    *pre = (prefixPattern >> ((i % sizeof(uint32_t)) * 8)) & 0xFF;
    *post = (postfixPattern >> ((i % sizeof(uint32_t)) * 8)) & 0xFF;
  }
}

// need to handle situation where allocator is corrupted
static void __write_to_file(struct he_file *file, const char *format, ...) {
#define _ALLOC_WRITE_FILE_BUFFER_SIZE 4096 
  va_list args;
  char buffer[_ALLOC_WRITE_FILE_BUFFER_SIZE] = {0};
  va_start(args, format);
  vsnprintf(buffer, _ALLOC_WRITE_FILE_BUFFER_SIZE, format, args);
  va_end(args);
  fs_append_to_file(file, buffer, strnlen(buffer, _ALLOC_WRITE_FILE_BUFFER_SIZE));  
#undef _ALLOC_WRITE_FILE_BUFFER_SIZE
}

#ifdef HE_ALLOC_BACKTRACE
static void __dump_backtrace(struct he_file* file, const sAllocUnit* ptr)
{
    if (ptr->backtrace_nptrs)
    {
#ifdef _WIN32
        HANDLE process = gProcessHandle;
        SymInitialize(process, NULL, TRUE);

        char         buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

        char             buffer_line[sizeof(IMAGEHLP_LINE64)];
        PIMAGEHLP_LINE64 pLine = (PIMAGEHLP_LINE64)buffer_line;

        char module_name[FS_MAX_PATH];

        for (int i = ptr->backtrace_skip; i < ptr->backtrace_nptrs; i++)
        {
            DWORD64 address = (DWORD64)(ptr->backtrace_buffer[i]);

            // Get symbol name for address
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen = MAX_SYM_NAME;
            SymFromAddr(process, address, 0, pSymbol);

            // Try to get line
            pLine->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD displacement;
            if (SymGetLineFromAddr64(process, address, &displacement, pLine))
            {
                if (fh)
                {
                    dumpLine(fh, "    at %s in %s: line: %lu: address: 0x%0X", pSymbol->Name, pLine->FileName, pLine->LineNumber,
                             pSymbol->Address);
                }
                else
                {
                    Log("    at %s in %s: line: %lu: address: 0x%0X", pSymbol->Name, pLine->FileName, pLine->LineNumber, pSymbol->Address);
                }
            }
            else
            {
                // Try get the module name
                HMODULE hModule = NULL;
                module_name[0] = '\0';
                GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)(address),
                                  &hModule);
                if (hModule != NULL)
                    GetModuleFileNameA(hModule, module_name, FS_MAX_PATH);

                if (fh)
                {
                    dumpLine(fh, "    at %s, address 0x%0X in %s", pSymbol->Name, pSymbol->Address, module_name);
                }
                else
                {
                    Log("    at %s, address 0x%0X in %s", pSymbol->Name, pSymbol->Address, module_name);
                }
            }
        }

        SymCleanup(process);
#else
      char **strings =
          backtrace_symbols(ptr->backtrace_buffer, ptr->backtrace_nptrs);
      if (strings != NULL) {
        for (int j = ptr->backtrace_skip; j < ptr->backtrace_nptrs; j++) {
          if (file) {
            __write_to_file(file, "\t%s", strings[j]);
          } else {
            HE_LOGF(eMEMORY_ALLOC, "\t%s", strings[j]);
          }
        }
        free(strings);
      }
#endif
    }
}
#endif


static void __write_allocations(struct he_file* file)
{
    __write_to_file(file, "Alloc.        Addr           Size           Addr           Size               BreakOn BreakOn");
    __write_to_file(file, "Number      Reported       Reported        Actual         Actual     Unused   Dealloc Realloc  Allocated by");
    __write_to_file(file,"------ ------------------ ---------- ------------------ ---------- ---------- ------- ------- ---------------------------------------------------");
    for (unsigned int i = 0; i < hashSize; i++) {
      sAllocUnit *ptr = hashTable[i];
      while (ptr) {
        char source_buffer[256]; 
        struct he_str_span buf = {source_buffer, HE_ARRAY_COUNT(source_buffer)};
        const struct he_str_span source_file = {ptr->source_file, ptr->source_file_len};
        const struct he_str_span source_func = {ptr->source_func, ptr->source_func_len};
        const size_t len = owning_string(buf, source_file, ptr->sourceLine, source_func);
        __write_to_file(file,
                        "% 6d 0x%016zX 0x%08zX 0x%016zX 0x%08zX 0x%08X    %c       %c    %*.s",
                        ptr->allocationNumber, (size_t)(ptr->reported_address),
                        ptr->reportedSize, (size_t)(ptr->actual_address),
                        ptr->actual_size, calc_unused_alloc(ptr),
                        ptr->breakOnDealloc ? 'Y' : 'N',
                        ptr->breakOnRealloc ? 'Y' : 'N', len, buf.buf);
#ifdef HE_ALLOC_BACKTRACE
        __dump_backtrace(file, ptr);
#endif
        ptr = ptr->next;
      }
    }
}

void write_leak_report(struct he_file *file) {
  {
    // Header
    time_t t = time(NULL);
    struct tm tme;
#ifdef HE_TARGET_WINDOWS
    localtime_s(&tme, &t);
#else
    localtime_s(&t, &tme);
#endif

    {
      __write_to_file(file,
                      " -------------------------------------------------------------------------------\n"
                      "                Memory leak report %02d/%02d/%04d %02d:%02d:%02d:               \n"
                      " -------------------------------------------------------------------------------\n",
                      tme.tm_mon + 1, tme.tm_mday, tme.tm_year + 1900,
                      tme.tm_hour, tme.tm_min, tme.tm_sec);
    }
    if (stats.totalAllocUnitCount) {
      __write_to_file(file, "%d memory leak%s found:\n",
                      stats.totalAllocUnitCount,
                      stats.totalAllocUnitCount == 1 ? "" : "s");
    } else {
      __write_to_file(file, "Congratulations! No memory leaks found!\n");
      // We can finally free up our own memory allocations

      if (reservoirBuffer) {
        for (unsigned int i = 0; i < reservoirBufferSize; i++) {
          free(reservoirBuffer[i]);
        }
        free(reservoirBuffer);
        reservoirBuffer = 0;
        reservoirBufferSize = 0;
        reservoir = NULL;
      }
    }

    if (stats.totalAllocUnitCount) {
      __write_allocations(file);
    }

    __write_to_file(file, "----All Allocations and Deallocations----");

    // dumpLine(&fh, allMemoryLog);

    if (!stats.totalAllocUnitCount) {
      __write_to_file(file, " ------------------------------------------------------------------------------");
      __write_to_file(file, "Congratulations! No memory leaks found!");
      __write_to_file(file, " ------------------------------------------------------------------------------");
    }
  }

  HE_RAW_ASSERT(stats.totalAllocUnitCount == 0 && "Memory leaks found");
}

void he_exit_alloc() {

  struct he_file file = {0};
  fs_open("./MemLeaks.memleak", HE_FS_WRITE, &file);
  write_leak_report(&file);
  fs_close(&file);

#ifdef HE_ALLOC_BACKTRACE
#ifdef _WIN32
    CloseHandle(gProcessHandle);
#endif
#endif

}

struct MemoryStatistics he_get_memory_stats()
{
    MemoryStatistics result = {
        .totalReportedMemory = (uint32_t)stats.totalReportedMemory,
        .totalActualMemory = (uint32_t)stats.totalActualMemory,
        .peakReportedMemory = (uint32_t)stats.peakReportedMemory,
        .peakActualMemory = (uint32_t)stats.peakActualMemory,
        .accumulatedReportedMemory = (uint32_t)stats.accumulatedReportedMemory,
        .accumulatedActualMemory = (uint32_t)stats.accumulatedActualMemory,
        .accumulatedAllocUnitCount = (uint32_t)stats.accumulatedAllocUnitCount,
        .totalAllocUnitCount = (uint32_t)stats.totalAllocUnitCount,
        .peakAllocUnitCount = (uint32_t)stats.peakAllocUnitCount,
    };
    return result;
}

static sAllocUnit *fetch_new_alloc_unit() {
  if (!reservoir) {
    // Allocate 256 reservoir elements

    reservoir = (sAllocUnit *)malloc(sizeof(sAllocUnit) * 256);

    // If you hit this assert, then the memory manager failed to allocate
    // internal memory for tracking the allocations
    HE_RAW_ASSERT(reservoir != NULL);

    // Danger Will Robinson!

    if (reservoir == NULL) {
      printf("Unable to allocate RAM for internal memory tracking data");
      fflush(stdout);
      HE_RAW_ASSERT(false &&
                    "Unable to allocate RAM for internal memory tracking data");
      return NULL;
    }
    // Build a linked-list of the elements in our reservoir

    memset(reservoir, 0, sizeof(sAllocUnit) * 256);
    for (unsigned int i = 0; i < 256 - 1; i++) {
      reservoir[i].next = &reservoir[i + 1];
    }

    // Add this address to our reservoirBuffer so we can free it later

    sAllocUnit **temp = (sAllocUnit **)realloc(
        reservoirBuffer, (reservoirBufferSize + 1) * sizeof(sAllocUnit *));
    HE_RAW_ASSERT(temp);
    if (temp) {
      reservoirBuffer = temp;
      reservoirBuffer[reservoirBufferSize++] = reservoir;
    }
  }

  // Logical flow says this should never happen...
  HE_RAW_ASSERT(reservoir != NULL);

  // Grab a new allocaton unit from the front of the reservoir

  sAllocUnit *au = reservoir;
  reservoir = au->next;
  return au;
}

void m_breakOnAllocation(unsigned int count) { breakOnAllocationCount = count; }

static size_t calculate_actual_size(const size_t reportedSize)
{
    // We use DWORDS as our padding, and a uint32_t is guaranteed to be 4 bytes, but an int is not (ANSI defines an int as
    // being the standard word size for a processor; on a 32-bit machine, that's 4 bytes, but on a 64-bit machine, it's
    // 8 bytes, which means an int can actually be larger than a uint32_t.)

    return reportedSize + HE_ALLOC_PADDING_SIZE * sizeof(uint32_t) * 2;
}

void *_internal_he_tracked_malloc(void *user, size_t size, const char *source_file, int l,
                                  const char *source_function) {
  // Make sure alignment is valid
  const size_t alignment = sizeof(void *);
  he_acquire_mutex(&alloc_mutex);
  currentAllocationCount++;
  if (HE_ALLOC_ALWAYS_LOG_ALL) {
    char source_buffer[256];
    struct he_str_span buf = {source_buffer, HE_ARRAY_COUNT(source_buffer)};
    const size_t len = owning_string(buf, c_to_str_span(source_file), l,
                                     c_to_str_span(source_function));
    HE_LOGF(eMEMORY_ALLOC, "[+] %05d of size 0x%08X(%08d) by %*.s",
            currentAllocationCount, size, size, (int)len, buf.buf);
  }
  sAllocUnit *au = fetch_new_alloc_unit(); // this should never be null
  memset(au, 0, sizeof(sAllocUnit));
  au->actual_size = calculate_actual_size(size) + alignment;
  au->actual_address = malloc(au->actual_size);
  au->reportedSize = size;
  au->reported_address = calculate_reported_address(au->actual_address);
  au->alignment = alignment;
  au->sourceLine = sourceLine;
  au->allocationNumber = currentAllocationCount;
  
  HE_RAW_ASSERT(au->actual_address);
  // Make sure the address we return to user is aligned to the specified
  // alignment
  size_t offset = ((size_t)au->reported_address) % alignment;
  if (offset) {
    au->reported_address = (uint8_t *)au->reported_address + (alignment - offset);
  }
  au->offset = offset;

  struct he_str_span source_file_span = source_file ? c_to_str_span(source_file) : c_to_str_span("??");
  struct he_str_span source_func_span = source_function ? c_to_str_span(source_function) : c_to_str_span("??");
  au->source_file_len = HE_MIN(source_file_span.len, Q_ARRAY_COUNT(au->source_file));
  au->source_func_len = HE_MIN(source_func_span.len, Q_ARRAY_COUNT(au->source_func));
  memcpy(au->source_file, source_file_span.buf, au->source_file_len);
  memcpy(au->source_func, source_func_span.buf, au->source_func_len);

#ifdef HE_ALLOC_BACKTRACE
#ifdef _WIN32
  au->backtrace_nptrs = CaptureStackBackTrace(
      stackSkipCount + 1, HE_BACKTRACE_SIZE, au->backtrace_buffer, NULL);
  // Skipped for us above
  au->backtrace_skip = 0;
#else
  au->backtrace_nptrs = backtrace(au->backtrace_buffer, HE_BACKTRACE_SIZE);
  au->backtrace_skip = stackSkipCount + 1;
#endif
#endif

  size_t hashIndex = (((size_t)au->reported_address) >> 4) & (hashSize - 1);
  if (hashTable[hashIndex])
    hashTable[hashIndex]->prev = au;
  au->next = hashTable[hashIndex];
  au->prev = NULL;
  hashTable[hashIndex] = au;

  // Account for the new allocatin unit in our stats
  stats.totalReportedMemory += (unsigned int)(au->reportedSize);
  stats.totalActualMemory += (unsigned int)(au->actual_size);
  stats.totalAllocUnitCount++;
  if (stats.totalReportedMemory > stats.peakReportedMemory)
    stats.peakReportedMemory = stats.totalReportedMemory;
  if (stats.totalActualMemory > stats.peakActualMemory)
    stats.peakActualMemory = stats.totalActualMemory;
  if (stats.totalAllocUnitCount > stats.peakAllocUnitCount)
    stats.peakAllocUnitCount = stats.totalAllocUnitCount;
  stats.accumulatedReportedMemory += (unsigned int)(au->reportedSize);
  stats.accumulatedActualMemory += (unsigned int)(au->actual_size);
  stats.accumulatedAllocUnitCount++;

  // Prepare the allocation unit for use (wipe it with recognizable garbage)
  wipe_with_pattern(au, unusedPattern, 0);

  // Validate every single allocated unit in memory
  if (HE_ALLOC_ALWAYS_VALIDATE_ALL)
    he_validate_all_allocations();

  // Log the result
  if (HE_ALLOC_ALWAYS_LOG_ALL)
    HE_LOGF(eERROR, "[+] ---->             addr 0x%08zX",
            (size_t)(au->reported_address));

  he_release_mutex(&alloc_mutex);
  return au->reported_address;
}
void* _internal_he_tracked_realloc(void* user,void* ptr, size_t size, const char* f, int l, const char* sf) {
    // Round up the size to a multiple of sizeof(uint32_t) so we don't write to misaligned pointers in wipeWithPattern.
    size += sizeof(uint32_t) - 1;
    size &= ~(sizeof(uint32_t) - 1);
    if(ptr == NULL) {
        return _internal_he_tracked_malloc(user, size, f, l, sf);
    }
    he_acquire_mutex(&alloc_mutex);
    currentAllocationCount++;
    if (HE_ALLOC_ALWAYS_LOG_ALL) {
      char source_buffer[256];
      struct he_str_span buf = {source_buffer, HE_ARRAY_COUNT(source_buffer)};
      const size_t len = owning_string(buf, c_to_str_span(f), l, c_to_str_span(sf));
      HE_LOGF(eMEMORY_ALLOC, "[+] %05d of size 0x%08X(%08d)",
              currentAllocationCount, size, size, (int)len, buf.buf);
    }
    sAllocUnit *au = find_alloc_unit(ptr);
    HE_LOGF_IF(eMEMORY_ALLOC, au == NULL, "[!] Requested to reallocated RAM that was never allocated");
    HE_RAW_ASSERT(au != NULL); // ptr has not been allocated 
    
    {
        const bool is_valid = he_validate_alloc_unit(au);
        HE_RAW_ASSERT(is_valid);
    }

    const size_t alignment = au->alignment;
    const size_t old_reported_size = au->reportedSize;
    const void* old_reported_address = au->reported_address;
    const unsigned int orignal_reported_size = (unsigned int)(au->reportedSize);
    if (HE_ALLOC_ALWAYS_LOG_ALL)
      HE_LOGF(eMEMORY_ALLOC, "[~] ---->             from 0x%08X(%08d)",
              orignal_reported_size, orignal_reported_size);
    const size_t new_actual_size = calculate_actual_size(size) + alignment;
    void *new_actual_address = realloc(au->actual_address, new_actual_size);
    HE_LOGF_IF(eMEMORY_ALLOC, new_actual_address == NULL,
               "[!] Request for reallocation failed. Out of memory.");
    const ptrdiff_t old_ptr_diff =
        ((uint8_t *)au->reported_address - (uint8_t *)au->actual_address);

    // Remove this allocation from our stats (we'll add the new reallocation
    // again later)
    stats.totalReportedMemory -= (unsigned int)(au->reportedSize);
    stats.totalActualMemory -= (unsigned int)(au->actual_size);

    // Update the allocation with the new information
    au->actual_size = new_actual_size;
    au->actual_address = new_actual_address;
    au->reportedSize = size;
    au->reported_address = calculate_reported_address(new_actual_address);
    au->sourceLine = sourceLine;
    au->allocationNumber = currentAllocationCount;

    // Make sure the address we return to user is aligned to the specified
    // alignment
    const size_t offset = ((size_t)au->reported_address) % alignment;
    if (offset) {
      au->reported_address =
          (uint8_t *)au->reported_address + (alignment - offset);
    }
    const ptrdiff_t new_ptr_diff =
        (uint8_t *)au->reported_address - (uint8_t *)au->actual_address;
    if (new_ptr_diff != old_ptr_diff) {
      memmove(au->reported_address,
              (uint8_t *)au->actual_address + old_ptr_diff,
              HE_MIN(new_actual_size, old_reported_size));
    }

    struct he_str_span source_file_span =
        f ? c_to_str_span(f) : c_to_str_span("??");
    struct he_str_span source_func_span =
        sf ? c_to_str_span(sf) : c_to_str_span("??");
    au->source_file_len =
        HE_MIN(source_file_span.len, Q_ARRAY_COUNT(au->source_file));
    au->source_func_len =
        HE_MIN(source_func_span.len, Q_ARRAY_COUNT(au->source_func));
    memcpy(au->source_file, source_file_span.buf, au->source_file_len);
    memcpy(au->source_func, source_func_span.buf, au->source_func_len);
#ifdef HE_ALLOC_BACKTRACE
#ifdef _WIN32
    au->backtrace_nptrs = CaptureStackBackTrace(
        stackSkipCount + 1, HE_BACKTRACE_SIZE, au->backtrace_buffer, NULL);
    // Skipped for us above
    au->backtrace_skip = 0;
#else
    au->backtrace_nptrs = backtrace(au->backtrace_buffer, HE_BACKTRACE_SIZE);
    au->backtrace_skip = stackSkipCount + 1;
#endif
#endif

    unsigned int hashIndex = (unsigned int)(-1);
    if (old_reported_address != au->reported_address) {
      // Remove this allocation unit from the hash table
      {
        size_t hashIndex =
            (((size_t)old_reported_address) >> 4) & (hashSize - 1);
        if (hashTable[hashIndex] == au) {
          hashTable[hashIndex] = hashTable[hashIndex]->next;
        } else {
          if (au->prev)
            au->prev->next = au->next;
          if (au->next)
            au->next->prev = au->prev;
        }
      }

      // Re-insert it back into the hash table

      hashIndex = (((size_t)au->reported_address) >> 4) & (hashSize - 1);
      if (hashTable[hashIndex])
        hashTable[hashIndex]->prev = au;
      au->next = hashTable[hashIndex];
      au->prev = NULL;
      hashTable[hashIndex] = au;
    }

    stats.totalReportedMemory += (unsigned int)(au->reportedSize);
    stats.totalActualMemory += (unsigned int)(au->actual_size);
    if (stats.totalReportedMemory > stats.peakReportedMemory)
      stats.peakReportedMemory = stats.totalReportedMemory;
    if (stats.totalActualMemory > stats.peakActualMemory)
      stats.peakActualMemory = stats.totalActualMemory;
    int deltaReportedSize = (int)(size - orignal_reported_size);
    if (deltaReportedSize > 0) {
      stats.accumulatedReportedMemory += deltaReportedSize;
      stats.accumulatedActualMemory += deltaReportedSize;
    }

    wipe_with_pattern(au, unusedPattern, orignal_reported_size);
    {
        const bool is_valid = he_validate_alloc_unit(au);
        HE_RAW_ASSERT(is_valid);
    }

    // Validate every single allocated unit in memory
    if (HE_ALLOC_ALWAYS_VALIDATE_ALL)
      he_validate_all_allocations();

    if (HE_ALLOC_ALWAYS_LOG_ALL)
      HE_LOGF(eERROR, "[~] ---->             addr 0x%08zX",
              (size_t)(au->reported_address));
    he_release_mutex(&alloc_mutex);
    return au->reported_address;
}

void *_internal_he_tracked_calloc(void *user, size_t count, size_t size,
                                  const char *f, int l, const char *sf) {
  void *ptr = _internal_he_tracked_malloc(user, count * size, f, l, sf);
  memset(ptr, 0, count * size);
  return ptr;
}
void  _internal_he_tracked_free(void* user,void* ptr, const char* f, int l, const char* sf){
  if (ptr == NULL)
    return;

  he_acquire_mutex(&alloc_mutex);
  sAllocUnit *au = find_alloc_unit(ptr);
  
if (HE_ALLOC_ALWAYS_LOG_ALL)
    HE_LOGF(eERROR, "[~] ---->             addr 0x%08zX",
            (size_t)(au->reported_address));

  if (HE_ALLOC_ALWAYS_LOG_ALL) {
    char source_buffer[256];
    struct he_str_span buf = {source_buffer, HE_ARRAY_COUNT(source_buffer)};
    const size_t len = owning_string(buf, c_to_str_span(f), l, c_to_str_span(sf));
    HE_LOGF(eMEMORY_ALLOC, "[-] -----  0x%08zX by %*.s",
            (size_t)((void *)(ptr)), (int)len, buf.buf);
  }

    HE_LOGF_IF(eMEMORY_ALLOC, au == NULL,
             "[!] Requested to reallocated RAM that was never allocated");
  HE_RAW_ASSERT(au != NULL); // ptr has not been allocated
  {
    const bool is_valid = he_validate_alloc_unit(au);
    HE_RAW_ASSERT(is_valid);
  }

  wipe_with_pattern(au, releasedPattern, 0);

  free(au->actual_address);

  size_t hashIndex = ((size_t)(au->reported_address) >> 4) & (hashSize - 1);
  if (hashTable[hashIndex] == au) {
    hashTable[hashIndex] = au->next;
  } else {
    if (au->prev)
      au->prev->next = au->next;
    if (au->next)
      au->next->prev = au->prev;
  }

  // Remove this allocation from our stats

  stats.totalReportedMemory -= (unsigned int)(au->reportedSize);
  stats.totalActualMemory -= (unsigned int)(au->actual_size);
  stats.totalAllocUnitCount--;

  // Add this allocation unit to the front of our reservoir of unused allocation units
  memset(au, 0, sizeof(sAllocUnit));
  au->next = reservoir;
  reservoir = au;

  // Validate every single allocated unit in memory
  if (HE_ALLOC_ALWAYS_VALIDATE_ALL)
    he_validate_all_allocations();

  he_release_mutex(&alloc_mutex);
}


static bool he_validate_alloc_unit(const sAllocUnit* allocUnit)
{
    // Make sure the padding is untouched

    uint8_t*     pre = ((uint8_t*)allocUnit->reported_address - HE_ALLOC_PADDING_SIZE * sizeof(uint32_t));
    uint8_t*     post = ((uint8_t*)allocUnit->reported_address + allocUnit->reportedSize);
    bool         errorFlag = false;
    const size_t paddingBytes = HE_ALLOC_PADDING_SIZE * sizeof(uint32_t);
    for (size_t i = 0; i < paddingBytes; i++, pre++, post++)
    {
        const uint8_t expectedPrefixByte = (prefixPattern >> ((i % sizeof(uint32_t)) * 8)) & 0xFF;
        if (*pre != expectedPrefixByte)
        {
            HE_LOGF(eMEMORY_ALLOC,"[!] A memory allocation unit was corrupt because of an underrun:");
            he_dump_alloc_unit(allocUnit, "  ");
            errorFlag = true;
        }

        // If you hit this assert, then you should know that this allocation unit has been damaged. Something (possibly the
        // owner?) has underrun the allocation unit (modified a few bytes prior to the start). You can interrogate the
        // variable 'allocUnit' to see statistics and information about this damaged allocation unit.
        HE_RAW_ASSERT(*pre == expectedPrefixByte);

        const uint8_t expectedPostfixByte = (postfixPattern >> ((i % sizeof(uint32_t)) * 8)) & 0xFF;
        if (*post != expectedPostfixByte)
        {
            HE_LOGF(eMEMORY_ALLOC,"[!] A memory allocation unit was corrupt because of an overrun:");
            he_dump_alloc_unit(allocUnit, "  ");
            errorFlag = true;
        }

        // If you hit this assert, then you should know that this allocation unit has been damaged. Something (possibly the
        // owner?) has overrun the allocation unit (modified a few bytes after the end). You can interrogate the variable
        // 'allocUnit' to see statistics and information about this damaged allocation unit.
        HE_RAW_ASSERT(*post == expectedPostfixByte);
    }

    // Return the error status (we invert it, because a return of 'false' means error)

    return !errorFlag;
}

// ---------------------------------------------------------------------------------------------------------------------------------

int he_validate_all_allocations()
{
    // Just go through each allocation unit in the hash table and count the ones that have errors

    unsigned int errors = 0;
    unsigned int allocCount = 0;
    for (unsigned int i = 0; i < hashSize; i++)
    {
        sAllocUnit* ptr = hashTable[i];
        while (ptr)
        {
            allocCount++;
            if (!he_validate_alloc_unit(ptr))
                errors++;
            ptr = ptr->next;
        }
    }

    // Test for hash-table correctness

    if (allocCount != stats.totalAllocUnitCount)
    {
        HE_LOGF(eERROR,"[!] Memory tracking hash table corrupt!");
        errors++;
    }

    // If you hit this assert, then the internal memory (hash table) used by this memory tracking software is damaged! The
    // best way to track this down is to use the alwaysLogAll flag in conjunction with STRESS_TEST macro to narrow in on the
    // offending code. After running the application with these settings (and hitting this assert again), interrogate the
    // memory.log file to find the previous successful operation. The corruption will have occurred between that point and this
    // assertion.
    HE_ASSERT(allocCount == stats.totalAllocUnitCount);

    // If you hit this assert, then you've probably already been notified that there was a problem with a allocation unit in a
    // prior call to validateAllocUnit(), but this assert is here just to make sure you know about it. :)
    HE_ASSERT(errors == 0);

    // Log any errors

    if (errors)
        HE_LOGF(eERROR,"[!] While validting all allocation units, %d allocation unit(s) were found to have problems", errors);

    // Return the error status

    return errors != 0;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Unused RAM calculation routines. Use these to determine how much of your RAM is unused (in bytes)
// ---------------------------------------------------------------------------------------------------------------------------------

static unsigned int calc_unused_alloc(const sAllocUnit* allocUnit)
{
    const uint32_t* ptr = (const uint32_t*)(allocUnit->reported_address);
    unsigned int    count = 0;

    for (unsigned int i = 0; i < allocUnit->reportedSize; i += sizeof(uint32_t), ptr++)
    {
        if (*ptr == unusedPattern)
            count += sizeof(uint32_t);
    }

    return count;
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned int mmgrCalcAllUnused(void)
{
    // Just go through each allocation unit in the hash table and count the unused RAM

    unsigned int total = 0;
    for (unsigned int i = 0; i < hashSize; i++)
    {
        sAllocUnit* ptr = hashTable[i];
        while (ptr)
        {
            total += calc_unused_alloc(ptr);
            ptr = ptr->next;
        }
    }

    return total;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- The following functions are for logging and statistics reporting.
// ---------------------------------------------------------------------------------------------------------------------------------

static void he_dump_alloc_unit(const sAllocUnit* allocUnit, const char* prefix)
{
    char source_buffer[256]; 
    struct he_str_span buf = {source_buffer, HE_ARRAY_COUNT(source_buffer)};
    HE_LOGF(eMEMORY_ALLOC,"[I] %sAddress (reported): %010p", prefix, allocUnit->reported_address);
    HE_LOGF(eMEMORY_ALLOC,"[I] %sAddress (actual)  : %010p", prefix, allocUnit->actual_address);
    {
        const int len = memory_size_string(buf,(unsigned int)(allocUnit->reportedSize));
        HE_LOGF(eMEMORY_ALLOC,"[I] %sSize (reported)   : 0x%08X (%*.s)", prefix, (unsigned int)(allocUnit->reportedSize),
            (int)HE_MAX(0,len), buf.buf);
    }
    {
        const int len = memory_size_string(buf,(unsigned int)(allocUnit->actual_size));
        HE_LOGF(eMEMORY_ALLOC,"[I] %sSize (actual)     : 0x%08X (%*.s)", prefix, (unsigned int)(allocUnit->actual_size),
            (int)HE_MAX(0, len), buf.buf);
    }
    HE_LOGF(eMEMORY_ALLOC, "[I] %sOwner             : %*.s(%d)::%*.s", prefix,
            (int)allocUnit->source_file_len, allocUnit->source_file,
            allocUnit->sourceLine, 
            (int)allocUnit->source_func_len, allocUnit->source_func);

#ifdef HE_ALLOC_BACKTRACE
    HE_LOGF(eMEMORY_ALLOC,"[I] %sBacktrace             : ", prefix);
    __dump_backtrace(NULL, allocUnit);
#endif

    //HE_LOGF(eMEMORY_ALLOC,"[I] %sAllocation type   : %s", prefix, allocationTypes[allocUnit->allocationType]);
    HE_LOGF(eMEMORY_ALLOC,"[I] %sAllocation number : %d", prefix, allocUnit->allocationNumber);
}


