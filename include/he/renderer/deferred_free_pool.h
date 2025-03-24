
#include "he/core/types.h"
#include "he/graphics/graphics.h"

#ifndef HE_DEFERRED_H
#define HE_DEFERRED_H

enum HeDeferredType {
  DEFER_UNKNOWN = 0,
  DEFER_VK_START = 0,
  DEFER_VK_IMAGE,
  DEFER_VK_IMAGEVIEW,
  DEFER_VK_SAMPLER,
  DEFER_VK_VMA_AllOC,
  DEFER_VK_BUFFER,
  DEFER_VK_BUFFER_VIEW,
  DEFER_VK_END,
};

struct HeDeferredFree {
  uint8_t type;
  union {
#if (DEVICE_IMPL_VULKAN)
    VkCommandBuffer vkCmdBuffer;
    VkImage vkImage;
    VkImageView vkImageView;
    VkBuffer vkBuffer;
    VkSampler vkSampler;
    VkBufferView vkBufferView;
    struct VmaAllocation_T *vmaAlloc;
#endif
  };
};

struct HeDeferredFreePool {
  struct HeDeferredFree *free;
};


#endif
