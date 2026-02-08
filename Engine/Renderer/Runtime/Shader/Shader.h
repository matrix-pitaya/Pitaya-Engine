#pragma once

#include<Enum/Enum.h>

#include<cstdint>

namespace Pitaya::Engine::Renderer
{
	class Shader
	{


	public:
		inline static uint32_t GetShaderDataTypeSize(Pitaya::Engine::Renderer::ShaderDataType type) noexcept
		{
			switch (type)
			{
				case Pitaya::Engine::Renderer::ShaderDataType::Float:    return 4;
				case Pitaya::Engine::Renderer::ShaderDataType::Float2:   return 4 * 2;
				case Pitaya::Engine::Renderer::ShaderDataType::Float3:   return 4 * 3;
				case Pitaya::Engine::Renderer::ShaderDataType::Float4:   return 4 * 4;
				case Pitaya::Engine::Renderer::ShaderDataType::Mat3:     return 4 * 3 * 3;
				case Pitaya::Engine::Renderer::ShaderDataType::Mat4:     return 4 * 4 * 4;
				case Pitaya::Engine::Renderer::ShaderDataType::Int:      return 4;
				case Pitaya::Engine::Renderer::ShaderDataType::Int2:     return 4 * 2;
				case Pitaya::Engine::Renderer::ShaderDataType::Int3:     return 4 * 3;
				case Pitaya::Engine::Renderer::ShaderDataType::Int4:     return 4 * 4;
				case Pitaya::Engine::Renderer::ShaderDataType::Bool:     return 1;
				case Pitaya::Engine::Renderer::ShaderDataType::Unknown:  return 0;
				default:												 return 0;
			}
		}
	};
}