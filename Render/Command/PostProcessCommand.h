#pragma once

#include<Core/Math/Rect.h>
#include<GPU/Common/Identifier.h>
#include<Render/Common/RenderCommandType.h>
#include<Render/Common/PostProcessStep.h>

namespace Pitaya::Render
{
    struct PostProcessCommand
    {
        inline static constexpr const RenderCommandType Type = RenderCommandType::PostProcess;
        Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> WriteFrameBuffer = 0;      //最终帧缓冲区附件
        Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> ReadTexture = 0;	         //场景颜色附件
        Pitaya::GPU::Identifier<Pitaya::GPU::Shader> ProcessShader = 0;	             //场景颜色附件
        Pitaya::Render::PostProcessStep PostProcessStep;

        //用于多采用解析
        bool ResolveMSAA = false;
        glm::uvec2 ResolveSize = { 0,0 };
        Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> ResolveReadFrameBuffer = 0;
        Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> ResolveWriteFrameBuffer = 0;
    };
}