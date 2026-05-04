#pragma once

#include<GPU/Common/Identifier.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	struct TextureCubemap
	{
		struct Factory
		{
			friend class RHIDevice;
		private:
			static TextureCubemap Create(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest);
			static void Destroy(TextureCubemap);
		};

		Pitaya::GPU::Identifier<TextureCubemap> Id = 0;
	};
}
