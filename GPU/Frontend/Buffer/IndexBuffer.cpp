#include<GPU/Frontend/Buffer/IndexBuffer.h>
#include<GPU/Backend/OpenGL/Buffer/OpenGLIndexBuffer.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Core/Allocate/Allocate.h>

Pitaya::GPU::IndexBuffer* Pitaya::GPU::IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return PITAYA_NEW(OpenGLIndexBuffer, indices, count);

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create GPU index buffer");
			return nullptr;
	}
}
