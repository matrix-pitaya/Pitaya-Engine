#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Frontend/Texture/Texture.h>

namespace Pitaya::GPU
{
    class RHIDevice;
    class Texture2DArray : public Texture
    {
        friend class Pitaya::GPU::RHIDevice;
    public:
        Texture2DArray(int width, int height, int layers, bool isDepth) {}
        virtual ~Texture2DArray() override = default;

    public:
        virtual Identifier<Texture2DArray> GetGPUIdentifier() const = 0;

    private:
        static Texture2DArray* Create(int width, int height, int layers, bool isDepth);
    };
}