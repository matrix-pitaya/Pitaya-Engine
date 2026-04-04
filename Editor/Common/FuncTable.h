#pragma once

#include<Core/PassKey/PassKey.h>
#include<Context/Context.h>
#include<Log/Common/LogLevel.h>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Editor::Editor>
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
			if (!OnInitializeForRender) { throw std::runtime_error("FuncTable miss [Editor::InitializeForRender] Function!"); }
			if (!OnReleaseForRender) { throw std::runtime_error("FuncTable miss [Editor::ReleaseForRender] Function!"); }
			if (!OnConsole) { throw std::runtime_error("FuncTable miss [Editor::Console] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnInitializeForRender = nullptr;
			OnReleaseForRender = nullptr;
			OnConsole = nullptr;
		}

	public:
		inline bool InvokeOnInitializeForRender(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey) const
		{
			return OnInitializeForRender(passkey);
		}
		inline void InvokeOnReleaseForRender(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey) const
		{
			OnReleaseForRender(passkey);
		}
		inline void InvokeOnConsole(Pitaya::Log::LogLevel level, std::string_view message) const
		{
			OnConsole(level, message);
		}
	
	private:
		bool (*OnInitializeForRender)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;
		void (*OnReleaseForRender)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;
		void (*OnConsole)(Pitaya::Log::LogLevel, std::string_view) = nullptr;
	};
}

namespace Pitaya::Editor
{
	inline bool InitializeForRender(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Editor::Editor>().InvokeOnInitializeForRender(passkey);
	}
	inline void ReleaseForRender(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Editor::Editor>().InvokeOnReleaseForRender(passkey);
	}
	inline void Console(Pitaya::Log::LogLevel level, std::string_view message)
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Editor::Editor>().InvokeOnConsole(level, message);
	}
}