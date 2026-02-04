#pragma once

namespace Pitaya::Engine::Interface
{
	class Physics
	{
	public:
		Physics() = default;
		virtual ~Physics() = default;
		Physics(const Physics&) = delete;
		Physics& operator=(const Physics&) = delete;
		Physics(Physics&&) = delete;
		Physics& operator=(Physics&&) = delete;

	public:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;
		virtual void OnFixupdata() = 0;
	};
}