#pragma once

#include<Renderer/Runtime/Component/Camera/CameraSnapshot.h>

#include<glm.hpp>

namespace Pitaya::Engine::Renderer
{
	struct BeginPassCommand
	{
		CameraSnapshot Snapshot;
		uint32_t RenderTargetID = 0;
		glm::vec4 ClearColor;
		bool ClearDepth = true;
		bool ClearStencil = true;
	};
}