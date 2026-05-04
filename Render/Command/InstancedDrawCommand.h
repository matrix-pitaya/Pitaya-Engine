#pragma once

#include<Core/Container/SlotMap.h>
#include<Render/Common/RenderCommandType.h>

namespace Pitaya::GPU
{
    class Texture;
    class Texture2D;
    class Texture2DArray;
    class TextureCubemap;

    class Shader;

    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;

    class UniformBuffer;
    class ShaderStorageBuffer;

    class FrameBuffer;
}

namespace Pitaya::Render
{
    struct InstancedDrawCommand
    {
        inline static constexpr const RenderCommandType Type = RenderCommandType::InstanceDraw;

        Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle VertexArrayHandle;
        Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle ShaderHandle;
        Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle TextureHandles[static_cast<size_t>(Pitaya::GPU::TextureUsage::Unknown)] = {};

        //渲染状态
        bool DepthTest = true;
        bool Blend = false;
        bool CullFace = true;

        //索引缓冲参数
        uint32_t IndexCount = 0;
        uint32_t BaseIndex = 0;
        uint32_t BaseVertex = 0;

        //材质ID
        uint32_t MaterialId = 0;

        //实例化核心数据
        uint32_t InstanceCount = 0;  //实例数量
        uint32_t BaseInstance = 0;   //在SSBO中的起始偏移量
    };
}
