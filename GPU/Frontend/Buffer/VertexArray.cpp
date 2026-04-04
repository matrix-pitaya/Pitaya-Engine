#include<GPU/Frontend/Buffer/VertexArray.h>
#include<GPU/Backend/OpenGL/Buffer/OpenGLVertexArray.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>

Pitaya::GPU::VertexArray* Pitaya::GPU::VertexArray::Create()
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return new OpenGLVertexArray();

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create GPU vertex aray");
			return nullptr;
	}
}
