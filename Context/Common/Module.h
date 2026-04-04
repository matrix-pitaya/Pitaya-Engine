#pragma once

#include<utility>

namespace Pitaya::Engine
{
	class Engine;

	template<typename T>
	class Module
	{
		friend class Pitaya::Engine::Engine;
	private:
		Module() = default;
		~Module() = default;

	public:
		Module(const Module&) = delete;
		Module& operator=(const Module&) = delete;
		Module(Module&&) = delete;
		Module& operator=(Module&&) = delete;

	public:
		T* operator->() 
		{ 
			return kernel;
		}
		const T* operator->() const 
		{ 
			return kernel;
		}

	public:
		inline explicit constexpr operator bool() const noexcept
		{
			return kernel != nullptr;
		}

	public:
		inline T* GetKernel() const noexcept
		{
			return kernel;
		}

	private:
		template<typename... Args>
		inline bool Create(Args&&... args)
		{
			kernel = T::Factory::Create(std::forward<Args>(args)...);
			return kernel != nullptr;
		}
		template<typename... Args>
		inline void Destroy(Args&&... args)
		{
			T::Factory::Destroy(kernel, std::forward<Args>(args)...);
			kernel = nullptr;
		}
		template<typename... Args>
		inline bool Initialize(Args&&... args)
		{
			return T::Lifecycle::Initialize(kernel, std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline void BeginFrame(Args&&... args)
		{
			T::Lifecycle::BeginFrame(kernel, std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline void FixedUpdate(Args&&... args)
		{
			T::Lifecycle::FixedUpdate(kernel, std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline void Update(Args&&... args)
		{
			T::Lifecycle::Update(kernel, std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline void LateUpdate(Args&&... args)
		{
			T::Lifecycle::LateUpdate(kernel, std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline void EndFrame(Args&&... args)
		{
			T::Lifecycle::EndFrame(kernel, std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline void Release(Args&&... args)
		{
			T::Lifecycle::Release(kernel, std::forward<Args>(args)...);
		}

	private:
		T* kernel = nullptr;
	};
}