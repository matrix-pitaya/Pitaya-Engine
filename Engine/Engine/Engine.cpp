#include<Engine/Engine.h>

#include<Physics/Backend/Bullet/BulletPhysics.h>
#include<Renderer/Backend/OpenGL/OpenGLRenderer.h>
#include<Window/Platform/GLFW/GLFWWindow.h>

#include<Engine/Internal/Input/Input.h>
#include<Engine/Internal/Time/Time.h>
#include<Engine/Internal/Log/Log.h>
#include<Engine/Internal/Event/Event.h>
#include<Engine/Internal/Thread/Thread.h>

#include<Console/Console.h>

bool Pitaya::Engine::Engine::Initialize()
{
	if (!CreateEngineModel())
	{
		return false;
	}

	input->Initialize();
	thread->Initialize();
	log->Initialize();
	physics->Initialize();
	window->Initialize(config.WindowWidth, config.WindowHeight, config.Name.c_str());
	renderer->Bind(window);
	renderer->Initialize();
	
	return true;
}
bool Pitaya::Engine::Engine::IsRunning()
{
	return !window->IsClose();
}
void Pitaya::Engine::Engine::Tick()
{
	time->Tick();
	input->PrepareNewFrame();
	window->PollEvents();
	input->UpdateSnapshots();
}
void Pitaya::Engine::Engine::Fixupdata()
{
	physics->OnFixupdata();
}
void Pitaya::Engine::Engine::Updata()
{
	//TODO GetKeyDown不能一直返回true
	if (input->GetKeyDown(Pitaya::Engine::Input::KeyCode::A))
	{
		Pitaya::Core::Console::Print(Core::Color::Yellow,"A Down");
	}
	if (input->GetKeyPressed(Pitaya::Engine::Input::KeyCode::A))
	{
		Pitaya::Core::Console::Print(Core::Color::Green, "A Prese");
	}
	if (input->GetKeyReleased(Pitaya::Engine::Input::KeyCode::A))
	{
		Pitaya::Core::Console::Print(Core::Color::Blue, "A Released");
	}
}
void Pitaya::Engine::Engine::CoroutineSchedule()
{

}
void Pitaya::Engine::Engine::Lateupdata()
{

}
void Pitaya::Engine::Engine::Renderer()
{
	renderer->BeginRenderFrame();


	renderer->BeginPass(Pitaya::Engine::Renderer::CameraSnapshot());
	//auto meshRenderers = scene->GetMeshRenderer();
	//auto cameraStates = scene->GetCameraState()
	//for(auto cameraState : cameraStates)
	//{
	//	 renderer->BeginPass(cameraState->GetCameraSnapshot());
	//   for(auto meshRenderer : meshRenderers)
	//   {
	//	    if(!cameraState.CanView(meshRenderer->GetAABB()))
	//	    {
	//			continue;
	//		}
	//      
	//      renderer->Submit();
	//   }
	//	 renderer->EndPass();
	//}

	renderer->EndRenderFrame();
}
void Pitaya::Engine::Engine::EndOfFrame()
{
	
}
void Pitaya::Engine::Engine::FrameSync()
{
	time->FrameSync();
}
void Pitaya::Engine::Engine::Release()
{
	renderer->Release();
	physics->Release();
	thread->Release();
	input->Release();
	event->Release();
	time->Release();
	log->Release();

	delete renderer;
	delete physics;
	delete log;
	delete thread;
	delete input;
	delete event;
	delete time;

	renderer = nullptr;
	physics = nullptr;
	log = nullptr;
	thread = nullptr;
	input = nullptr;
	event = nullptr;
	time = nullptr;
}
bool Pitaya::Engine::Engine::CreateEngineModel()
{
	input = new Pitaya::Engine::Internal::Input();
	time = new Pitaya::Engine::Internal::Time();
	event = new Pitaya::Engine::Internal::Event();
	log = new Pitaya::Engine::Internal::Log();
	thread = new Pitaya::Engine::Internal::Thread();

	//适配窗口Platform
	switch (config.WindowPlatform)
	{
		case Pitaya::Engine::Window::Platform::GLFW:
			window = new Pitaya::Engine::Window::GLFWWindow();
			break;

		default:
			throw std::exception("Unknown Window WindowPlatform");
	}

	//适配渲染API
	switch (config.RendererAPI)
	{
		case Pitaya::Engine::Renderer::API::OpenGL:
			renderer = new Pitaya::Engine::Renderer::OpenGLRenderer();
			break;

		default:
			throw std::exception("Unknown Renderer Backend API");
	}

	//适配物理API
	switch (config.PhysicsAPI)
	{
		case Pitaya::Engine::Physics::API::Bullet:
			physics = new Pitaya::Engine::Physics::BulletPhysics();
			break;

		default:
			throw std::exception("Unknown Physics Backend API");
	}

	return CheckEngineModel();
}
bool Pitaya::Engine::Engine::CheckEngineModel()
{
	if (!input)
	{
		throw std::runtime_error("Engine Input module missing!");
	}
	if (!time)
	{
		throw std::runtime_error("Engine Time module missing!");
	}
	if (!event)
	{
		throw std::runtime_error("Engine Event module missing!");
	}
	if (!log)
	{
		throw std::runtime_error("Engine Log module missing!");
	}
	if (!thread)
	{
		throw std::runtime_error("Engine Thread module missing!");
	}
	if (!renderer)
	{
		throw std::runtime_error("Engine Renderer module missing!");
	}
	if (!physics)
	{
		throw std::runtime_error("Engine Physics module missing!");
	}
	if (!window)
	{
		throw std::runtime_error("Engine Window module missing!");
	}

	return true;
}