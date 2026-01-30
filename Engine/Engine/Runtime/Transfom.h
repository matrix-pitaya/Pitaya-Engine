#pragma once

#include"Component.h"

#include"glm.hpp"
#include"gtc/matrix_transform.hpp"
#include"gtc/type_ptr.hpp"
#include"gtx/euler_angles.hpp"

namespace Pitaya::Engine
{
	class GameObject;
	class Transform : public Component
	{
		DELETE_COPY_AND_MOVE(Transform)

	public:
		Transform(GameObject* const gameobject) 
			:gameobject(gameobject) {}
		~Transform() = default;

	public:


	private:
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotate = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		GameObject* const gameobject;

		bool isDirty = true;
	};
}