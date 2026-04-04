#pragma once

#include<string>

namespace Pitaya::Game
{
	class GameObject;
	struct Tag
	{
	public:
		Tag(std::string_view name, std::string_view tag, GameObject* gameobject)
			:name(name), tag(tag), gameobject(gameobject) {}

	public:
		inline std::string_view GetName() const noexcept
		{
			return name;
		}
		inline std::string_view GetTag() const noexcept
		{
			return tag;
		}
		inline GameObject* GetGameObject() const noexcept
		{
			return gameobject;
		}

	public:
		inline void SetName(std::string_view name) noexcept
		{
			this->name = name;
		}
		inline void SetTag(std::string_view tag) noexcept
		{
			this->tag = tag;
		}
		inline void SetGameObject(GameObject* gameobject) noexcept
		{
			this->gameobject = gameobject;
		}

	private:
		std::string name = "GameObject";
		std::string tag = "default";
		GameObject* gameobject = nullptr;
	};
}