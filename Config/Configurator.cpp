#include<Config/Configurator.h>
#include<Core/Utils/File.h>
#include<Log/Common/FuncTable.h>
#include<Event/Common/FuncTable.h>

bool Pitaya::Config::Configurator::Initialize()
{
	windowFramebufferResetSizeToken = Pitaya::Event::Subscribe(
		Pitaya::Event::EventType::WindowFramebufferResetSize,
		&Pitaya::Config::Configurator::OnWindowFramebufferResetSize, this);

	return true;
}
void Pitaya::Config::Configurator::Release()
{
	Pitaya::Event::UnSubscribe(windowFramebufferResetSizeToken);

	if (!Export()) { Pitaya::Log::Error("config export fail!"); }
}
bool Pitaya::Config::Configurator::Import()
{
	const std::filesystem::path path = std::filesystem::path(Pitaya::Core::GetWorkspace()) / fileName;
	if (!std::filesystem::exists(path)) { return false; }
	return info.DeserializeFromFile(path, Pitaya::Serialize::API::YAML);
}
bool Pitaya::Config::Configurator::Export()
{
	const std::filesystem::path path = std::filesystem::path(Pitaya::Core::GetWorkspace()) / fileName;
	return info.SerializeToFile(path, Pitaya::Serialize::API::YAML);
}
void Pitaya::Config::Configurator::OnWindowFramebufferResetSize(const Pitaya::Event::Event& event)
{
	if (event.type != Pitaya::Event::EventType::WindowFramebufferResetSize) { return; }
	const Pitaya::Event::FramebufferResetSizeEventArgs& args = static_cast<const Pitaya::Event::FramebufferResetSizeEventArgs&>(event.args);
	info.WindowWidth = args.width;
	info.WindowHeight = args.height;
}

void Pitaya::Config::Configurator::ConfigInfo::Serialize(Pitaya::Serialize::SerializeContext& context) const
{
	//Engine
	auto& engine = context.GetSubContext("Engine");
	engine.Write("Name", Name);
	engine.Write("Version", Version);

	//Physics
	auto& physics = context.GetSubContext("Physics");
	switch (PhysicsAPI)
	{
		case Pitaya::Physics::API::Bullet:	physics.Write("API", "Bullet");		break;
		case Pitaya::Physics::API::Unknown: physics.Write("API", "Bullet");		break;
		default:							physics.Write("API", "Bullet");		break;
	}

	//Renderer
	auto& renderer = context.GetSubContext("Render");
	switch (RenderAPI)
	{
		case Pitaya::Render::API::OpenGL:	renderer.Write("API", "OpenGL");	break;
		case Pitaya::Render::API::Unknown:	renderer.Write("API", "OpenGL");	break;
		default:							renderer.Write("API", "OpenGL");	break;	
	}
	renderer.Write("EnableVSync", EnableVSync);
	auto SerializeFrameBufferSpecification = [](Pitaya::Serialize::SerializeContext& context, const std::string& name, const Pitaya::GPU::FrameBufferSpecification& spec)
		{
			auto& subContext = context.GetSubContext(name);
			subContext.Write("Width", spec.Width);
			subContext.Write("Height", spec.Height);
			subContext.Write("Samples", spec.Samples);
			subContext.Write("SwapChainTarget", spec.SwapChainTarget);
			subContext.Write("HDR", spec.HDR);
			subContext.Write("HasDepth", spec.HasDepth);
		};
	SerializeFrameBufferSpecification(renderer, "MainSceneSpec", MainSceneSpec);
	SerializeFrameBufferSpecification(renderer, "MainPingPongSpec", MainPingPongSpec);
	SerializeFrameBufferSpecification(renderer, "MainFinalSpec", MainFinalSpec);

	//Window
	auto& window = context.GetSubContext("Window");
	switch (WindowPlatform)
	{
		case Pitaya::Window::Platform::GLFW:	window.Write("Platform", "GLFW");	break;
		case Pitaya::Window::Platform::Unknown:	window.Write("Platform", "GLFW");	break;
		default:								window.Write("Platform", "GLFW");	break;
	}
	window.Write("Width", WindowWidth);
	window.Write("Height", WindowHeight);
}
void Pitaya::Config::Configurator::ConfigInfo::Deserialize(const Pitaya::Serialize::DeserializeContext& context)
{
	int out_int;
	bool out_bool;
	uint32_t out_uint32_t;
	std::string out_str;

	//Engine
	if (context.HasSubContext("Engine"))
	{
		const auto& engine = context.GetSubContext("Engine");

		if (engine.Read("Name", out_str)) { Name = out_str; }
		if (engine.Read("Version", out_str)) { Version = out_str; }
	}
	
	//Physics
	if (context.HasSubContext("Physics"))
	{
		const auto& physics = context.GetSubContext("Physics");
		if (physics.Read("API", out_str))
		{
			if (out_str == "Bullet")
			{
				PhysicsAPI = Physics::API::Bullet;
			}
			else
			{
				PhysicsAPI = Physics::API::Bullet;
			}
		}
	}

	//Renderer
	if (context.HasSubContext("Render"))
	{
		const auto& renderer = context.GetSubContext("Render");
		if (renderer.Read("API", out_str))
		{
			if (out_str == "OpenGL")
			{
				RenderAPI = Pitaya::Render::API::OpenGL;
			}
			else
			{
				RenderAPI = Pitaya::Render::API::OpenGL;
			}
		}
		if (renderer.Read("EnableVSync", out_bool))
		{
			EnableVSync = out_bool;
		}
		auto DeserializeFrameBufferSpecification = [](const Pitaya::Serialize::DeserializeContext& context, const std::string& name, Pitaya::GPU::FrameBufferSpecification& spec)
			{
				if (context.HasSubContext(name))
				{
					const auto& subContext = context.GetSubContext(name);
					int out_int; bool out_bool;
					if (subContext.Read("Width", out_int)) { spec.Width = out_int; }
					if (subContext.Read("Height", out_int)) { spec.Height = out_int; }
					if (subContext.Read("Samples", out_int)) { spec.Samples = out_int; }
					if (subContext.Read("SwapChainTarget", out_bool)) { spec.SwapChainTarget = out_bool; }
					if (subContext.Read("HDR", out_bool)) { spec.HDR = out_bool; }
					if (subContext.Read("HasDepth", out_bool)) { spec.HasDepth = out_bool; }
				}
			};
		DeserializeFrameBufferSpecification(renderer, "MainSceneSpec", MainSceneSpec);
		DeserializeFrameBufferSpecification(renderer, "MainPingPongSpec", MainPingPongSpec);
		DeserializeFrameBufferSpecification(renderer, "MainFinalSpec", MainFinalSpec);
	}

	//Window
	if (context.HasSubContext("Window"))
	{
		const auto& window = context.GetSubContext("Window");

		if (window.Read("Width", out_int)) { WindowWidth = out_int; }
		if (window.Read("Height", out_int)) { WindowHeight = out_int; }
		if (window.Read("Platform", out_str))
		{
			if (out_str == "GLFW")
			{
				WindowPlatform = Window::Platform::GLFW;
			}
			else
			{
				WindowPlatform = Window::Platform::GLFW;
			}
		}
	}
}
