#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>
#include<Game/Scene/Scene.h>

namespace Pitaya::Game
{
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
			static void BeginFrame(GameWorld* gameWorld)
			{
				gameWorld->BeginFrame();
			}
			static void EndFrame(GameWorld* gameWorld)
			{
				gameWorld->EndFrame();
			}
			static void FixedUpdate(GameWorld* gameWorld)
			{
				gameWorld->FixedUpdate();
			}
			static void Update(GameWorld* gameWorld)
			{
				gameWorld->Update();
			}
			static void LateUpdate(GameWorld* gameWorld)
			{
				gameWorld->LateUpdate();
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
		void BeginFrame();
		void FixedUpdate();
		void Update();
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
