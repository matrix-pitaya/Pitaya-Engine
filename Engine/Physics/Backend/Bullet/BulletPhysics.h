#pragma once

#include"../../Engine/Engine/Interface/Physics.h"

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Physics
{
	class BulletPhysics : public Pitaya::Engine::Interface::Physics
	{
		friend class Pitaya::Engine::Engine;
	private:
		BulletPhysics() = default;
		~BulletPhysics() override { Release(); }

	public:
		BulletPhysics(const BulletPhysics&) = delete;
		BulletPhysics& operator=(const BulletPhysics&) = delete;
		BulletPhysics(BulletPhysics&&) = delete;
		BulletPhysics& operator=(BulletPhysics&&) = delete;

	public:
		bool Initialize() override;
		void Release() override;
		void OnFixupdata() override;
	};
}