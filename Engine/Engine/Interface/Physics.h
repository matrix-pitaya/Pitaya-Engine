#pragma once

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Interface
{
	class Physics
	{
		friend class Pitaya::Engine::Engine;
	protected:
		Physics() = default;
		virtual ~Physics() = default;

	public:
		Physics(const Physics&) = delete;
		Physics& operator=(const Physics&) = delete;
		Physics(Physics&&) = delete;
		Physics& operator=(Physics&&) = delete;

	protected:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;
		virtual void OnFixupdata() = 0;
	};
}