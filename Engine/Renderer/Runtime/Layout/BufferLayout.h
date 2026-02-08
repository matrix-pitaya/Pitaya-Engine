#pragma once

#include<Renderer/Runtime/Layout/BufferElement.h>

#include<vector>

namespace Pitaya::Engine::Renderer
{
    class BufferLayout
    {
    public:
        BufferLayout() = default;
        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : elements(elements)
        {
            CalculateOffsetsAndStride();
        }
        ~BufferLayout() = default;

        inline uint32_t GetStride() const noexcept
        { 
            return stride;
        }
        inline const std::vector<BufferElement>& GetElements() const noexcept 
        { 
            return elements;
        }

    private:
        inline void CalculateOffsetsAndStride()
        {
            size_t offset = 0;
            stride = 0;
            for (auto& element : elements) 
            {
                element.Offset = offset;
                offset += element.Size;
                stride += element.Size;
            }
        }

    private:
        std::vector<Pitaya::Engine::Renderer::BufferElement> elements;
        uint32_t stride = 0;
    };
}