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
            static FrameBuffer Create();
            static FrameBuffer Create(Pitaya::GPU::FrameBufferSpecification spec);
            static void Destroy(FrameBuffer);
        };

        Pitaya::GPU::Identifier<FrameBuffer> Id = 0;
        Pitaya::GPU::Identifier<FrameBuffer> InternalId = 0;
        Pitaya::GPU::Identifier<Texture2D> ColorAttachmentId = 0;
        Pitaya::GPU::Identifier<Texture2D> InternalColorAttachmentId = 0;
        Pitaya::GPU::Identifier<Texture2D> DepthAttachmentId = 0;
        Pitaya::GPU::FrameBufferSpecification Specification;
    };
}
