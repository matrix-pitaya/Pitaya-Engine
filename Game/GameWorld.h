#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>

namespace Pitaya::Game
{
	class Scene;
	class GameWorld
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<GameWorld>;
		private:
			static GameWorld* Create()
			{
				return PITAYA_NEW(GameWorld);
			}
			static void Destroy(GameWorld* gameWorld)
			{
				PITAYA_DELETE(gameWorld);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<GameWorld>;
		private:
			static bool Initialize(GameWorld* gameWorld)
			{
				return gameWorld->Initialize();
			}
			static void Release(GameWorld* gameWorld)
			{
				gameWorld->Release();
			}
			static void LateUpdate(GameWorld* gameWorld)
			{
				gameWorld->LateUpdate();
			}
			static void EndFrame(GameWorld* gameWorld)
			{
				gameWorld->EndFrame();
			}
		};

	private:
		GameWorld() = default;
		~GameWorld() = default;

	public:
		GameWorld(const GameWorld&) = delete;
		GameWorld& operator=(const GameWorld&) = delete;
		GameWorld(GameWorld&&) = delete;
		GameWorld& operator=(GameWorld&&) = delete;

	private:
		bool Initialize();
		void Release();
		void LateUpdate();
		void EndFrame();

	public:
		inline Scene* GetActiveScene() const noexcept
		{
			return activeScene;
		}

	private:
		Scene* activeScene = nullptr;
	};
}
