#pragma once

#include<Engine/Runtime/Component.h>
#include<Renderer/Runtime/Component/Camera/CameraState.h>

namespace Pitaya::Engine
{
	class Camera : public Component
	{
	public:
		Camera(GameObject* gameobject)
			:Component(gameobject) {}
		~Camera() = default;

	private:
		Pitaya::Engine::Renderer::CameraState cameraState;
	};
}