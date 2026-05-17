#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Common/PixelFormat.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	struct TextureCubemap
	{
		struct Factory
		{
			friend class RHIDevice;
		private:
			// 用 6 面图像数据创建 cubemap（导入路径）
			static TextureCubemap Create(const void** datas, int* widths, int* heights, PixelFormat format, bool isGenerateMipmap, bool isNearest);
			// 创建空 cubemap（含 mip 链），用于 IBL 预计算时作为渲染目标
			static TextureCubemap Create(int size, int mipLevels, PixelFormat format);
			static void Destroy(TextureCubemap);
		};

		Pitaya::GPU::Identifier<TextureCubemap> Id = 0;
	};
}
