#ifndef HE_SCRATCH_ALLOCATOR_H_INCLUDED
#define HE_SCRATCH_ALLOCATOR_H_INCLUDED

#include "he/core/arch.h"
#include "he/core/types.h"
#include "he/core/alloc.h"

struct HeScratchAllocator {
	struct he_allocator* alloctor;
	struct HeAllocScratchBlock *freeBlocks;
	struct HeAllocScratchBlock *current;
	size_t pos;
	size_t blockSize;
	uint16_t alignment;
};

struct HeScratchAllocDesc {
	size_t blockSize;
	size_t alignment;
};

#ifdef __cplusplus
extern "C" {
#endif

void heInitScratchAllocator(struct HeScratchAllocator *alloc,
                            struct HeScratchAllocDesc *desc);
void heFreeScratchAllocator(struct HeScratchAllocator *alloc);
void heResetScratchAllocator(struct HeScratchAllocator *alloc);
void *heScratchAlloc(struct HeScratchAllocator *alloc, size_t size);

#ifdef __cplusplus
}
#endif

#endif
