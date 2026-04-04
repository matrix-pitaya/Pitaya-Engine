#pragma once

#include<Context/Context.h>
#include<Render/Common/API.h>
#include<stdexcept>

namespace Pitaya::Engine 
{
	template<>
	struct FuncTable<Pitaya::Config::Configurator>
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
			if (!OnGetRenderAPI) { throw std::runtime_error("FuncTable miss [Config::GetRenderAPI] Function!"); }
			if (!OnGetMaxFixupdataExecuteTimes) { throw std::runtime_error("FuncTable miss [Config::GetMaxFixupdataExecuteTimes] Function!"); }
			if (!OnGetMaxInstancesCount) { throw std::runtime_error("FuncTable miss [Config::GetMaxInstancesCount] Function!"); }
			if (!OnGetMaxBonesPerInstance) { throw std::runtime_error("FuncTable miss [Config::GetMaxBonesPerInstance] Function!"); }
			if (!OnGetEnableVSync) { throw std::runtime_error("FuncTable miss [Config::GetEnableVSync] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnGetRenderAPI = nullptr;
			OnGetMaxFixupdataExecuteTimes = nullptr;
			OnGetMaxInstancesCount = nullptr;
			OnGetMaxBonesPerInstance = nullptr;
			OnGetEnableVSync = nullptr;
		}

	public:
		inline Pitaya::Render::API InvokeOnGetRenderAPI() noexcept
		{
			return OnGetRenderAPI();
		}
		inline size_t InvokeOnGetMaxFixupdataExecuteTimes() noexcept
		{
			return OnGetMaxFixupdataExecuteTimes();
		}
		inline uint32_t InvokeOnGetMaxInstancesCount() const noexcept
		{
			return OnGetMaxInstancesCount();
		}
		inline uint32_t InvokeOnGetMaxBonesPerInstance() const noexcept
		{
			return OnGetMaxBonesPerInstance();
		}
		inline bool InvokeOnGetEnableVSync() const noexcept
		{
			return OnGetEnableVSync();
		}

	private:
		Pitaya::Render::API(*OnGetRenderAPI)() noexcept = nullptr;
		size_t(*OnGetMaxFixupdataExecuteTimes)() noexcept = nullptr;
		uint32_t (*OnGetMaxInstancesCount)() noexcept = nullptr;
		uint32_t (*OnGetMaxBonesPerInstance)() noexcept = nullptr;
		bool(*OnGetEnableVSync)() noexcept = nullptr;
	};
}

namespace Pitaya::Config
{
	inline Pitaya::Render::API GetRenderAPI() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetRenderAPI();
	}
	inline size_t GetMaxFixupdataExecuteTimes() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetMaxFixupdataExecuteTimes();
	}
	inline uint32_t GetMaxInstancesCount() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetMaxInstancesCount();
	}
	inline uint32_t GetMaxBonesPerInstance() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetMaxBonesPerInstance();
	}
	inline bool GetEnableVSync() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetEnableVSync();
	}
}
