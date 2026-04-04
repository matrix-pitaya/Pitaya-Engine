#pragma once

#include<Render/Common/PostProcessType.h>

namespace Pitaya::Render
{
    // 在 C++ 侧， 不能直接用 C++ bool (1字节)，必须用 int32_t (4字节) true 用 1 表示，false 用 0 表示 
    struct alignas(16) BloomParams
    {
        inline static constexpr const PostProcessType Type = PostProcessType::Bloom;

        float Threshold = 1.0f;       
        float Intensity = 0.5f;        
    };

    struct alignas(16) ToneMappingParams
    {
        inline static constexpr const PostProcessType Type = PostProcessType::ToneMapping;

        float Exposure = 1.0f;
    };

    struct alignas(16) GammaCorrectionParams
    {
        inline static constexpr const PostProcessType Type = PostProcessType::GammaCorrection;

        float Gamma = 2.2f;    
    };
}
