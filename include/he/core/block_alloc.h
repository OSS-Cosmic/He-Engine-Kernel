#ifndef HE_BLOCK_ALLOC_H_
#define HE_BLOCK_ALLOC_H_

#include "he/core/types.h"
#include "he/core/arch.h"

struct HeChunk {
  struct HeChunk* next;
};

struct HeBlockHeader {
  struct HeBlockHeader *prev;
  struct HeBlockHeader *next;
};

struct HeBlockAllocator {
	struct HeChunk* nextChunk; 
	struct HeBlockHeader* freeBlocks;
	uint16_t numElements;
	uint16_t alignment;
	size_t elementSize;
};

struct HeBlockAllocatorDesc {
	uint16_t numElements;
	uint16_t alignment;
	size_t elementSize;
};

#ifdef __cplusplus
extern "C" {
#endif

void heInitBlockAllocator(struct HeBlockAllocatorDesc *desc,
                          struct HeBlockAllocator *allocator);

void *heBlockAllocatorAlloc(struct HeBlockAllocator *allocator);
void *heBlockAllocatorFree(struct HeBlockAllocator *allocator, void *alloc);

#ifdef __cplusplus
}
#endif


#endif

