#include<Physics/Frontend/PhysicsSimulator.h>
#include<Physics/Common/RigidBodyType.h>
#include<Time/Common/FuncTable.h>
#include<Core/Allocate/Allocate.h>

#if defined(PITAYA_PHYSICS_BULLET)
#include<btBulletDynamicsCommon.h>

namespace
{
    struct BulletPhysicsContext
    {
        btDiscreteDynamicsWorld* DynamicsWorld = nullptr;
        btDefaultCollisionConfiguration* CollisionConfiguration = nullptr;
        btCollisionDispatcher* Dispatcher = nullptr;
        btBroadphaseInterface* Broadphase = nullptr;
        btSequentialImpulseConstraintSolver* ConstraintSolver = nullptr;
    };
}

bool Pitaya::Physics::PhysicsSimulator::InitializePhysicsContext()
{
    auto& context = backendStorage.Cast<BulletPhysicsContext>();
    context.CollisionConfiguration = Pitaya::Core::New<btDefaultCollisionConfiguration>();
    context.Dispatcher = Pitaya::Core::New<btCollisionDispatcher>(context.CollisionConfiguration);
    context.Broadphase = Pitaya::Core::New<btDbvtBroadphase>();
    context.ConstraintSolver = Pitaya::Core::New<btSequentialImpulseConstraintSolver>();
    context.DynamicsWorld = Pitaya::Core::New<btDiscreteDynamicsWorld>(context.Dispatcher, context.Broadphase, context.ConstraintSolver, context.CollisionConfiguration);
    context.DynamicsWorld->getSolverInfo().m_numIterations = 10;
    context.DynamicsWorld->setGravity(btVector3(0, -9.8, 0));
    return true;
}
void Pitaya::Physics::PhysicsSimulator::ReleasePhysicsContext()
{
    auto& context = backendStorage.Cast<BulletPhysicsContext>();
    Pitaya::Core::Delete(context.DynamicsWorld);
    Pitaya::Core::Delete(context.ConstraintSolver);
    Pitaya::Core::Delete(context.Broadphase);
    Pitaya::Core::Delete(context.Dispatcher);
    Pitaya::Core::Delete(context.CollisionConfiguration);
    context.DynamicsWorld = nullptr;
    context.ConstraintSolver = nullptr;
    context.Broadphase = nullptr;
    context.Dispatcher = nullptr;
    context.CollisionConfiguration = nullptr;
}
#endif
