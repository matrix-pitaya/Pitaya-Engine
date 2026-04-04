#pragma once

#include<Core/Camera/CameraState.h>
#include<Game/Component/Component.h>

namespace Pitaya::Game
{
	class Camera : public Component
	{
	public:
		Camera(GameObject* gameobject)
			:Component(gameobject) {}
		~Camera() = default;

	private:
		Pitaya::Core::CameraState cameraState;
	};
}
