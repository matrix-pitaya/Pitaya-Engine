#include<Physics/Backend/Bullet/BulletPhysicsSimulator.h>
#include<Hook/def.h>

bool Pitaya::Physics::BulletPhysicsSimulator::Initialize()
{
	return true;
}
void Pitaya::Physics::BulletPhysicsSimulator::Release()
{

}
void Pitaya::Physics::BulletPhysicsSimulator::FixedUpdate()
{
	INVOKE_PREFIXEDUPDATE_HOOK
}
