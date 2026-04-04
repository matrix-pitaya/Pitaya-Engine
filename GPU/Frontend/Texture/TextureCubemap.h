#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Frontend/Texture/Texture.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	class TextureCubemap : public Texture
	{
		friend class Pitaya::GPU::RHIDevice;
	public:
		TextureCubemap(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest) {}
		virtual ~TextureCubemap() = default;

	public:
		virtual Pitaya::GPU::Identifier<TextureCubemap> GetGPUIdentifier() const = 0;

	private:
		static Pitaya::GPU::TextureCubemap* Create(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest);
	};
}
