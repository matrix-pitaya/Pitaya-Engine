#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/PassKey/PassKey.h>
#include<Serialize/Serializable.h>
#include<Render/Common/API.h>
#include<Physics/Common/API.h>
#include<Window/Common/Platform.h>
#include<Config/Common/FunctionTable.h>

#include<string>

namespace Pitaya::Config
{
	class Configurator
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<Configurator>;
		private:
			static Configurator* Create()
			{
				Configurator* configurator = PITAYA_NEW(Configurator);
				configurator->Import();
				return configurator;
			}
			static void Destroy(Configurator* configurator)
			{
				PITAYA_DELETE(configurator);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<Configurator>;
		private:
			static bool Initialize(Configurator* configurator)
			{
				return configurator->Initialize();
			}
			static void Release(Configurator* configurator)
			{
				configurator->Release();
			}
		};

	private:
		Configurator() = default;
		~Configurator() = default;

	public:
		Configurator(const Configurator&) = delete;
		Configurator& operator=(const Configurator&) = delete;
		Configurator(Configurator&&) = delete;
		Configurator& operator=(Configurator&&) = delete;

	private:
		struct ConfigInfo : public Pitaya::Serialize::Serializable
		{
			//Engine
			std::string Name = "Pitaya";
			std::string Version = "1.0.0";

			//Physics
			size_t MaxFixupdataExecuteTimes = 5;
			Pitaya::Physics::API PhysicsAPI = Pitaya::Physics::API::Bullet;

			//Render
			Pitaya::Render::API RenderAPI = Pitaya::Render::API::OpenGL;
			bool EnableVSync = true;

			//Window
			Pitaya::Window::Platform WindowPlatform = Pitaya::Window::Platform::GLFW;
			int32_t WindowWidth = 1600;
			int32_t WindowHeight = 900;

			//GPU
			uint32_t MaxInstancesCount = 1000;
			uint32_t MaxBonesPerInstance = 100;

		public:
			void Serialize(Pitaya::Serialize::SerializeContext& context) const override;
			void Deserialize(const Pitaya::Serialize::DeserializeContext& context) override;
		};

	private:
		bool Initialize();
		void Release();

	private:
		bool Import();
		bool Export();

	public:
		inline Pitaya::Render::API GetRenderAPI() const noexcept
		{
			return info.RenderAPI;
		}
		inline Pitaya::Physics::API GetPhysicsAPI() const noexcept
		{
			return info.PhysicsAPI;
		}
		inline Pitaya::Window::Platform GetWindowPlatform() const noexcept
		{
			return info.WindowPlatform;
		}
		inline const std::string& GetWindowName()  const noexcept
		{
			return info.Name;
		}
		inline int32_t GetWindowWidth() const noexcept
		{
			return info.WindowWidth;
		}
		inline int32_t GetWindowHeight() const noexcept
		{
			return info.WindowHeight;
		}
		inline size_t GetMaxFixupdataExecuteTimes() const noexcept
		{
			return info.MaxFixupdataExecuteTimes;
		}
		inline uint32_t GetMaxInstancesCount() const noexcept
		{
			return info.MaxInstancesCount;
		}
		inline uint32_t GetMaxBonesPerInstance() const noexcept
		{
			return info.MaxBonesPerInstance;
		}
		inline bool GetEnableVSync() const noexcept
		{
			return info.EnableVSync;
		}

	private:
		Pitaya::Config::Configurator::ConfigInfo info;

	private:
		inline static constexpr const char* fileName = "engine.cfg";
	};
}
