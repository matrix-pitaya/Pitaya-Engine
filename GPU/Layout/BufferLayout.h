#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Layout/BufferElement.h>

#include<vector>

namespace Pitaya::GPU
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
        BufferLayout(const std::vector<BufferElement>& elements)
            : elements(elements)
        {
            CalculateOffsetsAndStride();
        }
        ~BufferLayout() = default;

    public:
        inline uint32_t GetStride() const noexcept
        { 
            return stride;
        }
        inline const std::vector<BufferElement>& GetElements() const noexcept 
        { 
            return elements;
        }

    private:
        inline void CalculateOffsetsAndStride() noexcept
        {
            stride = 0;
            size_t offset = 0;
            for (auto& element : elements) 
            {
                element.Offset = offset;
                offset += element.Size;
                stride += element.Size;
            }
        }

    private:
        std::vector<Pitaya::GPU::BufferElement> elements;
        uint32_t stride = 0;
    };
}
