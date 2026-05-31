#pragma once

#include<Core/Camera/CameraState.h>
#include<Core/StateFlags/StateFlags.h>
#include<Core/Asset/AssetRef.h>
#include<Asset/Common/RenderTarget.h>
#include<Asset/Common/FuncTable.h>
#include<Render/Common/PostProcessSetting.h>
#include<Render/Common/RenderLayer.h>

namespace Pitaya::Game
{
	struct Camera
	{
	public:
		inline Pitaya::Core::CameraState GetCameraState() const noexcept
		{
			return cameraState;
		}
		inline Pitaya::Render::RenderLayer GetCullingMask() const noexcept
		{
			return cullingMask.GetEnum();
		}
		inline Pitaya::Render::PostProcessSetting& GetPostProcessSetting() noexcept
		{
			return postProcessSetting;
		}
		inline Pitaya::Asset::RenderTarget* GetNativeRenderTarget() noexcept
		{
			return renderTarget.GetNativeAssetData();
		}
		inline bool GetRenderTargetIsReady() const noexcept
		{
			return renderTarget.IsReady();
		}
		inline bool GetIsRenderToMainDisplayRT() const noexcept
		{
			return renderTarget == nullptr;
		}

	public:
		inline void SetCameraState(Pitaya::Core::CameraState state) noexcept
		{
			cameraState = state;
		}
		inline void SetCullingMask(Pitaya::Render::RenderLayer mask) noexcept
		{
			cullingMask = mask;
		}
		inline void SetRenderTarget(Pitaya::Core::GUID renderTargetGUID) noexcept
		{
			renderTarget = Pitaya::Asset::LoadAsset<Pitaya::Asset::RenderTarget>(renderTargetGUID);
		}

	public:
		inline void MarkRenderToMainDisplayRT() noexcept
		{
			renderTarget = nullptr;
		}

	private:
		Pitaya::Core::CameraState cameraState;
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> cullingMask = static_cast<Pitaya::Render::RenderLayer>(~static_cast<uint32_t>(Pitaya::Render::RenderLayer::EditorOnly));
		Pitaya::Core::AssetRef<Pitaya::Asset::RenderTarget> renderTarget = nullptr;	//为nullptr则渲染到MainDisplayRT
		Pitaya::Render::PostProcessSetting postProcessSetting;		//TODO 换成Asset 然后生成PostProcessSetting 
	};
}
