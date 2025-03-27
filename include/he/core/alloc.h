
#include "he/core/arch.h"
#include "he/core/str.h"

struct he_file;

typedef struct tag_au
{
	size_t         actual_size;
	size_t         reportedSize;
	size_t         alignment;
	size_t         offset;
	void*          actual_address;
	void*          reported_address;

  size_t source_file_len;
  char source_file[140];
  
  size_t source_func_len;
  char source_func[140];

#ifdef HE_ALLOC_BACKTRACE
	void*          backtrace_buffer[HE_BACKTRACE_SIZE];

	int            backtrace_nptrs;
	int            backtrace_skip;
#endif
	unsigned int   sourceLine;
	//unsigned int   allocationType;
	bool           breakOnDealloc;
	bool           breakOnRealloc;
	unsigned int   allocationNumber;
	struct tag_au* next;
	struct tag_au* prev;
} sAllocUnit;

typedef struct
{
	unsigned int totalReportedMemory;
	unsigned int totalActualMemory;
	unsigned int peakReportedMemory;
	unsigned int peakActualMemory;
	unsigned int accumulatedReportedMemory;
	unsigned int accumulatedActualMemory;
	unsigned int accumulatedAllocUnitCount;
	unsigned int totalAllocUnitCount;
	unsigned int peakAllocUnitCount;
} sMStats;

typedef struct MemoryStatistics
{
    uint32_t totalReportedMemory;
    uint32_t totalActualMemory;
    uint32_t peakReportedMemory;
    uint32_t peakActualMemory;
    uint32_t accumulatedReportedMemory;
    uint32_t accumulatedActualMemory;
    uint32_t accumulatedAllocUnitCount;
    uint32_t totalAllocUnitCount;
    uint32_t peakAllocUnitCount;
} MemoryStatistics;

struct he_allocator {
  void* user;
  void* (*malloc)(void* user,size_t size, const char* f, int l, const char* sf);
  void* (*calloc)(void* user,size_t count, size_t size, const char* f, int l, const char* sf);
  void* (*realloc)(void* user,void* ptr, size_t size, const char* f, int l, const char* sf);
  void  (*free)(void* user,void* ptr, const char* f, int l, const char* sf);
};

void he_init_alloc();
void he_exit_alloc();
int he_validate_all_allocations();

struct MemoryStatistics he_get_memory_stats();

void* _internal_he_tracked_malloc(void* user,size_t size, const char* f, int l, const char* sf);
void* _internal_he_tracked_calloc(void* user,size_t count, size_t size, const char* f, int l, const char* sf);
void* _internal_he_tracked_realloc(void* user,void* ptr, size_t size, const char* f, int l, const char* sf);
void  _internal_he_tracked_free(void* user,void* ptr, const char* f, int l, const char* sf);

void write_leak_report(struct he_file* file); 

static void *_system_malloc(void* user,size_t size, const char *f, int l, const char *sf) {
  return malloc(size);
}

static void *_system_calloc(void* user,size_t count, size_t size, const char *f, int l, const char *sf) {
  return calloc(count, size);
}

static void *_system_realloc(void* user,void *ptr, size_t size, const char *f, int l,
                             const char *sf) {
  return realloc(ptr, size);
}

static void _system_free(void* user,void *ptr, const char *f, int l, const char *sf) {
  free(ptr);
}

static struct he_allocator system_allocator = {NULL,_system_malloc, 
                                              _system_calloc,
                                              _system_realloc, 
                                              _system_free};
static struct he_allocator tracked_allocator = {NULL,
    _internal_he_tracked_malloc, _internal_he_tracked_calloc,
    _internal_he_tracked_realloc, _internal_he_tracked_free};



static inline void *__he_alloc_malloc(struct he_allocator *alloc, size_t size,
                                      const char *f, int l, const char *sf) {
  if (alloc) {
    alloc->malloc(alloc->user, size, f, l, sf);
  }
  return system_allocator.malloc(system_allocator.user, size, f, l, sf);
}
static inline void *__he_alloc_calloc(struct he_allocator *alloc, size_t count, size_t size,
                                      const char *f, int l, const char *sf) {
  if (alloc) {
    alloc->calloc(alloc->user, count, size, f, l, sf);
  }
  return system_allocator.calloc(system_allocator.user, count, size, f, l, sf);
}

static inline void *__he_alloc_realloc(struct he_allocator *alloc, void* ptr, size_t size,
                                      const char *f, int l, const char *sf) {
  if (alloc) {
    alloc->realloc(alloc->user, ptr, size, f, l, sf);
  }
  return system_allocator.realloc(system_allocator.user, ptr, size, f, l, sf);
}
static inline void __he_alloc_free(struct he_allocator *alloc, void *ptr,
                                   const char *f, int l, const char *sf) {
  if (alloc) {
    alloc->free(alloc->user, ptr, f, l, sf);
    return;
  }
  system_allocator.free(system_allocator.user, ptr, f, l, sf);
}

#ifdef HE_ALLOC_TRACKED
#define DECLARE_DEFAULT_HE_ALLOCATOR tracked_allocator 
#define he_malloc(size) he_alloc_malloc(&tracked_allocator, size)
#define he_calloc(couint, size) he_alloc_calloc(&tracked_allocator, count, size)
#define he_realloc(ptr, size) he_alloc_realloc(&tracked_allocator, ptr, size)
#define he_free(ptr) he_alloc_free(&tracked_allocator, ptr)
#else
#define DECLARE_DEFAULT_HE_ALLOCATOR system_allocator  
#define he_malloc(size) he_alloc_malloc(&system_allocator, size)
#define he_calloc(couint, size) he_alloc_calloc(&system_allocator, count, size)
#define he_realloc(ptr, size) he_alloc_realloc(&system_allocator, ptr, size)
#define he_free(ptr) he_alloc_free(&system_allocator, ptr)
#endif

#define he_alloc_malloc(allocator, size) __he_alloc_malloc((allocator),size, __FILE__, __LINE__, __FUNCTION__)
#define he_alloc_calloc(allocator, count, size) __he_alloc_calloc((allocator),count, size, __FILE__, __LINE__, __FUNCTION__ )
#define he_alloc_realloc(allocator, ptr, size) __he_alloc_realloc((allocator), ptr, size, __FILE__, __LINE__, __FUNCTION__ )
#define he_alloc_free(allocator, ptr) __he_alloc_free((allocator),ptr, __FILE__, __LINE__, __FUNCTION__ )

#define HE_STACK_ALLOCATOR_BASE \
  struct he_allocator* fallback; \
  size_t cursor_last, cursor_current;

struct stack_allocator_buf {
  HE_STACK_ALLOCATOR_BASE   
  uint8_t data[];
};

struct stack_allocator_user {
  void* alloc;
  size_t size;
};


static void *_stack_malloc(void *user, size_t size, const char *f, int l,
                           const char *sf) {
  struct stack_allocator_user *header = (struct stack_allocator_user  *)user;
  struct stack_allocator_buf *alloc = header->alloc;
  void *res = (alloc->data + alloc->cursor_current) + size;
  if (alloc->cursor_current + size <= header->size) {
    alloc->cursor_last = alloc->cursor_current;
    alloc->cursor_current += size;
    return res;
  }
  return alloc->fallback->malloc(alloc->fallback->user, size, f, l, sf);
}

static void *_stack_calloc(void *user, size_t count, size_t size, const char *f,
                           int l, const char *sf) {
  struct stack_allocator_user *header = (struct stack_allocator_user  *)user;
  struct stack_allocator_buf *alloc = header->alloc;
  void *res = (alloc->data + alloc->cursor_current) + (count * size);
  if (alloc->cursor_current + (count * size) <= header->size) {
    alloc->cursor_last = alloc->cursor_current;
    alloc->cursor_current += (count * size);
    memset(alloc->data + alloc->cursor_last, 0, (count * size));
    return res;
  }
  return alloc->fallback->calloc(alloc->fallback->user, count, size, f, l,
                                   sf);
}

static void *_stack_realloc(void *user, void *ptr, size_t size, const char *f,
                            int l, const char *sf) {
  struct stack_allocator_user *header = (struct stack_allocator_user  *)user;
  struct stack_allocator_buf *alloc = header->alloc;
  if (ptr == alloc->data + alloc->cursor_last) {
    if (alloc->cursor_last + size > header->size)
      return alloc->fallback->malloc(alloc->fallback->user, size, f, l, sf);
    alloc->cursor_current = alloc->cursor_last + size;
    return ptr;
  }
  return _stack_malloc(user, size, f, l, sf);
}

static void _stack_free(void* user,void *ptr, const char *f, int l, const char *sf) {
  struct stack_allocator_user *header = (struct stack_allocator_user  *)user;
  struct stack_allocator_buf *alloc = header->alloc;
  if ((uint8_t *)ptr >= alloc->data &&
      (uint8_t *)ptr <= (alloc->data + header->size))
    return;

  return alloc->fallback->free(alloc->fallback->user, ptr, f, l, sf); 
}

#define reset_stack_allocator(alloc) do \
{ \
 alloc.cursor_last = 0; \
 alloc.cursor_current = 0;\
} while(false);


static inline struct he_allocator fixed_stack_allocator(struct stack_allocator_user* user) {
  struct he_allocator alloc = {
    user, _stack_malloc, _stack_calloc, _stack_realloc, _stack_free 
  };
  return alloc;
}
