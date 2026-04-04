#pragma once

#include<Physics/Frontend/PhysicsSimulator.h>

namespace Pitaya::Physics { class Module; }

namespace Pitaya::Physics
{
	class BulletPhysicsSimulator : public Pitaya::Physics::PhysicsSimulator
	{
		friend class Pitaya::Physics::PhysicsSimulator;
	private:
		BulletPhysicsSimulator() = default;
		~BulletPhysicsSimulator() override = default;

	public:
		BulletPhysicsSimulator(const BulletPhysicsSimulator&) = delete;
		BulletPhysicsSimulator& operator=(const BulletPhysicsSimulator&) = delete;
		BulletPhysicsSimulator(BulletPhysicsSimulator&&) = delete;
		BulletPhysicsSimulator& operator=(BulletPhysicsSimulator&&) = delete;

	private:
		bool Initialize() override;
		void Release() override;
		void FixedUpdate() override;
	};
}
