#pragma once

#include<Core/Camera/CameraState.h>
#include<Core/Common/Direction.h>
#include<Core/Asset/Asset.h>
#include<Core/StateFlags/StateFlags.h>

#include<Event/Common/EventArgs.h>
#include<Asset/Common/RenderTarget.h>
#include<Render/Common/RenderTargetSnapshot.h>
#include<Render/Common/PostProcessSetting.h>
#include<Render/Common/RenderLayer.h>

namespace Pitaya::Editor
{
	class Editor;
	class Camera
	{
		friend class Pitaya::Editor::Editor;
	private:
		Camera() = default;
		~Camera() = default;

	public:
		Camera(const Camera&) = delete;
		Camera& operator=(const Camera&) = delete;
		Camera(Camera&&) = delete;
		Camera& operator=(Camera&&) = delete;

	private:
		bool Initialize();
		void Release();

	private:
		void Updata();

	private:
		void Move(Pitaya::Core::Direction);

	private:
		void OnMouseScroll(const Pitaya::Event::MouseScrollEventArgs&);
		void OnMouseCurrsorMove(const Pitaya::Event::MouseCurrsorMoveEventArgs&);

	public:
		inline const Pitaya::Core::CameraSnapshot& GetCameraSnapshot() const noexcept
		{
			static Pitaya::Core::CameraSnapshot snapshot = state.BuildSnapshot(position, forward, up);
			if (dirty) 
			{ 
				snapshot  = state.BuildSnapshot(position, forward, up);
				dirty = false;
			}
			return snapshot;
		}
		inline const Pitaya::Render::RenderTargetSnapshot& GetRenderTarget() const noexcept
		{
			auto BuildRT = [this]() -> Pitaya::Render::RenderTargetSnapshot
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
				};

			static Pitaya::Render::RenderTargetSnapshot snapshot = BuildRT();
			if (dirtyRT)
			{
				snapshot = BuildRT();
				dirtyRT = false;
			}
			return snapshot;
		}
		inline const Pitaya::Render::PostProcessSetting& GetPostProcessSettings() const noexcept
		{
			return setting;
		}
		inline Pitaya::Render::RenderLayer GetCullingMask() const noexcept
		{
			return cullingMask.GetEnum();
		}
		inline float GetAspectRatio() const noexcept
		{
			return state.AspectRatio;
		}
		inline bool IsRenderTargetReady()
		{
			return renderTarget.IsReady();
		}

	private:
		Pitaya::Core::CameraState state;
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> cullingMask = static_cast<Pitaya::Render::RenderLayer>(0xFFFFFFFF);
		Pitaya::Core::Asset<Pitaya::Asset::RenderTarget> renderTarget = nullptr;
		Pitaya::Render::PostProcessSetting setting;		//TODO 换成Asset 然后生成PostProcessSetting 

		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec2 lastMousePosition = glm::vec2(0.0f);

		float speed = 5.0f;
		float sensitivity = 0.1f;

		float yaw = -90.0f;
		float pitch = 0.0f;

		bool firstMove = true;

		mutable bool dirty = false;
		mutable bool dirtyRT = false;
	};
}
