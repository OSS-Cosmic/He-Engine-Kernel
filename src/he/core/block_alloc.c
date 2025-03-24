#include "he/core/block_alloc.h"

void heInitBlockAllocator(struct HeBlockAllocatorDesc *desc,
                          struct HeBlockAllocator *alloc) {
  alloc->alignment = desc->alignment;
  alloc->elementSize = desc->elementSize;
  alloc->numElements = desc->numElements;
}
