#pragma once

#include<Core/Identifier/GUID.h>
#include<Core/Container/SlotMap.h>
#include<GPU/Frontend/Shader/Shader.h>

namespace Pitaya::Asset
{
	struct Shader
	{
		Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle ShaderHandle;
	
		//Shader 13-16
		inline static constexpr const Pitaya::Core::GUID Default = Pitaya::Core::GUID("00000000-0000-0000-0001-000000000000");
	};
}
