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
			static Workspace* Create(int argc, char** argv)
			{
				Workspace* workspace = PITAYA_NEW(Workspace);
				workspace->Resolve(argc, argv);
				return workspace;
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
			static bool Initialize(Workspace* workspace)
			{
				return workspace->Initialize();
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
		bool Initialize();
		void Release();

	private:
		void Resolve(int argc, char** argv);

	public:
		inline std::filesystem::path GetRootPath() const noexcept
		{
			return rootPath;
		}
		inline std::filesystem::path GetPitayaFilePath() const noexcept
		{
			return pitayaFile;
		}
		inline std::filesystem::path GetAssetFolderPath() const noexcept
		{
			return assetFolder;
		}
		inline std::filesystem::path GetResourceFolderPath() const noexcept
		{
			return resourceFolder;
		}
		inline std::filesystem::path GetScriptFolderPath() const noexcept
		{
			return scriptFolder;
		}
		inline std::filesystem::path GetLibFolderPath() const noexcept
		{
			return libFolder;
		}

	private:
		std::filesystem::path rootPath;
		std::filesystem::path pitayaFile;
		std::filesystem::path assetFolder;
		std::filesystem::path resourceFolder;
		std::filesystem::path scriptFolder;
		std::filesystem::path libFolder;
	};
}
