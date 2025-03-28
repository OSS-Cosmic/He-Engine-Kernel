#ifndef HE_SCRATCH_ALLOCATOR_H_INCLUDED
#define HE_SCRATCH_ALLOCATOR_H_INCLUDED

#include "he/core/arch.h"
#include "he/core/types.h"
#include "he/core/alloc.h"

struct he_scratch_allocator {
	struct he_allocator* alloctor;
	struct HeAllocScratchBlock *freeBlocks;
	struct HeAllocScratchBlock *current;
	size_t pos;
	size_t blockSize;
	uint16_t alignment;
};

struct he_scrach_alloc_desc {
	size_t blockSize;
	size_t alignment;
};

#ifdef __cplusplus
extern "C" {
#endif

void init_scratch_allocator(struct he_scratch_allocator *alloc,
                            struct he_scrach_alloc_desc *desc);
void free_scratch_allocator(struct he_scratch_allocator *alloc);
void reset_scratch_allocator(struct he_scratch_allocator *alloc);
void *scratch_alloc(struct he_scratch_allocator *alloc, size_t size);

#ifdef __cplusplus
}
#endif

#endif
