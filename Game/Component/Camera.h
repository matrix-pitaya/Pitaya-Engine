#pragma once

#include<Core/Camera/CameraState.h>
#include<Core/StateFlags/StateFlags.h>
#include<Core/Asset/Asset.h>
#include<Asset/Common/FuncTable.h>
#include<Asset/Common/RenderTarget.h>
#include<Render/Common/PostProcessSetting.h>
#include<Render/Common/RenderLayer.h>
#include<Render/Common/RenderTargetSnapshot.h>

namespace Pitaya::Game
{
	struct Camera
	{
	public:
		inline Pitaya::Core::CameraState GetCameraState() const noexcept
		{
			return cameraState;
		}
		inline Pitaya::Render::RenderTargetSnapshot RenderTargetSnapshot() const noexcept
		{
			return { renderTarget->SceneFrameBuffer,
					 renderTarget->SceneInternalFrameBuffer,
					 renderTarget->SceneColorAttachment,
				   { renderTarget->PingPongFrameBuffers[0],renderTarget->PingPongFrameBuffers[1] },
				   { renderTarget->PingPongColorAttachments[0],renderTarget->PingPongColorAttachments[1] },
					 renderTarget->FinalFrameBuffer,
					 renderTarget->FinalColorAttachment,
					 renderTarget->ClearColor,
				   { {0.0f, 0.0f}, { renderTarget->SceneFrameBufferSpecification.Width, renderTarget->SceneFrameBufferSpecification.Height } },
					 renderTarget->ClearDepth,
					 renderTarget->ClearStencil,
					 renderTarget->SceneFrameBufferSpecification.Samples > 1 };
		}
		inline Pitaya::Render::RenderLayer GetCullingMask() const noexcept
		{
			return cullingMask.GetEnum();
		}
		inline Pitaya::Render::PostProcessSetting& GetPostProcessSetting() noexcept
		{
			return setting;
		}
		inline bool GetRenderTargetIsReady() const noexcept
		{
			return renderTarget.IsReady();
		}
		inline bool HasCustomRenderTarget() const noexcept
		{
			return renderTarget != nullptr;
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
		inline void SetRenderTarget(Pitaya::Core::GUID rt) noexcept
		{
			renderTarget = Pitaya::Asset::LoadAsset<Pitaya::Asset::RenderTarget>(rt);
		}

	private:
		Pitaya::Core::CameraState cameraState;
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> cullingMask = 
			static_cast<Pitaya::Render::RenderLayer>(~static_cast<uint32_t>(Pitaya::Render::RenderLayer::EditorOnly));
		Pitaya::Core::Asset<Pitaya::Asset::RenderTarget> renderTarget = nullptr;
		Pitaya::Render::PostProcessSetting setting;		//TODO 换成Asset 然后生成PostProcessSetting 
	};
}
