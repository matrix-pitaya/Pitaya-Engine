#pragma once

#include<GPU/Common/GPUObjectType.h>

#include<cstdint>
#include<functional>
#include<type_traits>

namespace Pitaya::GPU
{
    template<GPUObjectType T>
	struct Identifier
	{
        constexpr Identifier(uint64_t value = 0) noexcept
            : value(value) {}

        template<GPUObjectType U>
        constexpr Identifier(const Identifier<U>&) = delete;
        template<GPUObjectType U>
        Identifier& operator=(const Identifier<U>&) = delete;
        template<GPUObjectType U>
        constexpr Identifier(Identifier<U>&&) = delete;
        template<GPUObjectType U>
        Identifier& operator=(Identifier<U>&&) = delete;

        template<GPUObjectType U>
        constexpr Identifier<U> As() const noexcept
        {
            return Identifier<U>(this->value);
        }

        constexpr bool operator==(const Identifier& other) const noexcept
        {
            return value == other.value;
        }
        constexpr bool operator!=(const Identifier& other) const noexcept
        {
            return value != other.value;
        }
        constexpr bool operator<(const Identifier& other) const noexcept
        {
            return value < other.value;
        }

        template<GPUObjectType U>
        constexpr bool operator==(const Identifier<U>&) const noexcept = delete;
        template<GPUObjectType U>
        constexpr bool operator!=(const Identifier<U>&) const noexcept = delete;
        template<GPUObjectType U>
        constexpr bool operator<(const Identifier<U>&) const noexcept = delete;
        

        explicit constexpr operator bool() const noexcept
        {
            return value != 0;
        }
        constexpr operator uint64_t() const noexcept
        {
            return value;
        }
        constexpr Identifier& operator=(uint64_t value) noexcept
        {
            this->value = value;
            return *this;
        }

    public:
        static const Identifier Invalid;

    private:
        uint64_t value = 0;
	};

    template <GPUObjectType T>
    inline constexpr const Identifier<T> Identifier<T>::Invalid = Identifier(0);
}

namespace std
{
    template <GPUObjectType T>
    struct hash<::Pitaya::GPU::Identifier<T>>
    {
        size_t operator()(const ::Pitaya::GPU::Identifier<T>& GPUID) const noexcept
        {
            return std::hash<uint64_t>{}(GPUID);
        }
    };
}
