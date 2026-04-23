#pragma once

#include<Core/Identifier/GUID.h>
#include<GPU/Frontend/Shader/Shader.h>

namespace Pitaya::Asset
{
	struct Shader
	{
		Pitaya::GPU::Identifier<Pitaya::GPU::Shader> ID = 0;
		Pitaya::GPU::Shader::Location Location = -1;
	
		//Shader 13-16
		inline static constexpr const Pitaya::Core::GUID Default = Pitaya::Core::GUID("00000000-0000-0000-0001-000000000000");
	};
}
