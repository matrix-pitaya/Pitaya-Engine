#pragma once

#include<GPU/Common/Identifier.h>
#include<cstdint>

namespace Pitaya::GPU
{
    class RHIDevice;
    class ShaderStorageBuffer
    {
        friend class Pitaya::GPU::RHIDevice;
    public:
        ShaderStorageBuffer(uint32_t size, uint32_t bindingPoint)
            :size(size), bindingPoint(bindingPoint) {}
        virtual ~ShaderStorageBuffer() = default;

    public:
        virtual void Bind(uint32_t bindingPoint) const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) const = 0;
        virtual Pitaya::GPU::Identifier<ShaderStorageBuffer> GetGPUIdentifier() const = 0;

    public:
        inline uint32_t GetSize() const noexcept
        {
            return size;
        }
        inline uint32_t GetBindingPoint() const noexcept
        {
            return bindingPoint;
        }

    private:
        static Pitaya::GPU::ShaderStorageBuffer* Create(uint32_t size, uint32_t bindingPoint);

    protected:
        uint32_t size = 0;
        uint32_t bindingPoint = 0;
    };
}
