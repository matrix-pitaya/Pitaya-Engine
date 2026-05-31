#pragma once

namespace Pitaya::Game
{
    struct CapsuleCollider
    {
    public:
        inline float GetRadius() const noexcept
        {
            return radius;
        }
        inline float GetHeight() const noexcept
        {
            return height;
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
        inline void SetHeight(float h) noexcept
        {
            height = h;
        }
        inline void SetIsTrigger(bool trigger) noexcept
        {
            isTrigger = trigger;
        }

    private:
        float radius = 0.5f;
        float height = 1.0f;
        bool isTrigger = false;
    };
}
