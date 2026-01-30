#pragma once

#include"Define/Define.h"

namespace Pitaya::Engine::Interface
{
	class Physics
	{
		DECLARE_INTERFACE(Physics)

	public:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;
		virtual void OnFixupdata() = 0;

	protected:
		bool isInitialized = false;
		bool isReleased = false;
	};
}