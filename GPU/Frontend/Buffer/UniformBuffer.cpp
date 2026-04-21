#include<GPU/Frontend/Buffer/UniformBuffer.h>
#include<GPU/Backend/OpenGL/Buffer/OpenGLUniformBuffer.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Core/Allocate/Allocate.h>

Pitaya::GPU::UniformBuffer* Pitaya::GPU::UniformBuffer::Create(uint32_t size, uint32_t bindingPoint)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return PITAYA_NEW(OpenGLUniformBuffer, size, bindingPoint);

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create GPU uniform buffer");
			return nullptr;
	}
}
