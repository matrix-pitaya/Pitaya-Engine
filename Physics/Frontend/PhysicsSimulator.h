#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Storage/Storage.h>
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
			static PhysicsSimulator* Create()
			{
				return PITAYA_NEW(PhysicsSimulator);
			}
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

	private:
		PhysicsSimulator() = default;
		~PhysicsSimulator() = default;

	public:
		PhysicsSimulator(const PhysicsSimulator&) = delete;
		PhysicsSimulator& operator=(const PhysicsSimulator&) = delete;
		PhysicsSimulator(PhysicsSimulator&&) = delete;
		PhysicsSimulator& operator=(PhysicsSimulator&&) = delete;

	private:
		bool Initialize();
		void Release();
		void FixedUpdate();

	public:
		inline static constexpr const uint32_t MaxFixupdataExecuteTimes = 5;

	private:
		Pitaya::Core::Storage<128> backendStorage;
	};
}
