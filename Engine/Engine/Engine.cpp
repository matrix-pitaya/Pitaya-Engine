#include"Engine.h"
#include"Config.h"

#include"../Physics/Backend/Bullet/BulletPhysics.h"
#include"../Renderer/Backend/OpenGL/OpenGLRenderer.h"
#include"../Window/Backend/OpenGL/OpenGLWindow.h"

IMPLEMENT_SINGLETON_CLASS(Pitaya::Engine, Engine);

bool Pitaya::Engine::Engine::Initialize()
{
	if (isInitialized)
	{
		return false;
	}

	if (!BindEngineModelBackendAPI() || !CheckEngineModelBackendBindIsSuccess())
	{
		return false;
	}

	physics->Initialize();
	renderer->Bind(window);
	window->Initialize(1080, 720, "Pitaya");
	renderer->Initialize();
	
	isInitialized = true;
	return true;
}
bool Pitaya::Engine::Engine::IsRunning()
{
	return !window->IsClose();
}
void Pitaya::Engine::Engine::Input()
{
	input.Listen();
}
void Pitaya::Engine::Engine::Tick()
{
	time.Tick();
	window->PollEvents();
}
void Pitaya::Engine::Engine::Fixupdata()
{
	physics->OnFixupdata();
}
void Pitaya::Engine::Engine::Updata()
{

}
void Pitaya::Engine::Engine::CoroutineSchedule()
{

}
void Pitaya::Engine::Engine::Lateupdata()
{

}
void Pitaya::Engine::Engine::Renderer()
{
	renderer->Render();
}
void Pitaya::Engine::Engine::EndOfFrame()
{
	
}
void Pitaya::Engine::Engine::FrameSync()
{
	time.FrameSync();
}
void Pitaya::Engine::Engine::Release()
{
	if (isReleased)
	{
		return;
	}

	delete physics;
	delete renderer;

	isReleased = true;
}
bool Pitaya::Engine::Engine::BindEngineModelBackendAPI()
{
	//  ≈‰‰÷»æAPI
	switch (rendererAPI)
	{
		case Pitaya::Engine::Renderer::RendererAPI::OpenGL:
			renderer = new Pitaya::Engine::Renderer::OpenGLRenderer();
			window = new Pitaya::Engine::Window::OpenGLWindow();
			break;

		default:
			throw std::exception("Unknow Renderer Backend API");
	}

	//  ≈‰ŒÔ¿ÌAPI
	switch (physicsAPI)
	{
		case Pitaya::Engine::Physics::PhysicsAPI::Bullet:
			physics = new Pitaya::Engine::Physics::BulletPhysics();
			break;

		default:
			throw std::exception("Unknow Physics Backend API");
	}

}
bool Pitaya::Engine::Engine::CheckEngineModelBackendBindIsSuccess()
{
	if (!renderer)
	{
		throw std::runtime_error("Engine renderer module missing!");
	}
	if (!physics)
	{
		throw std::runtime_error("Engine physics module missing!");
	}
	if (!window)
	{
		throw std::runtime_error("Engine window module missing!");
	}

	return true;
}