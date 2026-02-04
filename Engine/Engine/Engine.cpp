#include"Engine.h"

#include"../Physics/Backend/Bullet/BulletPhysics.h"
#include"../Renderer/Backend/OpenGL/OpenGLRenderer.h"
#include"../Window/Backend/OpenGL/OpenGLWindow.h"

#include"Console/Console.h"

bool Pitaya::Engine::Engine::Initialize()
{
	if (!BindEngineModelBackendAPI() || !CheckEngineModelBackendBindIsSuccess())
	{
		return false;
	}

	input.Initialize();
	thread.Initialize();
	log.Initialize();
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
void Pitaya::Engine::Engine::Tick()
{
	time.Tick();
	input.PrepareNewFrame();
	window->PollEvents();
	input.UpdateSnapshots();
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
	renderer->Release();
	delete renderer;
	renderer = nullptr;

	physics->Release();
	delete physics;
	physics = nullptr;

	log.Release();
	thread.Release();
	input.Release();
}
bool Pitaya::Engine::Engine::BindEngineModelBackendAPI()
{
	//  ≈‰‰÷»æAPI
	switch (config.RendererAPI)
	{
		case Pitaya::Engine::Renderer::API::OpenGL:
			renderer = new Pitaya::Engine::Renderer::OpenGLRenderer();
			window = new Pitaya::Engine::Window::OpenGLWindow();
			break;

		default:
			throw std::exception("Unknown Renderer Backend API");
	}

	//  ≈‰ŒÔ¿ÌAPI
	switch (config.PhysicsAPI)
	{
		case Pitaya::Engine::Physics::API::Bullet:
			physics = new Pitaya::Engine::Physics::BulletPhysics();
			break;

		default:
			throw std::exception("Unknown Physics Backend API");
	}

	return true;
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