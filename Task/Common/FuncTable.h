#pragma once

#include<Context/Context.h>
#include<functional>
#include<stdexcept>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Task::TaskScheduler>
	{
		friend class Pitaya::Engine::Engine;
	private:
		FuncTable() = default;
		~FuncTable() = default;

	public:
		FuncTable(const FuncTable&) = delete;
		FuncTable& operator=(const FuncTable&) = delete;
		FuncTable(FuncTable&&) = delete;
		FuncTable& operator=(FuncTable&&) = delete;

	public:
		inline bool Check() const
		{
			if (!OnPostJob) { throw std::runtime_error("FuncTable miss [Task::PostJob] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnPostJob = nullptr;
		}

	public:
		void InvokeOnPostJob(std::function<void()> func, std::string_view name) noexcept
		{
			OnPostJob(std::move(func), name);
		}

	private:
		void (ENGINE_CALL *OnPostJob)(std::function<void()>, std::string_view) noexcept = nullptr;
	};
}

namespace Pitaya::Task
{
	inline void PostJob(std::function<void()> func, std::string_view name)
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Task::TaskScheduler>().InvokeOnPostJob(std::move(func), name);
	}
}
