#pragma once

#include<Render/Common/PostProcessStep.h>

namespace Pitaya::Render
{
    struct PostProcessSetting
    {
        inline static constexpr const uint8_t MAX_STEPS = 8;

        PostProcessStep Steps[MAX_STEPS] = {};
        uint8_t StepCount = 0;

        inline void AddPostProcessStep(const PostProcessStep& step)
        {
            if (StepCount >= MAX_STEPS) { return; }
            Steps[StepCount++] = step;
        }
    };
}