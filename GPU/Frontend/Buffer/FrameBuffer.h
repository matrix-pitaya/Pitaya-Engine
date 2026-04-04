#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Common/FrameBufferSpecification.h>

namespace Pitaya::GPU
{
    class RHIDevice;
    class FrameBuffer
    {
        friend class Pitaya::GPU::RHIDevice;
    public:
        FrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec)
            :specification(spec) {}
        virtual ~FrameBuffer() = default;

    public:
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual Identifier<FrameBuffer> GetGPUIdentifier() const = 0;
        virtual Identifier<FrameBuffer> GetInternalGPUIdentifier() const = 0;
        virtual Identifier<Texture2D> GetColorAttachmentGPUIdentifier() const = 0;
        virtual Identifier<Texture2D> GetDepthAttachmentGPUIdentifier() const = 0;

    public:
        inline FrameBufferSpecification GetSpecification() const noexcept
        {
            return specification;
        }

    private:
        static Pitaya::GPU::FrameBuffer* Create(const Pitaya::GPU::FrameBufferSpecification& spec);
    
    protected:
        FrameBufferSpecification specification;
    };
}
