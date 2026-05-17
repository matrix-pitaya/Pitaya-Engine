#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/PassKey/PassKey.h>
#include<Core/Color/Color.h>
#include<Serialize/Serializable.h>
#include<Render/Common/API.h>
#include<Physics/Common/API.h>
#include<Window/Common/Platform.h>
#include<GPU/Common/FrameBufferSpecification.h>
#include<Event/Common/Event.h>
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
		struct ConfigInfo final : public Pitaya::Serialize::Serializable
		{
			//Engine
			std::string Name = "Pitaya";
			std::string Version = PITAYA_VERSION;

			//Physics
			Pitaya::Physics::API PhysicsAPI = Pitaya::Physics::API::Bullet;

			//Render
			Pitaya::Render::API RenderAPI = Pitaya::Render::API::OpenGL;
			Pitaya::GPU::FrameBufferSpecification MainSceneSpec = { 1600, 900, 4, Pitaya::GPU::PixelFormat::RGBA16F, Pitaya::GPU::PixelFormat::Depth24_Stencil8 };
			Pitaya::GPU::FrameBufferSpecification MainPingPongSpec = { 1600, 900, 1, Pitaya::GPU::PixelFormat::RGBA16F, Pitaya::GPU::PixelFormat::Invalid };
			Pitaya::GPU::FrameBufferSpecification MainFinalSpec = { 1600, 900, 1, Pitaya::GPU::PixelFormat::RGBA8,    Pitaya::GPU::PixelFormat::Invalid };
			bool EnableVSync = true;

			//Window
			Pitaya::Window::Platform WindowPlatform = Pitaya::Window::Platform::GLFW;
			int32_t WindowWidth = 1600;
			int32_t WindowHeight = 900;

		public:
			void Serialize(Pitaya::Serialize::SerializeContext& context) const override;
			void Deserialize(const Pitaya::Serialize::DeserializeContext& context) override;
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
		inline Pitaya::GPU::FrameBufferSpecification GetMainSceneSpec() const noexcept
		{
			return info.MainSceneSpec;
		}
		inline Pitaya::GPU::FrameBufferSpecification GetMainPingPongSpec() const noexcept
		{
			return info.MainPingPongSpec;
		}
		inline Pitaya::GPU::FrameBufferSpecification GetMainFinalSpec() const noexcept
		{
			return info.MainFinalSpec;
		}
		inline std::string_view GetWindowName()  const noexcept
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
		inline bool GetEnableVSync() const noexcept
		{
			return info.EnableVSync;
		}

	private:
		void OnWindowFramebufferResetSize(Pitaya::Event::Event event);

	private:
		inline static void OnWindowFramebufferResetSize(void* listener, Pitaya::Event::Event event)
		{
			static_cast<Configurator*>(listener)->OnWindowFramebufferResetSize(event);
		}

	private:
		Pitaya::Config::Configurator::ConfigInfo info;
		Pitaya::Event::EventToken windowFramebufferResetSizeToken;

	private:
		inline static constexpr const char* fileName = "engine.cfg";
	};
}
