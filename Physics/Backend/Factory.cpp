#include<Physics/Frontend/PhysicsSimulator.h>
#include<Physics/Backend/Bullet/BulletPhysicsSimulator.h>

Pitaya::Physics::PhysicsSimulator* Pitaya::Physics::PhysicsSimulator::Factory::Create(Pitaya::Physics::API api)
{
	switch (api)
	{
		case Pitaya::Physics::API::Bullet:
			return PITAYA_NEW(Pitaya::Physics::BulletPhysicsSimulator);

		case Pitaya::Physics::API::Unknown:
			throw std::exception("Unknown Physics Backend API");

		default:
			throw std::exception("Unknown Physics Backend API");
	}
}