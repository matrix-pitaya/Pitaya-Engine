#pragma once

#include<Core/PassKey/PassKey.h>
#include<Core/Container/SlotMap.h>
#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<GPU/Frontend/Buffer/IndexBuffer.h>
#include<vector>

namespace Pitaya::GPU
{
    class RHIDevice;
    struct VertexArray
    {
        struct Factory
        {
            friend class RHIDevice;
        private:
            static VertexArray Create();
            static void Destroy(VertexArray);
        };
        struct Operate
        {
            friend class RHIDevice;
        private:
            static void Link(VertexArray, VertexBuffer, IndexBuffer);
        };

        Pitaya::GPU::Identifier<VertexArray> Id = 0;
        Pitaya::Core::SlotMap<VertexBuffer>::Handle VertexBufferHandle;
        Pitaya::Core::SlotMap<IndexBuffer>::Handle IndexBufferHandle;
    };
}
