#include<GPU/Frontend/Texture/Texture2D.h>
#include<GPU/Backend/OpenGL/Texture/OpenGLTexture2D.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>

Pitaya::GPU::Texture2D* Pitaya::GPU::Texture2D::Create(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return new OpenGLTexture2D(data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create gpu texture2D");
			return nullptr;
	}
}
