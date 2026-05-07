#pragma once

#include<Core/Color/Color.h>
#include<Core/Math/Rect.h>
#include<Core/Camera/CameraSnapshot.h>
#include<Core/Container/SlotMap.h>
#include<Render/Common/RenderCommandType.h>
#include<Render/Common/PostProcessSetting.h>
#include<GPU/Common/GPUObjectType.h>
#include<glm.hpp>

namespace Pitaya::Render
{
	struct BeginPassCommand
	{
		inline static constexpr const RenderCommandType Type = RenderCommandType::BeginPass;

		Pitaya::Core::CameraSnapshot CameraSnapshot;
		Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle SceneFrameBufferHandle;
		Pitaya::Core::Color ClearColor = Pitaya::Core::Color::SkyBlue;
		Pitaya::Core::Rect Rect;
		uint32_t LightDataOffset = 0;
		uint32_t LightCount = 0;
		bool ClearDepth = true;
		bool ClearStencil = true;
	};
}
