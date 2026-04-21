#include<GPU/Frontend/Buffer/VertexArray.h>
#include<GPU/Backend/OpenGL/Buffer/OpenGLVertexArray.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Core/Allocate/Allocate.h>

Pitaya::GPU::VertexArray* Pitaya::GPU::VertexArray::Create(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return PITAYA_NEW(OpenGLVertexArray, passkey);

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create GPU vertex aray");
			return nullptr;
	}
}
