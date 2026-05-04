#pragma once

#include<type_traits>

namespace Pitaya::GPU
{
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

template <typename T>
concept GPUObjectType =
    std::is_same_v<T, Pitaya::GPU::Texture2D> ||
    std::is_same_v<T, Pitaya::GPU::TextureCubemap> ||
    std::is_same_v<T, Pitaya::GPU::Shader> ||
    std::is_same_v<T, Pitaya::GPU::VertexArray> ||
    std::is_same_v<T, Pitaya::GPU::VertexBuffer> ||
    std::is_same_v<T, Pitaya::GPU::Texture2DArray> ||
    std::is_same_v<T, Pitaya::GPU::IndexBuffer> ||
    std::is_same_v<T, Pitaya::GPU::UniformBuffer> ||
    std::is_same_v<T, Pitaya::GPU::ShaderStorageBuffer> ||
    std::is_same_v<T, Pitaya::GPU::FrameBuffer>;
