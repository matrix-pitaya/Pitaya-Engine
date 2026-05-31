#pragma once

#include<glm.hpp>

namespace Pitaya::Game
{
    struct BoxCollider
    {
    public:
        inline glm::vec3 GetHalfExtents() const noexcept
        {
            return halfExtents;
        }
        inline bool GetIsTrigger() const noexcept
        {
            return isTrigger;
        }

    public:
        inline void SetHalfExtents(glm::vec3 extents) noexcept
        {
            halfExtents = extents;
        }
        inline void SetIsTrigger(bool trigger) noexcept
        {
            isTrigger = trigger;
        }

    private:
        glm::vec3 halfExtents = glm::vec3(0.5f);
        bool isTrigger = false;
    };
}
