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
		enum class CameraMode : uint8_t
		{
			Fly = 0,
			Orbit,
			Transitioning
		};

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
		void Update();
		void UpdateCameraVectors();
		void Move(Pitaya::Core::Direction);

	public:
		void Focus(const glm::vec3& targetPoint, float targetDist = 5.0f);

	private:
		void OnMouseScroll(const Pitaya::Event::MouseScrollEventArgs&);
		void OnMouseCurrsorMove(const Pitaya::Event::MouseCurrsorMoveEventArgs&);

	public:
		inline const Pitaya::Core::CameraState& GetCameraState() const noexcept
		{
			return state;
		}
		inline const Pitaya::Core::CameraSnapshot& GetCameraSnapshot() const noexcept
		{
			if (dirty) 
			{ 
				snapshot = state.BuildSnapshot(position, forward, up);
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
			if (dirtyRT)
			{
				rtSnapshot = BuildRT();
				dirtyRT = false;
			}
			return rtSnapshot;
		}
		inline const Pitaya::Render::PostProcessSetting& GetPostProcessSettings() const noexcept
		{
			return setting;
		}
		inline Pitaya::Render::RenderLayer GetCullingMask() const noexcept
		{
			return cullingMask.GetEnum();
		}
		inline bool GetRenderTargetIsReady() const noexcept
		{
			return renderTarget.IsReady();
		}
		inline void ApplyViewMatrix(const glm::mat4& newViewMatrix)
		{
			glm::mat4 invView = glm::inverse(newViewMatrix);
			position = glm::vec3(invView[3]);
			glm::vec3 worldForward = -glm::vec3(invView[2]);
			forward = glm::normalize(worldForward);
			pitch = glm::degrees(asin(glm::clamp(forward.y, -0.999f, 0.999f)));
			yaw = glm::degrees(atan2(forward.z, forward.x));
			if (mode == CameraMode::Orbit) { pivot = position + forward * distance; }
			dirty = true;
		}

	private:
		//Camera Base
		Pitaya::Core::CameraState state;
		mutable Pitaya::Core::CameraSnapshot snapshot;

		//Render Base
		Pitaya::Core::StateFlags<Pitaya::Render::RenderLayer> cullingMask = static_cast<Pitaya::Render::RenderLayer>(0xFFFFFFFF);
		Pitaya::Core::Asset<Pitaya::Asset::RenderTarget> renderTarget = nullptr;
		mutable Pitaya::Render::RenderTargetSnapshot rtSnapshot;
		Pitaya::Render::PostProcessSetting setting;		//TODO 换成Asset 然后生成PostProcessSetting 

		//Transform
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		float yaw = -90.0f;
		float pitch = 0.0f;

		//Orbit
		glm::vec3 pivot = glm::vec3(0.0f);	//轨道中心点
		float distance = 10.0f;				//轨道距离

		//Transition Animation
		glm::vec3 startPos = glm::vec3(0.0f);
		glm::vec3 targetPos = glm::vec3(0.0f);
		glm::vec3 startPivot = glm::vec3(0.0f);
		glm::vec3 targetPivot = glm::vec3(0.0f);
		float transitionElapsed = 0.0f;
		inline static const constexpr float TransitionDuration = 0.5f; // 0.5秒过渡

		//Internal State
		glm::vec2 lastMousePosition = glm::vec2(0.0f);
		float speed = 5.0f;
		float sensitivity = 0.1f;
		bool firstMove = true;
		mutable bool dirty = true;
		mutable bool dirtyRT = true;
		CameraMode mode = CameraMode::Fly;
	};
}
