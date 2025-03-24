
#include "he/core/arch.h"

struct HeAllocator {
  void* (*malloc)(size_t size, const char* f, int l, const char* sf);
  void* (*calloc)(size_t count, size_t size, const char* f, int l, const char* sf);
  void* (*realloc)(void* ptr, size_t size, const char* f, int l, const char* sf);
  void  (*free)(void* ptr, const char* f, int l, const char* sf);
};

void* _internal_he_tracked_malloc(size_t size, const char* f, int l, const char* sf);
void* _internal_he_tracked_calloc(size_t count, size_t size, const char* f, int l, const char* sf);
void* _internal_he_tracked_realloc(void* ptr, size_t size, const char* f, int l, const char* sf);
void  _internal_he_tracked_free(void* ptr, const char* f, int l, const char* sf);

#define he_allocator_malloc(allocator, size) allocator->malloc(size, __FILE__, __LINE__, __FUNCTION__ )
#define he_allocator_calloc(allocator, couint, size) allocator->calloc(count, size, __FILE__, __LINE__, __FUNCTION__ )
#define he_allocator_realloc(allocator, ptr, size) allocator->realloc(ptr, size, __FILE__, __LINE__, __FUNCTION__ )
#define he_allocator_free(allocator, ptr) allocator->free(ptr, __FILE__, __LINE__, __FUNCTION__ )

static void *_system_malloc(size_t size, const char *f, int l, const char *sf) {
  return malloc(size);
}
static void *_system_calloc(size_t count, size_t size, const char *f, int l,
                            const char *sf) {
  return calloc(count, size);
}
static void *_system_realloc(void *ptr, size_t size, const char *f, int l,
                             const char *sf) {
  return realloc(ptr, size);
}
static void _system_free(void *ptr, const char *f, int l, const char *sf) {
  free(ptr);
}

static struct HeAllocator SystemAllocator = {_system_malloc, 
                                             _system_calloc,
                                             _system_realloc, 
                                             _system_free};
static struct HeAllocator TrackedAllocator = {
    _internal_he_tracked_malloc, 
    _internal_he_tracked_calloc,
    _internal_he_tracked_realloc, 
    _internal_he_tracked_free
};

#ifdef HE_ALLOC_TRACKED
#define DECLARE_DEFAULT_HE_ALLOCATOR TrackedAllocator 
#define he_malloc(size) TrackedAllocator.malloc(size, __FILE__, __LINE__, __FUNCTION__ )
#define he_calloc(couint, size) TrackedAllocator.calloc(count, size, __FILE__, __LINE__, __FUNCTION__ )
#define he_realloc(ptr, size) TrackedAllocator.realloc(ptr, size, __FILE__, __LINE__, __FUNCTION__ )
#define he_free(ptr) TrackedAllocator.free(ptr, __FILE__, __LINE__, __FUNCTION__ )
#else
#define DECLARE_DEFAULT_HE_ALLOCATOR SystemAllocator 
#define he_malloc(size) SystemAllocator.malloc(size, __FILE__, __LINE__, __FUNCTION__ )
#define he_calloc(couint, size) SystemAllocator.calloc(count, size, __FILE__, __LINE__, __FUNCTION__ )
#define he_realloc(ptr, size) SystemAllocator.realloc(ptr, size, __FILE__, __LINE__, __FUNCTION__ )
#define he_free(ptr) SystemAllocator.free(ptr, __FILE__, __LINE__, __FUNCTION__ )
#endif



