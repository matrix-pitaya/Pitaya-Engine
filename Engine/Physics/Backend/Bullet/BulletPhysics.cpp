#include"BulletPhysics.h"

bool Pitaya::Engine::Physics::BulletPhysics::Initialize()
{
	if (isInitialized)
	{
		return false;
	}

	isInitialized = true;
	return true;
}
void Pitaya::Engine::Physics::BulletPhysics::Release()
{
	if (isReleased)
	{
		return;
	}

	isReleased = true;
}
void Pitaya::Engine::Physics::BulletPhysics::OnFixupdata()
{

}