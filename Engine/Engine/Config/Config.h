#pragma once

#include"Define/Define.h"
#include"Utility/Utility.h"

#include<yaml-cpp/yaml.h>

#include<fstream>
#include<string>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Config
{
	struct Config
	{
		DELETE_COPY_AND_MOVE(Config)

		friend class Pitaya::Engine::Engine;
	private:
		Config()
		{
			Load();
		}
		~Config()
		{
			Save();
		}

	private:
		inline void Load()
		{
			const std::string& path = Pitaya::Core::Utility::GetExeDir() + fileName;
			std::ifstream fin = std::ifstream(path);
			if (!fin.is_open())
			{
				return;
			}

			YAML::Node config = YAML::Load(fin);
			//Engine
			if (auto node = config["Engine"])
			{
				Name = node["Name"].as<std::string>(Name);
			}

			//Physics
			if (auto node = config["Physics"])
			{
				MaxFixupdataExecuteTimes = node["MaxFixupdataExecuteTimes"].as<size_t>(MaxFixupdataExecuteTimes);
				std::string api = node["API"].as<std::string>("Bullet");
				if (api == "Bullet")
				{
					PhysicsAPI = Pitaya::Engine::Physics::PhysicsAPI::Bullet;
				}
				else
				{
					PhysicsAPI = Pitaya::Engine::Physics::PhysicsAPI::Bullet;
				}
			}

			//Renderer
			if (auto node = config["Renderer"])
			{
				std::string api = node["API"].as<std::string>("OpenGL");
				if (api == "OpenGL")
				{
					RendererAPI = Pitaya::Engine::Renderer::RendererAPI::OpenGL;
				}
				else
				{
					RendererAPI = Pitaya::Engine::Renderer::RendererAPI::OpenGL;
				}
			}

			//Window
			if (auto node = config["Window"])
			{
				WindowWidth = node["Width"].as<int>(WindowWidth);
				WindowHeight = node["Height"].as<int>(WindowHeight);
			}
		}
		inline void Save()
		{
			YAML::Node config;
			//Engine
			config["Engine"]["Name"] = Name;

			//Physics
			config["Physics"]["MaxFixupdataExecuteTimes"] = MaxFixupdataExecuteTimes;
			switch (PhysicsAPI)
			{
				case Pitaya::Engine::Physics::PhysicsAPI::Bullet:
					config["Physics"]["API"] = "Bullet";
					break;
				
				default:
					config["Physics"]["API"] = "Bullet";
					break;
			}

			//Renderer
			switch (RendererAPI)
			{
				case Pitaya::Engine::Renderer::RendererAPI::OpenGL:
					config["Renderer"]["API"] = "OpenGL";
					break;

				default:
					config["Renderer"]["API"] = "OpenGL";
					break;
			}
			
			//Window
			config["Window"]["Width"] = WindowWidth;
			config["Window"]["Height"] = WindowHeight;

			const std::string& path = Pitaya::Core::Utility::GetExeDir() + fileName;
			std::ofstream fout = std::ofstream(path);
			if (fout.is_open())
			{
				fout << config;
			}
		}

	public:
		//Engine
		std::string Name = "Pitaya";

		//Physics
		size_t MaxFixupdataExecuteTimes = 5;
		Pitaya::Engine::Physics::PhysicsAPI PhysicsAPI = Pitaya::Engine::Physics::PhysicsAPI::Bullet;

		//Renderer
		Pitaya::Engine::Renderer::RendererAPI RendererAPI = Pitaya::Engine::Renderer::RendererAPI::OpenGL;

		//Window
		int WindowWidth = 1280;
		int WindowHeight = 720;

	private:
		static inline constexpr const char* fileName = "engine.cfg";
	};
}