#pragma once

#include<Core/Container/SlotMap.h>
#include<GPU/Frontend/Texture/Texture2D.h>
#include<GPU/Frontend/Texture/TextureCubemap.h>
#include<glm.hpp>

namespace Pitaya::Render
{
	struct SceneInfoSetup
	{
		glm::vec4 AmbientColor = glm::vec4(1.0f);
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle EnvCubemapHandle;
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle IrradianceHandle;
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle PrefilteredHandle;
		Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle BRDFLUTHandle;
		float DeltaTime = 0.0f;
	};
}
