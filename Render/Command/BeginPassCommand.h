#pragma once

#include<Core/Color/Color.h>
#include<Core/Camera/CameraSnapshot.h>
#include<Render/Common/RenderCommandType.h>
#include<Render/Common/RenderTargetSnapshot.h>
#include<Render/Common/PostProcessSetting.h>

#include<glm.hpp>

namespace Pitaya::Render
{
	struct BeginPassCommand
	{
		inline static constexpr const RenderCommandType Type = RenderCommandType::BeginPass;

		Pitaya::Core::CameraSnapshot CameraSnapshot;
		Pitaya::Render::RenderTargetSnapshot RenderTargetSnapshot;
	};
}
