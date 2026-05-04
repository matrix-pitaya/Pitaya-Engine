#pragma once

#include<GPU/Common/Identifier.h>
#include<cstdint>

namespace Pitaya::GPU
{
    class RHIDevice;
    struct ShaderStorageBuffer
    {
        struct Factory
        {
            friend class RHIDevice;
        private:
            static ShaderStorageBuffer Create(uint32_t size, uint32_t bindingPoint);
            static void Destroy(ShaderStorageBuffer);
        };

        Pitaya::GPU::Identifier<ShaderStorageBuffer> Id = 0;
        uint32_t size = 0;
        uint32_t bindingPoint = 0;
    };
}
