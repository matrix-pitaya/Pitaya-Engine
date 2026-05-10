#pragma once

#include<GPU/Common/Identifier.h>
#include<Render/Common/PostProcessType.h>
#include<Render/Common/PostProcessParams.h>

#include<cstdint>
#include<cstddef>

namespace Pitaya::Render
{
    struct PostProcessStep
    {
        inline static constexpr const uint8_t UniformBufferBytes = 16;

        std::byte ShaderParams[UniformBufferBytes] = {};
        PostProcessType Type = PostProcessType::Invalid;
        uint32_t Size = 0;

        template<PostProcessParamsType T>
        inline void SetParams(const T& data)
        {
            static_assert(sizeof(T) <= UniformBufferBytes, "Requested number of bytes exceeds the maximum");

            std::memcpy(ShaderParams, &data, sizeof(T));
            Size = sizeof(T);
            Type = T::Type;
        }
    };
}