#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<GPU/Backend/OpenGL/Buffer/OpenGLVertexBuffer.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Core/Allocate/Allocate.h>

Pitaya::GPU::VertexBuffer* Pitaya::GPU::VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return PITAYA_NEW(OpenGLVertexBuffer, vertices, size);

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create GPU vertex buffer");
			return nullptr;
	}
}
