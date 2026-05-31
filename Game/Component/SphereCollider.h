#pragma once

namespace Pitaya::Game
{
    struct SphereCollider
    {
    public:
        inline float GetRadius() const noexcept
        {
            return radius;
        }
        inline bool GetIsTrigger() const noexcept
        {
            return isTrigger;
        }

    public:
        inline void SetRadius(float r) noexcept
        {
            radius = r;
        }
        inline void SetIsTrigger(bool trigger) noexcept
        {
            isTrigger = trigger;
        }

    private:
        float radius = 0.5f;
        bool isTrigger = false;
    };
}
