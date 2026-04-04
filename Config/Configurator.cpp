#include<Config/Configurator.h>
#include<Core/Utils/File/File.h>

bool Pitaya::Config::Configurator::Initialize()
{
	return true;
}
void Pitaya::Config::Configurator::Release()
{

}
bool Pitaya::Config::Configurator::Import(Pitaya::Core::PassKey<Pitaya::Engine::Engine>)
{
	const std::filesystem::path path = Pitaya::Core::GetExecutableDirectory() / fileName;
	if (!std::filesystem::exists(path)) { return false; }
	return info.DeserializeFromFile(path, Pitaya::Serialize::API::YAML);
}
bool Pitaya::Config::Configurator::Export(Pitaya::Core::PassKey<Pitaya::Engine::Engine>)
{
	const std::filesystem::path path = Pitaya::Core::GetExecutableDirectory() / fileName;
	return info.SerializeToFile(path, Pitaya::Serialize::API::YAML);
}

void Pitaya::Config::Configurator::ConfigInfo::Serialize(Pitaya::Serialize::SerializeContext& context) const
{
	//Engine
	auto& engine = context.GetSubContext("Engine");
	engine.Write("Name", Name);
	engine.Write("Version", Version);

	//Physics
	auto& physics = context.GetSubContext("Physics");
	physics.Write("MaxFixupdataExecuteTimes", static_cast<int>(MaxFixupdataExecuteTimes));
	switch (PhysicsAPI)
	{
		case Pitaya::Physics::API::Bullet:
			physics.Write("API", "Bullet");
			break;

		case Pitaya::Physics::API::Unknown:
			physics.Write("API", "Bullet");
			break;

		default:
			physics.Write("API", "Bullet");
			break;
	}

	//Renderer
	auto& renderer = context.GetSubContext("Render");
	switch (RenderAPI)
	{
		case Pitaya::Render::API::OpenGL:
			renderer.Write("API", "OpenGL");
			break;

		case Pitaya::Render::API::Unknown:
			renderer.Write("API", "OpenGL");
			break;

		default:
			renderer.Write("API", "OpenGL");
			break;
	}
	renderer.Write("EnableVSync", EnableVSync);

	//Window
	auto& window = context.GetSubContext("Window");
	switch (WindowPlatform)
	{
		case Pitaya::Window::Platform::GLFW:
			window.Write("Platform", "GLFW");
			break;

		case Pitaya::Window::Platform::Unknown:
			window.Write("Platform", "GLFW");
			break;

		default:
			window.Write("Platform", "GLFW");
			break;

	}
	window.Write("Width", WindowWidth);
	window.Write("Height", WindowHeight);

	//GPU
	auto& gpu = context.GetSubContext("GPU");
	gpu.Write("MaxInstancesCount", MaxInstancesCount);
	gpu.Write("MaxBonesPerInstance", MaxBonesPerInstance);
}
void Pitaya::Config::Configurator::ConfigInfo::Deserialize(const Pitaya::Serialize::DeserializeContext& context)
{
	int _int;
	bool _bool;
	uint32_t _uint32_t;
	std::string str;

	//Engine
	if (context.HasSubContext("Engine"))
	{
		const auto& engine = context.GetSubContext("Engine");

		if (engine.Read("Name", str)) { Name = str; }
		if (engine.Read("Version", str)) { Version = str; }
	}
	
	//Physics
	if (context.HasSubContext("Physics"))
	{
		const auto& physics = context.GetSubContext("Physics");

		if (physics.Read("MaxFixupdataExecuteTimes", _int))
		{
			MaxFixupdataExecuteTimes = static_cast<size_t>(_int);
		}
		if (physics.Read("API", str))
		{
			if (str == "Bullet")
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
		if (renderer.Read("API", str))
		{
			if (str == "OpenGL")
			{
				RenderAPI = Pitaya::Render::API::OpenGL;
			}
			else
			{
				RenderAPI = Pitaya::Render::API::OpenGL;
			}
		}
		if (renderer.Read("EnableVSync",_bool))
		{
			EnableVSync = _bool;
		}
	}

	//Window
	if (context.HasSubContext("Window"))
	{
		const auto& window = context.GetSubContext("Window");

		if (window.Read("Width", _int)) { WindowWidth = _int; }
		if (window.Read("Height", _int)) { WindowHeight = _int; }
		if (window.Read("Platform", str))
		{
			if (str == "GLFW")
			{
				WindowPlatform = Window::Platform::GLFW;
			}
			else
			{
				WindowPlatform = Window::Platform::GLFW;
			}
		}
	}

	//GPU
	if (context.HasSubContext("GPU"))
	{
		const auto& gpu = context.GetSubContext("GPU");
		if (gpu.Read("MaxInstancesCount", _uint32_t)) { MaxInstancesCount = _uint32_t; }
		if (gpu.Read("MaxBonesPerInstance", _uint32_t)) { MaxBonesPerInstance = _uint32_t; }
	}
}
