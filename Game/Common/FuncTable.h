#pragma once

#include<Context/Context.h>

namespace Pitaya::Game { class Scene; }

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Game::GameWorld>
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
			if (!OnGetActiveScene) { throw std::runtime_error("Delegates miss [GameWorld::GetActiveScene] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnGetActiveScene = nullptr;
		}

	public:
		inline Pitaya::Game::Scene* InvokeOnGetActiveScene() const noexcept
		{
			return OnGetActiveScene();
		}

	private:
		Pitaya::Game::Scene* (ENGINE_CALL *OnGetActiveScene)() noexcept = nullptr;
	};
}

namespace Pitaya::Game
{
	inline Pitaya::Game::Scene* GetActiveScene() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Game::GameWorld>().InvokeOnGetActiveScene();
	}
}