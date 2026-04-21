#include<GPU/Frontend/Shader/Shader.h>
#include<GPU/Backend/OpenGL/Shader/OpenGLShader.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Core/Allocate/Allocate.h>

Pitaya::GPU::Shader* Pitaya::GPU::Shader::Create(const char* vertexSource, const char* fragmentSource)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return PITAYA_NEW(OpenGLShader, vertexSource, fragmentSource);

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create gpu shader");
			return nullptr;
	}
}
Pitaya::GPU::Shader* Pitaya::GPU::Shader::Create(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return PITAYA_NEW(OpenGLShader, vertexSource, fragmentSource, geometrySource);

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create gpu shader");
			return nullptr;
	}
}
