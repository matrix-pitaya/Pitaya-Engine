#pragma once

#include<Core/PassKey/PassKey.h>
#include<Core/Utils/String.h>

namespace Pitaya::Game
{
	class Scene;
	struct Metadata
	{
	public:
		Metadata(std::string_view name, std::string_view label)
		{
			Pitaya::Core::CopyStringToCharArray(name, this->name);
			Pitaya::Core::CopyStringToCharArray(label, this->label);
		}

	public:
		inline std::string_view GetName() const noexcept
		{
			return name;
		}
		inline std::string_view GetLabel() const noexcept
		{
			return label;
		}

	public:
		inline void SetName(std::string_view name) noexcept
		{
			Pitaya::Core::CopyStringToCharArray(name, this->name);
		}
		inline void SetLabel(std::string_view label) noexcept
		{
			Pitaya::Core::CopyStringToCharArray(label, this->label);
		}
		inline void SetActive(bool active) noexcept
		{
			if (this->active != active)
			{
				this->active = active;
				this->dirty = true;
			}
		}

	public:
		inline bool IsActive() const noexcept
		{
			return active;
		}
		inline bool IsDirty() const noexcept
		{
			return dirty;
		}

	public:
		inline void MarkDirty(Pitaya::Core::PassKey<Scene>) noexcept
		{
			dirty = true;
		}
		inline void MarkClear(Pitaya::Core::PassKey<Scene>) noexcept
		{
			dirty = false;
		}

	public:
		inline static constexpr const uint32_t MAX_STR_SIZE = 64;

	private:
		char name[MAX_STR_SIZE] = {};
		char label[MAX_STR_SIZE] = {};
		bool active = true;	// 处理物体是否激活
		bool dirty = true;	// 活性脏标记
	};
}