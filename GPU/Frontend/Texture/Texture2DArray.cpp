#include<GPU/Frontend/Texture/Texture2DArray.h>
#include<GPU/Backend/OpenGL/Texture/OpenGLTexture2DArray.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Core/Allocate/Allocate.h>

Pitaya::GPU::Texture2DArray* Pitaya::GPU::Texture2DArray::Create(int width, int height, int layers, bool isDepth)
{
    switch (Pitaya::Config::GetRenderAPI())
    {
        case Pitaya::Render::API::OpenGL:
            return PITAYA_NEW(OpenGLTexture2DArray, width, height, layers, isDepth);

        case Pitaya::Render::API::Unknown:
            Pitaya::Log::Error("use unknown api to create gpu texture2DArray");
            return nullptr;
    }
}