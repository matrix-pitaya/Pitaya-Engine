#pragma once

#include<GPU/Common/Identifier.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	struct Texture2D
	{
		struct Factory
		{
			friend class RHIDevice;
		private:
			static Texture2D Create(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest);
			static void Destroy(Texture2D);
		};

		Identifier<Texture2D> Id = 0;
	};
}
