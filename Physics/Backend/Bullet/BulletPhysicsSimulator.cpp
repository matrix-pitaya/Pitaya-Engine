#include<Physics/Frontend/PhysicsSimulator.h>
#include<Hook/def.h>

#if defined(PITAYA_USE_BULLET)

namespace
{

}

bool Pitaya::Physics::PhysicsSimulator::Initialize()
{
	return true;
}
void Pitaya::Physics::PhysicsSimulator::Release()
{

}
void Pitaya::Physics::PhysicsSimulator::FixedUpdate()
{
	INVOKE_PREFIXEDUPDATE_HOOK
}
#endif
