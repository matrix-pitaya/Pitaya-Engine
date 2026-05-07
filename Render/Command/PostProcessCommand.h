#pragma once

#include<Core/Math/Rect.h>
#include<Core/Container/SlotMap.h>
#include<Render/Common/RenderCommandType.h>
#include<Render/Common/PostProcessStep.h>
#include<GPU/Common/GPUObjectType.h>

namespace Pitaya::Render
{
    struct PostProcessCommand
    {
        inline static constexpr const RenderCommandType Type = RenderCommandType::PostProcess;
        Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle WriteFrameBufferHandle;  //最终帧缓冲区附件
        Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle ReadFrameBufferHandle; 
        Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle ProcessShaderHandle;     
        Pitaya::Render::PostProcessStep PostProcessStep;

        //用于多采用解析
        bool ResolveMSAA = false;
        glm::uvec2 ResolveSize = { 0,0 };
        Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle ResolveFrameBufferHandle;   //From MSAA Framebuffer Resolve To Normal Internal Framebuffer
    };
}