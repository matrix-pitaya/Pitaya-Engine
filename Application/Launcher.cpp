#include<Application/Launcher.h>
#include<Engine/API/func.h>
#include<Editor/API/def.h>
#include<Hook/def.h>
#include<Core/Utils/File.h>
#include<Core/Utils/Memory.h>
#include<Core/Utils/System.h>

#include<mimalloc.h>
#include<exception>
#include<stdexcept>
#include<filesystem>
#include<fstream>

namespace
{
	void* Editordll = nullptr;
	void AttachEditorEnvironment(int argc, char** argv)
	{
		DISCARD_HOOK
#if defined(PITAYA_PLATFORM_WINDOWS)
		if (Editordll) { FreeLibrary(static_cast<HMODULE>(Editordll)); Editordll = nullptr; }
		const std::filesystem::path dllPath = Pitaya::Core::GetExecutableDirectory() / "Editor.dll";
		Editordll = LoadLibraryA(dllPath.string().c_str());
		if (Editordll)
		{
			auto func = reinterpret_cast<void(EDITOR_CALL*)(int, char**)>(GetProcAddress(static_cast<HMODULE>(Editordll), "AttachRuntimeEnv"));
			if (func) { func(argc, argv); }
		}
#endif
	}
	void DetachEditorEnvironment()
	{
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "hook.state",
			"Pitaya Engine Hook State File", HOOK_STATE);
		DISCARD_HOOK
#if defined(PITAYA_PLATFORM_WINDOWS)
		if (Editordll) { FreeLibrary(static_cast<HMODULE>(Editordll)); Editordll = nullptr; }
#endif
	}
	void SafeTerminateInSEH()
	{
#if defined(PITAYA_PLATFORM_WINDOWS)
		__try
		{
			Pitaya::Engine::Terminate();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			MessageBoxA(NULL, "Engine Terminate Crash", "Error", MB_OK);
		}
#endif
	}
	void SetExceptionCrashFilter()
	{
#if defined(PITAYA_PLATFORM_WINDOWS)
		SetUnhandledExceptionFilter([](EXCEPTION_POINTERS*) -> LONG WINAPI
			{
				Pitaya::Core::PopMessageBox("Error", "program crash... TAT");
				Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
					"Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
				SafeTerminateInSEH();
				DetachEditorEnvironment();
				return EXCEPTION_EXECUTE_HANDLER;
			});
#endif
	}
	void CheckRuntimeEnvironment(int argc, char** argv)
	{
		//Workspace Check
		{
			const std::filesystem::path workspace = Pitaya::Core::GetWorkspace();
			if (argc > 1)
			{
				std::error_code ec;
				std::filesystem::path inputPath = std::filesystem::absolute(argv[1]);
				inputPath = std::filesystem::weakly_canonical(inputPath, ec);
				if (!std::filesystem::exists(inputPath, ec) ||
					!std::filesystem::is_regular_file(inputPath, ec) ||
					inputPath.filename() != ".pitaya")
				{
					Pitaya::Core::PopMessageBox("Error", ("Unable to open this file: " + inputPath.string()).c_str());
					Pitaya::Core::Terminate(-1);
				}
			}

			const std::filesystem::path pitayaFile = workspace / ".pitaya";
			if (!std::filesystem::exists(pitayaFile))
			{
				std::filesystem::create_directories(pitayaFile.parent_path());
				std::ofstream file(pitayaFile);
				if (!file.is_open())
				{
					std::error_code ec(errno, std::generic_category());
					Pitaya::Core::PopMessageBox("Error", ("Failed to create project marker file!\npath: " + pitayaFile.string() + "\nerror: " + ec.message()).c_str());
					Pitaya::Core::Terminate(-1);
				}
				file.close();
			}

			auto CheckFolder = [](const std::filesystem::path& folder)
				{
					std::error_code ec;
					if (!std::filesystem::create_directories(folder, ec) && ec)
					{
						Pitaya::Core::PopMessageBox("Error", ("Failed to create directory!\npath:" + folder.string() + "\nerror:" + ec.message()).c_str());
						Pitaya::Core::Terminate(-1);
					}
				};
			CheckFolder(workspace / "lib");
			CheckFolder(workspace / "Asset" / "Resource");
			CheckFolder(workspace / "Asset" / "Script");
			CheckFolder(workspace / "Asset" / "Scene");
		}

		//Mono Check
		//{
		//	const constexpr std::string_view CheckList[] =
		//	{
		//		"bin/mono-2.0-sgen.dll",
		//		"lib/mono/4.5/mscorlib.dll",	
		//		"lib/mono/4.5/System.dll",
		//		"lib/mono/4.5/System.Core.dll",
		//		"etc/mono/config",
		//		"etc/mono/4.5/machine.config"
		//	};
		//
		//	const std::filesystem::path folder = Pitaya::Core::GetExecutableDirectory() / "mono";
		//	for (const std::string_view fileName : CheckList)
		//	{
		//		if (fileName.empty()) { continue; }
		//		std::filesystem::path absPath = folder / fileName;
		//		if (!std::filesystem::exists(absPath))
		//		{
		//			Pitaya::Core::PopMessageBox("Error", ("miss mono core file: " + absPath.string()).c_str());
		//			Pitaya::Core::Terminate(-1);
		//		}
		//	}
		//}
	}
}

int Pitaya::Application::Execute(int argc, char** argv)
{
	try
	{
		SetExceptionCrashFilter();
		CheckRuntimeEnvironment(argc, argv);
		AttachEditorEnvironment(argc, argv);
		int32_t exitcode = Pitaya::Engine::Execute(argc, argv);
		DetachEditorEnvironment();
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
			"Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
		return exitcode;
	}
	catch (const std::exception& e)
	{
		Pitaya::Core::PopMessageBox("Error", e.what());
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
			"Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
		SafeTerminateInSEH();
		DetachEditorEnvironment();
		return -1;
	}
	catch (...)
	{
		Pitaya::Core::PopMessageBox("Error", "Unknown error!");
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
			"Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
		SafeTerminateInSEH();
		DetachEditorEnvironment();
		return -1;
	}
}
