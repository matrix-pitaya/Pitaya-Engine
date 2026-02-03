#pragma once

#include"Component.h"

#include"glm.hpp"
#include"gtc/matrix_transform.hpp"
#include"gtc/type_ptr.hpp"
#include"gtx/euler_angles.hpp"

namespace Pitaya::Engine
{
	class Transform : public Component
	{
		DEFAULT_COPY_AND_MOVE_PRIVATE(Transform)

		friend entt::storage<Transform>;
	private:
		Transform(GameObject* gameobject) 
			:Component(gameobject) {}
		~Transform() override = default;

	private:
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotate = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		bool isDirty = true;
	};
}

namespace entt
{
	template<>
	struct component_traits<Pitaya::Engine::Transform>
	{
		static constexpr bool in_place_delete = true;
	};
}