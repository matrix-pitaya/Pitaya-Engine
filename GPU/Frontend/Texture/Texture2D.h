#pragma once

#include<GPU/Common/Identifier.h>
#include<GPU/Frontend/Texture/Texture.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	class Texture2D : public Texture
	{
		friend class Pitaya::GPU::RHIDevice;
	public:
		Texture2D(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest) {}
		virtual ~Texture2D() override = default;

	public:
		virtual Identifier<Texture2D> GetGPUIdentifier() const = 0;

	private:
		static Texture2D* Create(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest);
	};
}
