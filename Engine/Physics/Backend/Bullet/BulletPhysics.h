#pragma once

#include"../../Engine/Engine/Interface/Physics.h"

namespace Pitaya::Engine::Physics
{
	class BulletPhysics : public Pitaya::Engine::Interface::Physics
	{
		DELETE_COPY_AND_MOVE(BulletPhysics)

	public:
		BulletPhysics() = default;
		~BulletPhysics() = default;

		bool Initialize() override;
		void Release() override;
		void OnFixupdata() override;
	};
}