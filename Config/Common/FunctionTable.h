#pragma once

#include<Context/Context.h>
#include<Render/Common/API.h>
#include<GPU/Common/FrameBufferSpecification.h>
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
		inline bool Verify() const
		{
			if (!OnGetRenderAPI) { throw std::runtime_error("FuncTable miss [Config::GetRenderAPI] Function!"); }
			if (!OnGetEnableVSync) { throw std::runtime_error("FuncTable miss [Config::GetEnableVSync] Function!"); }
			if (!OnGetMainSceneSpec) { throw std::runtime_error("FuncTable miss [Config::GetMainSceneSpec] Function!"); }
			if (!OnGetMainPingPongSpec) { throw std::runtime_error("FuncTable miss [Config::GetMainPingPongSpec] Function!"); }
			if (!OnGetMainFinalSpec) { throw std::runtime_error("FuncTable miss [Config::GetMainFinalSpec] Function!"); }
			return true;
		}
		inline void Nullify() noexcept
		{
			OnGetRenderAPI = nullptr;
			OnGetEnableVSync = nullptr;
			OnGetMainSceneSpec = nullptr;
			OnGetMainPingPongSpec = nullptr;
			OnGetMainFinalSpec = nullptr;
		}

	public:
		inline Pitaya::Render::API InvokeOnGetRenderAPI() noexcept
		{
			return OnGetRenderAPI();
		}
		inline bool InvokeOnGetEnableVSync() const noexcept
		{
			return OnGetEnableVSync();
		}
		inline Pitaya::GPU::FrameBufferSpecification InvokeOnGetMainSceneSpec() const noexcept
		{
			return OnGetMainSceneSpec();
		}
		inline Pitaya::GPU::FrameBufferSpecification InvokeOnGetMainPingPongSpec() const noexcept
		{
			return OnGetMainPingPongSpec();
		}
		inline Pitaya::GPU::FrameBufferSpecification InvokeOnGetMainFinalSpec() const noexcept
		{
			return OnGetMainFinalSpec();
		}

	private:
		Pitaya::Render::API (ENGINE_CALL *OnGetRenderAPI)() noexcept = nullptr;
		bool (ENGINE_CALL *OnGetEnableVSync)() noexcept = nullptr;
		Pitaya::GPU::FrameBufferSpecification (ENGINE_CALL *OnGetMainSceneSpec)() noexcept = nullptr;
		Pitaya::GPU::FrameBufferSpecification (ENGINE_CALL *OnGetMainPingPongSpec)() noexcept = nullptr;
		Pitaya::GPU::FrameBufferSpecification (ENGINE_CALL *OnGetMainFinalSpec)() noexcept = nullptr;
	};
}

namespace Pitaya::Config
{
	inline Pitaya::Render::API GetRenderAPI() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetRenderAPI();
	}
	inline bool GetEnableVSync() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetEnableVSync();
	}
	inline Pitaya::GPU::FrameBufferSpecification GetMainSceneSpec() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetMainSceneSpec();
	}
	inline Pitaya::GPU::FrameBufferSpecification GetMainPingPongSpec() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetMainPingPongSpec();
	}
	inline Pitaya::GPU::FrameBufferSpecification GetMainFinalSpec() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Config::Configurator>().InvokeOnGetMainFinalSpec();
	}
}
