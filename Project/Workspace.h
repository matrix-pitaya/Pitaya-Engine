#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>

#include<fstream>
#include<filesystem>

namespace Pitaya::Project
{
	class Workspace
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<Workspace>;
		private:
			static Workspace* Create()
			{
				return PITAYA_NEW(Workspace);
			}
			static void Destroy(Workspace* workspace)
			{
				PITAYA_DELETE(workspace);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<Workspace>;
		private:
			static bool Initialize(Workspace* workspace, int argc, char** argv)
			{
				return workspace->Initialize(argc, argv);
			}
			static void Release(Workspace* workspace)
			{
				workspace->Release();
			}
		};
	private:
		Workspace() = default;
		~Workspace() = default;

	public:
		Workspace(const Workspace&) = delete;
		Workspace& operator=(const Workspace&) = delete;
		Workspace(Workspace&&) = delete;
		Workspace& operator=(Workspace&&) = delete;

	private:
		bool Initialize(int argc, char** argv);
		void Release();

	private:
		std::filesystem::path rootPath;
		std::filesystem::path pitayaFile;
		std::filesystem::path assetFolder;
		std::filesystem::path resourceFolder;
		std::filesystem::path scriptFolder;
		std::filesystem::path libFolder;
	};
}
