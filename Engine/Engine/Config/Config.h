#pragma once

#include"Utility/Utility.h"

#include<yaml-cpp/yaml.h>

#include<fstream>
#include<string>
#include<filesystem>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Config
{
	struct Config
	{
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
		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;
		Config(Config&&) = delete;
		Config& operator=(Config&&) = delete;

	private:
		inline void Load()
		{
			std::filesystem::path path = std::filesystem::path(Pitaya::Core::Utility::GetExeDir()) / fileName;
			std::ifstream fin;
			fin.open(path);
			if (!fin.is_open())
			{
				return;
			}

			YAML::Node config = YAML::Load(fin);
			//Engine
			if (auto node = config["Engine"])
			{
				if (auto value = node["Name"])
				{
					Name = value.as<std::string>(Name);
				}
				if (auto value = node["Version"])
				{
					Version = value.as<std::string>(Version);
				}
			}

			//Physics
			if (auto node = config["Physics"])
			{
				if (auto value = node["MaxFixupdataExecuteTimes"])
				{
					MaxFixupdataExecuteTimes = value.as<size_t>(MaxFixupdataExecuteTimes);
				}
				if (auto value = node["API"])
				{
					std::string api = value.as<std::string>("Bullet");
					if (api == "Bullet")
					{
						PhysicsAPI = Pitaya::Engine::Physics::PhysicsAPI::Bullet;
					}
					else
					{
						PhysicsAPI = Pitaya::Engine::Physics::PhysicsAPI::Bullet;
					}
				}
			}

			//Renderer
			if (auto node = config["Renderer"])
			{
				if (auto value = node["API"])
				{
					std::string api = value.as<std::string>("OpenGL");
					if (api == "OpenGL")
					{
						RendererAPI = Pitaya::Engine::Renderer::RendererAPI::OpenGL;
					}
					else
					{
						RendererAPI = Pitaya::Engine::Renderer::RendererAPI::OpenGL;
					}
				}
			}

			//Window
			if (auto node = config["Window"])
			{
				if (auto value = node["Width"])
				{
					WindowWidth = value.as<int>(WindowWidth);
				}
				if (auto value = node["Height"])
				{
					WindowHeight = value.as<int>(WindowHeight);
				}
			}
		}
		inline void Save()
		{
			YAML::Node config;
			//Engine
			config["Engine"]["Name"] = Name;
			config["Engine"]["Version"] = Version;

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

			std::filesystem::path path = std::filesystem::path(Pitaya::Core::Utility::GetExeDir()) / fileName;
			std::ofstream fout;
			fout.open(path, std::ios::out | std::ios::trunc);
			if (fout.is_open())
			{
				fout << config;
			}
			fout.flush();
			fout.close();
		}

	public:
		//Engine
		std::string Name = "Pitaya";
		std::string Version = "1.0";

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