#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Common/PixelFormat.h>

namespace Pitaya::GPU
{
    class RHIDevice;
    struct Texture2DArray
    {
        struct Factory
        {
            friend class RHIDevice;
        private:
            static Texture2DArray Create(int width, int height, int layers, PixelFormat format);
            static void Destroy(Texture2DArray);
        };

        Pitaya::GPU::Identifier<Texture2DArray> Id = 0;
	    Pitaya::GPU::Identifier<Texture2DArray> SamplerId = 0;
    };
}
