#pragma once

#include<cstdint>
#include<functional>
#include<type_traits>

namespace Pitaya::GPU
{
    class Texture;
    class Texture2D;
    class Texture2DArray;
    class TextureCubemap;

    class Shader;

    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;

    class UniformBuffer;
    class ShaderStorageBuffer;

    class FrameBuffer;
}

namespace Pitaya::GPU
{
    template<typename T>
	struct Identifier
	{
        static_assert(
            std::is_same_v<T, Pitaya::GPU::Texture> ||
            std::is_same_v<T, Pitaya::GPU::Texture2D> ||
            std::is_same_v<T, Pitaya::GPU::TextureCubemap> ||
            std::is_same_v<T, Pitaya::GPU::Shader> ||
            std::is_same_v<T, Pitaya::GPU::VertexArray> ||
            std::is_same_v<T, Pitaya::GPU::VertexBuffer> ||
            std::is_same_v<T, Pitaya::GPU::Texture2DArray> ||
            std::is_same_v<T, Pitaya::GPU::IndexBuffer> ||
            std::is_same_v<T, Pitaya::GPU::UniformBuffer> ||
            std::is_same_v<T, Pitaya::GPU::ShaderStorageBuffer> ||
            std::is_same_v<T, Pitaya::GPU::FrameBuffer>,
            "Identifier<T>: T must be GPU Handle Type");

        constexpr Identifier(uint64_t value = 0) noexcept
            : value(value) {}

        template<typename U>
        constexpr Identifier(const Identifier<U>&) = delete;
        template<typename U>
        Identifier& operator=(const Identifier<U>&) = delete;
        template<typename U>
        constexpr Identifier(Identifier<U>&&) = delete;
        template<typename U>
        Identifier& operator=(Identifier<U>&&) = delete;

        template<typename U>
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

        template<typename U>
        constexpr bool operator==(const Identifier<U>&) const noexcept = delete;
        template<typename U>
        constexpr bool operator!=(const Identifier<U>&) const noexcept = delete;
        template<typename U>
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

    template <typename T>
    inline constexpr const Identifier<T> Identifier<T>::Invalid = Identifier(0);
}

namespace std
{
    template <typename T>
    struct hash<::Pitaya::GPU::Identifier<T>>
    {
        size_t operator()(const ::Pitaya::GPU::Identifier<T>& GPUID) const noexcept
        {
            return std::hash<uint64_t>{}(GPUID);
        }
    };
}
