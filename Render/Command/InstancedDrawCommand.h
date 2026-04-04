#pragma once

#include<GPU/Common/Identifier.h>

namespace Pitaya::Render
{
    struct InstancedDrawCommand
    {
        inline static constexpr const RenderCommandType Type = RenderCommandType::InstanceDraw;

        Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> VertexArray = 0;
        Pitaya::GPU::Identifier<Pitaya::GPU::Shader> Shader = 0;
        Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> Textures[static_cast<size_t>(Pitaya::GPU::TextureUsage::Unknown)] = {};

        //渲染状态
        bool DepthTest = true;
        bool Blend = false;

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
