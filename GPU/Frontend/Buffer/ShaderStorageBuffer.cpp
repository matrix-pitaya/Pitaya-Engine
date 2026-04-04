#include<GPU/Frontend/Buffer/ShaderStorageBuffer.h>
#include<GPU/Backend/OpenGL/Buffer/OpenGLShaderStorageBuffer.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>

Pitaya::GPU::ShaderStorageBuffer* Pitaya::GPU::ShaderStorageBuffer::Create(uint32_t size, uint32_t bindingPoint)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return new OpenGLShaderStorageBuffer(size, bindingPoint);

		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create GPU shader storage buffer");
			return nullptr;
	}
}
