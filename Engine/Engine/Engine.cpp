#include"Engine.h"

#include"../Physics/Backend/Bullet/BulletPhysics.h"
#include"../Renderer/Backend/OpenGL/OpenGLRenderer.h"
#include"../Window/Backend/OpenGL/OpenGLWindow.h"

bool Pitaya::Engine::Engine::Initialize()
{
	if (!BindEngineModelBackendAPI() || !CheckEngineModelBackendBindIsSuccess())
	{
		return false;
	}

	physics->Initialize();
	renderer->Bind(window);
	window->Initialize(config.WindowWidth, config.WindowHeight, config.Name.c_str());
	renderer->Initialize();
	
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
	delete physics;
	delete renderer;
}
bool Pitaya::Engine::Engine::BindEngineModelBackendAPI()
{
	//  ≈‰‰÷»æAPI
	switch (config.RendererAPI)
	{
		case Pitaya::Engine::Renderer::RendererAPI::OpenGL:
			renderer = new Pitaya::Engine::Renderer::OpenGLRenderer();
			window = new Pitaya::Engine::Window::OpenGLWindow();
			break;

		default:
			throw std::exception("Unknow Renderer Backend API");
	}

	//  ≈‰ŒÔ¿ÌAPI
	switch (config.PhysicsAPI)
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