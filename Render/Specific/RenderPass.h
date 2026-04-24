#pragma once

#include<Core/StateFlags/StateFlags.h>
#include<Core/Camera/CameraSnapshot.h>
#include<Render/Common/RenderTargetSnapshot.h>
#include<Render/Command/PostProcessCommand.h>
#include<Render/Common/PostProcessSetting.h>
#include<Render/Common/RenderLayer.h>

namespace Pitaya::Asset { class RenderTarget; }

namespace Pitaya::Render
{
	struct RenderPass
	{
		Pitaya::Core::CameraSnapshot CameraSnapshot;
		Pitaya::Render::PostProcessSetting PostProcessSetting;
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> CullingMask;
		Pitaya::Asset::RenderTarget* RenderTarget = nullptr;

		RenderPass(const Pitaya::Core::CameraSnapshot& cameraSnapshot,  const Pitaya::Render::PostProcessSetting& setting, Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> cullingMask, Pitaya::Asset::RenderTarget* renderTarget)
			:CameraSnapshot(cameraSnapshot), PostProcessSetting(setting), CullingMask(cullingMask), RenderTarget(renderTarget){ }
	};
}