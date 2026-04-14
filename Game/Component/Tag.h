#pragma once

#include<Core/Utils/String.h>

namespace Pitaya::Game
{
	class GameObject;
	struct Tag
	{
	public:
		Tag(std::string_view name, std::string_view tag)
		{
			Pitaya::Core::CopyStringToCharArray(name, this->name);
			Pitaya::Core::CopyStringToCharArray(tag, this->tag);
		}

	public:
		inline std::string_view GetName() const noexcept
		{
			return name;
		}
		inline std::string_view GetTag() const noexcept
		{
			return tag;
		}

	public:
		inline void SetName(std::string_view name) noexcept
		{
			Pitaya::Core::CopyStringToCharArray(name, this->name);
		}
		inline void SetTag(std::string_view tag) noexcept
		{
			Pitaya::Core::CopyStringToCharArray(tag, this->tag);
		}

	public:
		inline static const constexpr uint32_t MAX_STR_SIZE = 128;

	private:
		char name[MAX_STR_SIZE] = {};
		char tag[MAX_STR_SIZE] = {};
	};
}