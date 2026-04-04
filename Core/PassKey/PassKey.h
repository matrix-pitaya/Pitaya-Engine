#pragma once

namespace Pitaya::Core
{
    template <typename T>
    struct PassKey 
    {
        friend T;
    private:
        PassKey() = default;
    };
}