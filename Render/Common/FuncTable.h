#pragma once

#include<Core/PassKey/PassKey.h>
#include<Context/Context.h>
#include<Render/Bake-Input/IBLBakeInput.h>
#include<stdexcept>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Render::Renderer>
	{
		friend class Pitaya::Engine::Engine;
	private:
		FuncTable() = default;
		~FuncTable() = default;

	public:
		FuncTable(const FuncTable&) = delete;
		FuncTable& operator=(const FuncTable&) = delete;
		FuncTable(FuncTable&&) = delete;
		FuncTable& operator=(FuncTable&&) = delete;

	public:
		inline bool Verify() const
		{
			if (!OnBakeIBL) { throw std::runtime_error("FuncTable miss [Render::OnBakeIBL] Function!"); }
			return true;
		}
		inline void Nullify() noexcept
		{
			OnBakeIBL = nullptr;
		}

	public:
		inline bool InvokeOnBakeIBL(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const Pitaya::Render::IBLBakeInput& input)
		{
			return OnBakeIBL(passkey, input);
		}

	private:
		bool (ENGINE_CALL *OnBakeIBL)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const Pitaya::Render::IBLBakeInput&) = nullptr;
	};
}

namespace Pitaya::Render
{
	inline bool BakeIBL(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const IBLBakeInput& input)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Render::Renderer>().InvokeOnBakeIBL(passkey, input);
	}
}
