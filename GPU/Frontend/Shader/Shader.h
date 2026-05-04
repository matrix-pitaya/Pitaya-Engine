#pragma once

#include<Core/Utils/StateFlags.h>
#include<GPU/Common/ShaderType.h>
#include<GPU/Common/ShaderVariableType.h>
#include<GPU/Common/Identifier.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	struct Shader
	{
		struct Factory
		{
			friend class RHIDevice;
		private:
			static Shader Create(const char* vertexSource, const char* fragmentSource);
			static Shader Create(const char* vertexSource, const char* fragmentSource, const char* geometrySource);
			static void Destroy(Shader);
		};
	
		inline static const ShaderType VF = static_cast<ShaderType>(Pitaya::Core::CombineMask<ShaderType>(
			ShaderType::Vertex, ShaderType::Fragment));
		inline static const ShaderType VFG = static_cast<ShaderType>(Pitaya::Core::CombineMask<ShaderType>(
			ShaderType::Vertex, ShaderType::Geometry, ShaderType::Fragment));

		Pitaya::GPU::Identifier<Shader> Id = 0;
	};
}
