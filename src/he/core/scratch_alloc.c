
#include "he/core/scratch_alloc.h"

struct HeAllocScratchBlock {
	struct HeAllocScratchBlock *next;
	size_t len;
	void *data[];
};
static size_t __scratchDefaultBlockLen( struct he_scratch_allocator *alloc )
{
	return alloc->blockSize + alloc->alignment;
}

void init_scratch_allocator( struct he_scratch_allocator *alloc, struct he_scrach_alloc_desc *desc )
{
	assert( alloc );
	assert( desc );
	memset( alloc, 0, sizeof( struct he_scratch_allocator ) );
	alloc->blockSize = desc->blockSize;
	alloc->alignment = HE_MAX( desc->alignment, sizeof( uint16_t ) );
}

void reset_scratch_allocator( struct he_scratch_allocator *alloc )
{
	alloc->freeBlocks = NULL;
	struct HeAllocScratchBlock *blk = alloc->freeBlocks;
	while( blk ) {
		struct HeAllocScratchBlock *current = blk; // we only reset the scractch allocator to the current block which is the default blockSize
		blk = blk->next;
		if( current == alloc->current ) {
			current->next = NULL;
			alloc->freeBlocks = current;
			continue;
		}
		free( current );
	}
	alloc->pos = 0;
}

void free_scratch_allocator( struct he_scratch_allocator *alloc )
{
	struct HeAllocScratchBlock *blk = alloc->freeBlocks;
	while( blk ) {
		struct HeAllocScratchBlock *current = blk;
		blk = blk->next;
		free( current );
	}
	alloc->freeBlocks = NULL;
	alloc->current = NULL;
}

void *scratch_alloc( struct he_scratch_allocator *alloc, size_t size )
{
	const size_t reqSize = HE_ALIGN_TO( size, alloc->alignment );
	if( reqSize > alloc->blockSize ) {
		const size_t len = size + alloc->alignment; 
		struct HeAllocScratchBlock *block = (struct HeAllocScratchBlock *)malloc( sizeof( struct HeAllocScratchBlock ) + len );
		block->len = len;
		
		block->next = alloc->freeBlocks;
		alloc->freeBlocks = block;
		
		const size_t offset = ( (size_t)block->data ) % alloc->alignment;
		return ( (uint8_t *)block->data ) + offset;
	}

  if( alloc->current == NULL || ( alloc->pos + reqSize ) > alloc->current->len ) {
		
		const size_t defaultScratchLen = __scratchDefaultBlockLen(alloc); 
		struct HeAllocScratchBlock *block = (struct HeAllocScratchBlock *)malloc( sizeof( struct HeAllocScratchBlock ) + defaultScratchLen );
		block->len = defaultScratchLen;

		alloc->pos = ( (size_t)block->data ) % alloc->alignment;

		block->next = alloc->freeBlocks;
		alloc->freeBlocks = block;
		alloc->current = block;
	}
	void *result = ( (uint8_t *)alloc->current->data ) + alloc->pos;
	alloc->pos += reqSize;
	return result;
}
