#pragma once

#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include<gtx/euler_angles.hpp>
#include<gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<gtx/quaternion.hpp> 
#include<entt/entt.hpp>

namespace Pitaya::Game
{
	struct Transform
	{
	public:
		inline glm::vec3 GetLocalForward() const noexcept 
		{
			return glm::quat(localRotate) * glm::vec3(0.0f, 0.0f, -1.0f);
		}
		inline glm::vec3 GetLocalUp() const noexcept 
		{ 
			return glm::quat(localRotate) * glm::vec3(0.0f, 1.0f, 0.0f);
		}
		inline glm::vec3 GetLocalPosition() const noexcept
		{
			return localPosition;
		}
		inline glm::vec3 GetLocalRotate() const noexcept
		{
			return localRotate;
		}
		inline glm::vec3 GetLocalScale() const noexcept
		{
			return localScale;
		}
		inline glm::mat4 GetLocalMatrix() const noexcept
		{
			if (localdirty)
			{
				glm::mat4 t = glm::translate(glm::mat4(1.0f), localPosition);
				glm::mat4 r = glm::mat4_cast(glm::quat(localRotate));
				glm::mat4 s = glm::scale(glm::mat4(1.0f), localScale);
				localMatrix = t * r * s;
				localdirty = false;
			}
			return localMatrix;
		}
		inline const glm::mat4& GetWorldMatrix() const noexcept
		{

			return worldMatrix;
		}

	public:
		inline entt::entity GetParent() const noexcept
		{
			return parent;
		}
		inline entt::entity GetFirstChild() const noexcept
		{
			return firstChild;
		}
		inline entt::entity GetPreviousSibling() const noexcept
		{
			return previousSibling;
		}
		inline entt::entity GetNextSibling() const noexcept
		{
			return nextSibling;
		}

	public:
		inline bool GetWorldDirty() const noexcept
		{
			return worlddirty;
		}

	public:
		inline void SetLocalPosition(glm::vec3 pos) noexcept
		{
			localPosition = pos;
			localdirty = true;
			worlddirty = true;
		}
		inline void SetLocalRotate(glm::vec3 rot) noexcept
		{
			localRotate = rot;
			localdirty = true;
			worlddirty = true;
		}
		inline void SetLocalScale(glm::vec3 scale) noexcept
		{
			localScale = scale;
			localdirty = true;
			worlddirty = true;
		}
		inline void SetWorldMatrix(const glm::mat4& matrix) noexcept
		{
			worldMatrix = matrix;
		}
		inline void SetParent(entt::entity parent) noexcept
		{
			this->parent = parent;
			worlddirty = true;
		}
		inline void SetFirstChild(entt::entity firstChild) noexcept
		{
			this->firstChild = firstChild;
		}
		inline void SetPreviousSibling(entt::entity previousSibling) noexcept
		{
			this->previousSibling = previousSibling;
		}
		inline void SetNextSibling(entt::entity nextSibling) noexcept
		{
			this->nextSibling = nextSibling;
		}

	public:
		inline void SetWorldDirty(bool value) noexcept
		{
			worlddirty = value;
		}

	private:
		entt::entity parent = entt::null;
		entt::entity firstChild = entt::null;
		entt::entity previousSibling = entt::null;
		entt::entity nextSibling = entt::null;

		glm::vec3 localPosition = glm::vec3(0.0f);
		glm::vec3 localRotate = glm::vec3(0.0f);
		glm::vec3 localScale = glm::vec3(1.0f);
		mutable glm::mat4 localMatrix = glm::mat4(1.0f);
		glm::mat4 worldMatrix = glm::mat4(1.0f);
		
		mutable bool localdirty = true;
		bool worlddirty = true;
	};
}
