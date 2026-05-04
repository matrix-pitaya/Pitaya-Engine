#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Common/FrameBufferSpecification.h>

namespace Pitaya::GPU
{
    class RHIDevice;
    struct FrameBuffer
    {
        struct Factory
        {
            friend class RHIDevice;
        private:
            static FrameBuffer Create(Pitaya::GPU::FrameBufferSpecification spec);
            static void Destroy(FrameBuffer);
        };

        Identifier<FrameBuffer> Id = 0;
        Identifier<FrameBuffer> InternalId = 0;
        Identifier<Texture2D> ColorAttachmentId = 0;
        Identifier<Texture2D> InternalColorAttachmentId = 0;
        Identifier<Texture2D> DepthAttachmentId = 0;
        FrameBufferSpecification Specification;
    };
}
