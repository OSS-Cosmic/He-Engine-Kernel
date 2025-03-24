#ifndef RI_POGO_BUFFER_H
#define RI_POGO_BUFFER_H

#include "he/renderer/renderer.h"

struct RI_PogoBuffer {
	struct RIDescriptor_s pogoAttachment[2];
	uint16_t attachmentIndex;
};

VkImageMemoryBarrier2 VK_RI_PogoShaderMemoryBarrier2( VkImage image, bool initial );
VkImageMemoryBarrier2 VK_RI_PogoAttachmentMemoryBarrier2( VkImage image, bool initial );

void RI_PogoBufferToggle( struct RIGPUDevice *device, struct RI_PogoBuffer *pogo, struct RICmd *handle );

static inline struct RIDescriptor *RI_PogoBufferAttachment( struct RI_PogoBuffer *pogo )
{
	return pogo->pogoAttachment + pogo->attachmentIndex;
}

static inline struct RIDescriptor *RI_PogoBufferShaderResource( struct RI_PogoBuffer *pogo )
{
	return pogo->pogoAttachment + ( ( pogo->attachmentIndex + 1 ) % 2 );
}

#endif

