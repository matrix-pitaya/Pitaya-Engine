#include<GPU/Frontend/Texture/TextureCubemap.h>
#include<GPU/Backend/OpenGL/Texture/OpenGLTextureCubemap.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Core/Allocate/Allocate.h>

Pitaya::GPU::TextureCubemap* Pitaya::GPU::TextureCubemap::Create(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return PITAYA_NEW(OpenGLTextureCubemap, datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
			
		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create gpu texturecubemap");
			return nullptr;
	}
}
