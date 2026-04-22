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
        inline static constexpr const uint8_t UniformBufferBytes = 62;

        std::byte ShaderParams[UniformBufferBytes] = {};
        PostProcessType Type = PostProcessType::Unknown;
        uint8_t Size = 0;

        template<typename T>
        inline void SetParams(const T& data)
        {
            static_assert(std::is_same_v<T, Pitaya::Render::BloomParams> ||
                std::is_same_v<T, Pitaya::Render::ToneMappingParams> ||
                std::is_same_v<T, Pitaya::Render::GammaCorrectionParams>,
                "Unknow Asset!");
            static_assert(sizeof(T) <= UniformBufferBytes, "Requested number of bytes exceeds the maximum");

            std::memcpy(ShaderParams, &data, sizeof(T));
            Size = sizeof(T);
            Type = T::Type;
        }
    };
}