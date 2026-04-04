#pragma once

#include<Core/StateFlags/StateFlags.h>
#include<Core/Camera/CameraSnapshot.h>
#include<Render/Common/RenderTargetSnapshot.h>
#include<Render/Command/PostProcessCommand.h>
#include<Render/Common/PostProcessSetting.h>
#include<Render/Common/RenderLayer.h>

namespace Pitaya::Render
{
	struct RenderPass
	{
		Pitaya::Core::CameraSnapshot CameraSnapshot;
		Pitaya::Render::RenderTargetSnapshot RenderTargetSnapshot;
		Pitaya::Render::PostProcessSetting PostProcessSetting;
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> CullingMask;

		RenderPass(const Pitaya::Core::CameraSnapshot& cameraSnapshot, const Pitaya::Render::RenderTargetSnapshot& renderTargetSnapshot, const Pitaya::Render::PostProcessSetting& setting, Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> cullingMask)
			:CameraSnapshot(cameraSnapshot), RenderTargetSnapshot(renderTargetSnapshot), PostProcessSetting(setting), CullingMask(cullingMask) { }
	};
}