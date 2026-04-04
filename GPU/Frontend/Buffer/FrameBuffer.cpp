#include<GPU/Frontend/Buffer/IndexBuffer.h>
#include<GPU/Backend/OpenGL/Buffer/OpenGLFrameBuffer.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>

Pitaya::GPU::FrameBuffer* Pitaya::GPU::FrameBuffer::Create(const Pitaya::GPU::FrameBufferSpecification& spec)
{
	switch (Pitaya::Config::GetRenderAPI())
	{
		case Pitaya::Render::API::OpenGL:
			return new OpenGLFrameBuffer(spec);
			
		case Pitaya::Render::API::Unknown:
			Pitaya::Log::Error("use unknwon api to create GPU frame buffer");
			return nullptr;
	}
}
