#include<Physics/Frontend/PhysicsSimulator.h>
#include<Physics/Common/RigidBodyType.h>
#include<Time/Common/FuncTable.h>
#include<Core/Allocate/Allocate.h>
#include<Hook/def.h>

#if defined(PITAYA_USE_BULLET)
#include<btBulletDynamicsCommon.h>
#include<BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include<BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include<BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>
#include<LinearMath/btThreads.h>

namespace
{
    class BulletRigidBody
    {
    public:
        BulletRigidBody(btCollisionShape* shape, const btTransform& startTrans, float mass, Pitaya::Physics::RigidBodyType type)
            : transform(startTrans), shape(shape), type(type)
        {
            motionState = PITAYA_NEW(btDefaultMotionState, transform);
            btVector3 localInertia(0, 0, 0);
            if (type == Pitaya::Physics::RigidBodyType::Dynamic && mass > 0)
            {
                shape->calculateLocalInertia(mass, localInertia);
            }

            btRigidBody::btRigidBodyConstructionInfo ci(mass, motionState, shape, localInertia);
            body = PITAYA_NEW(btRigidBody, ci);

            switch (type)
            {
                case Pitaya::Physics::RigidBodyType::Static:
                case Pitaya::Physics::RigidBodyType::Dynamic:
                    break;

                case Pitaya::Physics::RigidBodyType::Kinematic:
                    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                    body->setActivationState(DISABLE_DEACTIVATION);
                    break;
            }
        }
        ~BulletRigidBody()
        {
            PITAYA_DELETE(body->getMotionState());

            PITAYA_DELETE(body);
            body = nullptr;
        }

        inline btRigidBody* GetNativeBody()
        {
            return body;
        }
        inline btTransform& GetTransform()
        {
            return transform;
        }
        inline void SetTransform(const btTransform& newTrans)
        {
            transform = newTrans;
            motionState->setWorldTransform(transform);
            body->setWorldTransform(transform);
        }
        inline btVector3 GetPosition() const
        {
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            return trans.getOrigin();
        }
        inline Pitaya::Physics::RigidBodyType GetRigidBodyType() const
        {
            return type;
        }
        inline void SetLinearVelocity(const btVector3& velocity)
        {
            body->setLinearVelocity(velocity);
        }
        inline btVector3 GetLinearVelocity() const
        {
            return body->getLinearVelocity();
        }
        inline void SetAngularVelocity(const btVector3& angVel)
        {
            body->setAngularVelocity(angVel);
        }
        inline btVector3 GetAngularVelocity() const
        {
            return body->getAngularVelocity();
        }
        inline void ApplyCentralForce(const btVector3& force)
        {
            body->applyCentralForce(force);
        }
        inline void ApplyCentralImpulse(const btVector3& impulse)
        {
            body->applyCentralImpulse(impulse);
        }
        inline void SetRotation(const btQuaternion& quat)
        {
            btTransform trans = body->getWorldTransform();
            trans.setRotation(quat);
            body->setWorldTransform(trans);
        }
        inline btQuaternion GetRotation() const
        {
            return body->getWorldTransform().getRotation();
        }
        inline void SetActive(bool isActive)
        {
            if (isActive)
                body->activate();
            else
                body->setActivationState(WANTS_DEACTIVATION);
        }
        inline bool IsActive() const
        {
            return body->isActive();
        }
        inline float GetMass() const
        {
            float invMass = body->getInvMass();
            return invMass > 0.0f ? 1.0f / invMass : 0.0f;
        }
        inline void SetDamping(float linear, float angular)
        {
            body->setDamping(linear, angular);
        }
        inline void ClearForces()
        {
            body->clearForces();
        }
        inline void SetGravity(const btVector3& g)
        {
            body->setGravity(g);
        }
        inline btVector3 GetGravity() const
        {
            return body->getGravity();
        }

    private:
        btTransform transform;
        btCollisionShape* shape = nullptr;
        btDefaultMotionState* motionState = nullptr;
        btRigidBody* body = nullptr;
        Pitaya::Physics::RigidBodyType type;
    };

    class BulletBoxCollision
    {
    public:
        BulletBoxCollision(const btVector3& halfExtents)
        {
            shape = PITAYA_NEW(btBoxShape, halfExtents);
        }
        ~BulletBoxCollision()
        {
            PITAYA_DELETE(shape);
            shape = nullptr;
        }

        inline btCollisionShape* GetbShape()
        {
            return shape;
        }

    private:
        btCollisionShape* shape = nullptr;
    };
    class BulletSphereCollision
    {
    public:
        BulletSphereCollision(float radius)
        {
            shape = PITAYA_NEW(btSphereShape, radius);
        }
        ~BulletSphereCollision()
        {
            PITAYA_DELETE(shape);
            shape = nullptr;
        }

        inline btCollisionShape* GetbShape() const
        {
            return shape;
        }

    private:
        btCollisionShape* shape = nullptr;
    };
    class BulletCapsuleCollision
    {
    public:
        BulletCapsuleCollision(float radius, float height)
        {
            shape = PITAYA_NEW(btCapsuleShape, radius, height);
        }
        ~BulletCapsuleCollision()
        {
            PITAYA_DELETE(shape);
            shape = nullptr;
        }

        inline btCollisionShape* GetbShape()
        {
            return shape;
        }

    private:
        btCollisionShape* shape = nullptr;
    };

    class BulletPhysicsWorld
    {
    public:
        BulletPhysicsWorld(const btVector3& gravity = btVector3(0, -9.8, 0))
        {
            scheduler = btCreateDefaultTaskScheduler();
            btSetTaskScheduler(scheduler);
            collisionConfiguration = new btDefaultCollisionConfiguration();
            dispatcher = new btCollisionDispatcherMt(collisionConfiguration);
            broadphase = new btDbvtBroadphase();
            constraintSolver = new btSequentialImpulseConstraintSolverMt();
            solverPool = new btConstraintSolverPoolMt(0);
            dynamicsWorld = new btDiscreteDynamicsWorldMt(dispatcher, broadphase, solverPool, constraintSolver, collisionConfiguration);
            dynamicsWorld->getSolverInfo().m_minimumSolverBatchSize = 128;
            dynamicsWorld->getSolverInfo().m_numIterations = 10;
            dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_SIMD;
            dynamicsWorld->setInternalTickCallback(InternalTickCallback);
            dynamicsWorld->setGravity(gravity);
        }
        ~BulletPhysicsWorld()
        {
            if (dynamicsWorld)
            {
                PITAYA_DELETE(dynamicsWorld);
                dynamicsWorld = nullptr;
            }
            if (solverPool)
            {
                PITAYA_DELETE(solverPool);
                solverPool = nullptr;
            }
            if (constraintSolver)
            {
                PITAYA_DELETE(constraintSolver);
                constraintSolver = nullptr;
            }
            if (broadphase)
            {
                PITAYA_DELETE(broadphase);
                broadphase = nullptr;
            }
            if (dispatcher)
            {
                PITAYA_DELETE(dispatcher);
                dispatcher = nullptr;
            }
            if (collisionConfiguration)
            {
                PITAYA_DELETE(collisionConfiguration);
                collisionConfiguration = nullptr;
            }
            if (scheduler)
            {
                PITAYA_DELETE(scheduler);
                scheduler = nullptr;
            }
        }

        inline void AddbRigidBody(BulletRigidBody* rb)
        {
            dynamicsWorld->addRigidBody(rb->GetNativeBody());
        }
        inline void RemovebRigidBody(BulletRigidBody* rb)
        {
            dynamicsWorld->removeRigidBody(rb->GetNativeBody());
        }
        inline void Simulation()
        {
            dynamicsWorld->stepSimulation(Pitaya::Time::delta(), Pitaya::Physics::PhysicsSimulator::MaxFixupdataExecuteTimes, Pitaya::Time::Fixdelta());
        }
        inline bool RaycastClosest(const btVector3& from, const btVector3& to, btCollisionWorld::ClosestRayResultCallback& resultOut)
        {
            dynamicsWorld->rayTest(from, to, resultOut);
            return resultOut.hasHit();
        }
        inline btDiscreteDynamicsWorldMt* GetNativeWorld()
        {
            return dynamicsWorld;
        }
        inline void SetGravity(const btVector3& gravity)
        {
            dynamicsWorld->setGravity(gravity);
        }
        inline btVector3 GetGravity() const
        {
            return dynamicsWorld->getGravity();
        }

    private:
        static void InternalTickCallback(btDynamicsWorld* world, btScalar timeStep)
        {
            //TODO 执行脚本的Fixupdata
        }

    private:
        btITaskScheduler* scheduler = nullptr;
        btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
        btCollisionDispatcherMt* dispatcher = nullptr;
        btBroadphaseInterface* broadphase = nullptr;
        btConstraintSolverPoolMt* solverPool = nullptr;
        btConstraintSolver* constraintSolver = nullptr;
        btDiscreteDynamicsWorldMt* dynamicsWorld = nullptr;
    };

    struct BulletPhysicsContext
    {

    };
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
