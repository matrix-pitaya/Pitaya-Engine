#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>
#include<Physics/Common/API.h>

namespace Pitaya::Physics
{
	class PhysicsSimulator
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<PhysicsSimulator>;
		private:
			static PhysicsSimulator* Create(Pitaya::Physics::API);
			static void Destroy(PhysicsSimulator* physicsSimulator)
			{
				PITAYA_DELETE(physicsSimulator);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<PhysicsSimulator>;
		private:
			static bool Initialize(PhysicsSimulator* physicsSimulator)
			{
				return physicsSimulator->Initialize();
			}
			static void Release(PhysicsSimulator* physicsSimulator)
			{
				physicsSimulator->Release();
			}
			static void FixedUpdate(PhysicsSimulator* physicsSimulator)
			{
				physicsSimulator->FixedUpdate();
			}
		};

	protected:
		PhysicsSimulator() = default;
		virtual ~PhysicsSimulator() = default;

	public:
		PhysicsSimulator(const PhysicsSimulator&) = delete;
		PhysicsSimulator& operator=(const PhysicsSimulator&) = delete;
		PhysicsSimulator(PhysicsSimulator&&) = delete;
		PhysicsSimulator& operator=(PhysicsSimulator&&) = delete;

	protected:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;
		virtual void FixedUpdate() = 0;

	public:
		inline static constexpr const uint32_t MaxFixupdataExecuteTimes = 5;
	};
}
