#pragma once

#include<Render/Common/PostProcessStep.h>

namespace Pitaya::Render
{
    struct PostProcessSetting
    {
        inline static constexpr const uint8_t MAX_STEPS = 8;

        PostProcessStep Steps[MAX_STEPS] = {};
        uint32_t StepCount = 0;

        inline void AddStep(const PostProcessStep& step)   //追加到末尾
        {
            if (StepCount >= MAX_STEPS) { return; }
            Steps[StepCount++] = step;
        }
        inline void AddStep(const PostProcessStep& step, uint32_t index) //插入到指定位置，越界则追加
        {
            if (StepCount >= MAX_STEPS) { return; }
            if (index > StepCount) { AddStep(step); return; }
            for (uint8_t i = StepCount; i > index; --i) //将 index 及其之后的元素向后移动一位
            {
                Steps[i] = Steps[i - 1];
            }
            Steps[index] = step;
            StepCount++;
        }
        inline void RemoveStep()    //移除最后一个
        {
            if (StepCount == 0) { return; }
            StepCount--;
        }
        inline void RemoveStep(uint32_t index)   //移除指定位置 越界则移除最后一个
        {
            if (index >= StepCount) { RemoveStep(); return; }
            for (uint8_t i = index; i < StepCount - 1; ++i)     //将 index 之后的元素全部向前移动一位
            {
                Steps[i] = Steps[i + 1];
            }
            StepCount--;
        }
        inline void SwapSteps(uint32_t indexA, uint32_t indexB) noexcept  //交换两个位置
        {
            if (indexA < StepCount && indexB < StepCount)
            {
                std::swap(Steps[indexA], Steps[indexB]);
            }
        }
        inline void Clear() noexcept    //清空所有步骤
        {
            StepCount = 0;
        }
    };
}