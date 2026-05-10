#pragma once

#include<Core/Camera/CameraState.h>
#include<Core/Direction/Direction.h>
#include<Core/Asset/Asset.h>
#include<Core/StateFlags/StateFlags.h>
#include<Core/PassKey/PassKey.h>

#include<Event/Common/EventArgs.h>
#include<Asset/Common/RenderTarget.h>
#include<Render/Common/PostProcessSetting.h>
#include<Render/Common/RenderLayer.h>

namespace Pitaya::Render { class Renderer; }

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
		bool Initialize_Main();
		bool Initialize_Render(Pitaya::Core::PassKey<Pitaya::Render::Renderer>);
		void Release_Main();
		void Release_Render();

	private:
		void Update();
		void UpdateCameraVectors();
		void Move(Pitaya::Core::Direction);

	public:
		void Focus(const glm::vec3& targetPoint, float targetDist = 5.0f);

	private:
		void OnMouseScroll(Pitaya::Event::MouseScrollEventArgs);
		void OnMouseCurrsorMove(Pitaya::Event::MouseCurrsorMoveEventArgs);

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
		inline const Pitaya::Render::PostProcessSetting& GetPostProcessSettings() const noexcept
		{
			return setting;
		}
		inline Pitaya::Render::RenderLayer GetCullingMask() const noexcept
		{
			return cullingMask.GetEnum();
		}
		inline Pitaya::Asset::RenderTarget* GetNativeRenderTarget()
		{
			return &renderTarget;
		}

	public:
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
		Pitaya::Asset::RenderTarget renderTarget;
		Pitaya::Render::PostProcessSetting setting;
		std::atomic<bool> falg = false;	

		//Transform
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		float yaw = -90.0f;
		float pitch = 0.0f;

		//Orbit
		glm::vec3 pivot = glm::vec3(0.0f);	//轨道中心点
		float distance = 5.0f;				//轨道距离

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
		CameraMode mode = CameraMode::Fly;
	};
}
