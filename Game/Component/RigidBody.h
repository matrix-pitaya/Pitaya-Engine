#pragma once

#include<Core/Container/SlotMap.h>
#include<Core/StateFlags/StateFlags.h>
#include<Physics/Common/RigidBodyType.h>
#include<Physics/Common/CollisionLayer.h>
#include<Physics/Specific/PhysicsBody.h>
#include<glm.hpp>

namespace Pitaya::Game
{
	struct RigidBody
	{
	public:
		inline Pitaya::Physics::RigidBodyType GetType() const noexcept
		{
			return type;
		}
		inline float GetMass() const noexcept
		{
			return mass;
		}
		inline float GetLinearDamping() const noexcept
		{
			return linearDamping;
		}
		inline float GetAngularDamping() const noexcept
		{
			return angularDamping;
		}
		inline float GetRestitution() const noexcept
		{
			return restitution;
		}
		inline float GetFriction() const noexcept
		{
			return friction;
		}
		inline glm::bvec3 GetLockPosition() const noexcept
		{
			return lockPosition;
		}
		inline glm::bvec3 GetLockRotation() const noexcept
		{
			return lockRotation;
		}
		inline bool GetUseGravity() const noexcept
		{
			return useGravity;
		}
		inline Pitaya::Physics::CollisionLayer GetCollisionLayer() const noexcept
		{
			return collisionLayer.GetEnum();
		}
		inline Pitaya::Physics::CollisionLayer GetCollisionMask() const noexcept
		{
			return collisionMask.GetEnum();
		}
		inline glm::vec3 GetInitialLinearVelocity() const noexcept
		{
			return initialLinearVelocity;
		}
		inline glm::vec3 GetInitialAngularVelocity() const noexcept
		{
			return initialAngularVelocity;
		}
		inline Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle GetNativeBodyHandle() const noexcept
		{
			return nativeBodyHandle;
		}

	public:
		inline void SetType(Pitaya::Physics::RigidBodyType type) noexcept
		{
			this->type = type;
		}
		inline void SetMass(float mass) noexcept
		{
			this->mass = mass;
		}
		inline void SetLinearDamping(float damping) noexcept
		{
			linearDamping = damping;
		}
		inline void SetAngularDamping(float damping) noexcept
		{
			angularDamping = damping;
		}
		inline void SetRestitution(float restitution) noexcept
		{
			this->restitution = restitution;
		}
		inline void SetFriction(float friction) noexcept
		{
			this->friction = friction;
		}
		inline void SetLockPosition(glm::bvec3 lock) noexcept
		{
			lockPosition = lock;
		}
		inline void SetLockRotation(glm::bvec3 lock) noexcept
		{
			lockRotation = lock;
		}
		inline void SetUseGravity(bool use) noexcept
		{
			useGravity = use;
		}
		inline void SetCollisionLayer(Pitaya::Physics::CollisionLayer layer) noexcept
		{
			collisionLayer = layer;
		}
		inline void SetCollisionMask(Pitaya::Physics::CollisionLayer mask) noexcept
		{
			collisionMask = mask;
		}
		inline void SetInitialLinearVelocity(glm::vec3 velocity) noexcept
		{
			initialLinearVelocity = velocity;
		}
		inline void SetInitialAngularVelocity(glm::vec3 velocity) noexcept
		{
			initialAngularVelocity = velocity;
		}
		inline void SetNativeBodyHandle(Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle handle) noexcept
		{
			nativeBodyHandle = handle;
		}

	private:
		float mass = 1.0f;
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;
		float restitution = 0.0f;
		float friction = 0.5f;

		bool useGravity = true;
		glm::bvec3 lockPosition = glm::bvec3(false);
		glm::bvec3 lockRotation = glm::bvec3(false);

		Pitaya::Physics::RigidBodyType type = Pitaya::Physics::RigidBodyType::Static;

		Pitaya::Core::StateFlags<Pitaya::Physics::CollisionLayer> collisionLayer = Pitaya::Physics::CollisionLayer::Default;
		Pitaya::Core::StateFlags<Pitaya::Physics::CollisionLayer> collisionMask = Pitaya::Physics::CollisionLayer::All;

		glm::vec3 initialLinearVelocity = glm::vec3(0.0f);
		glm::vec3 initialAngularVelocity = glm::vec3(0.0f);
		Pitaya::Core::SlotMap<Pitaya::Physics::PhysicsBody>::Handle nativeBodyHandle;
	};
}
