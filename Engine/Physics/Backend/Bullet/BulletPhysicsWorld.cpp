#pragma once

#include<Enum/Enum.h>

#include<Engine/API/Time/Time.h>
#include<Engine/API/Config/Config.h>

#include<btBulletDynamicsCommon.h>
#include<BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include<BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include<BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>
#include<LinearMath/btThreads.h>

namespace Pitaya::Engine::Physics
{
    class BulletRigidBody
    {
    public:
        BulletRigidBody(btCollisionShape* shape, const btTransform& startTrans, float mass, RigidBodyType type)
            : transform(startTrans), shape(shape), type(type)
        {
            motionState = new btDefaultMotionState(transform);
            btVector3 localInertia(0, 0, 0);
            if (type == RigidBodyType::Dynamic && mass > 0)
            {
                shape->calculateLocalInertia(mass, localInertia);
            }

            btRigidBody::btRigidBodyConstructionInfo ci(mass, motionState, shape, localInertia);
            body = new btRigidBody(ci);

            switch (type)
            {
            case RigidBodyType::Static:
            case RigidBodyType::Dynamic:
                break;

            case RigidBodyType::Kinematic:
                body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                body->setActivationState(DISABLE_DEACTIVATION);
                break;
            }
        }
        ~BulletRigidBody()
        {
            delete body->getMotionState();

            delete body;
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
        inline RigidBodyType GetRigidBodyType() const
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
        RigidBodyType type;
    };

    class BulletBoxCollision
    {
    public:
        BulletBoxCollision(const btVector3& halfExtents)
        {
            shape = new btBoxShape(halfExtents);
        }
        ~BulletBoxCollision()
        {
            delete shape;
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
            shape = new btSphereShape(radius);
        }
        ~BulletSphereCollision()
        {
            delete shape;
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
            shape = new btCapsuleShape(radius, height);
        }
        ~BulletCapsuleCollision()
        {
            delete shape;
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
                delete dynamicsWorld;
                dynamicsWorld = nullptr;
            }
            if (solverPool)
            {
                delete solverPool;
                solverPool = nullptr;
            }
            if (constraintSolver)
            {
                delete constraintSolver;
                constraintSolver = nullptr;
            }
            if (broadphase)
            {
                delete broadphase;
                broadphase = nullptr;
            }
            if (dispatcher)
            {
                delete dispatcher;
                dispatcher = nullptr;
            }
            if (collisionConfiguration)
            {
                delete collisionConfiguration;
                collisionConfiguration = nullptr;
            }
            if (scheduler)
            {
                delete scheduler;
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
            dynamicsWorld->stepSimulation(Engine::Time::delta(), Engine::Config::GetMaxFixupdataExecuteTimes(), Engine::Time::Fixdelta());
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
            //TODO Ö´ÐÐ½Å±¾µÄFixupdata
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
}
