#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Common/PixelFormat.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	struct Texture2D
	{
		struct Factory
		{
			friend class RHIDevice;
		private:
			static Texture2D Create(const void* data, int width, int height, PixelFormat format, bool isGenerateMipmap, bool isNearest);
			static void Destroy(Texture2D);
		};

		Pitaya::GPU::Identifier<Texture2D> Id = 0;
		Pitaya::GPU::Identifier<Texture2D> SamplerId = 0;
	};
}
