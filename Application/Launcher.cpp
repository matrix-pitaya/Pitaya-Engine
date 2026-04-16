#include<Application/Launcher.h>
#include<Engine/API/func.h>
#include<Editor/API/def.h>
#include<Hook/def.h>
#include<Core/Utils/File.h>
#include<Core/Utils/Memory.h>

#define NOMINMAX
#include<windows.h>
#include<mimalloc.h>

#include<exception>
#include<stdexcept>
#include<filesystem>
#include<fstream>

namespace
{
	HMODULE Editordll = nullptr;
	void LoadEditordll()
	{
		DISCARD_HOOK
		if (Editordll) { FreeLibrary(Editordll); Editordll = nullptr; }
		const std::filesystem::path dllPath = Pitaya::Core::GetExecutableDirectory() / "Editor.dll";
		Editordll = LoadLibraryA(dllPath.string().c_str());
		if (Editordll)
		{
			auto func = reinterpret_cast<void(EDITOR_CALL*)()>(GetProcAddress(Editordll, "MountEngineHook"));
			if (func) { func(); }
		}
	}
	void FreeEditordll()
	{
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "hook.state",
			"Pitaya Engine Hook State File", HOOK_STATE);
		DISCARD_HOOK
		if (Editordll) { FreeLibrary(Editordll); Editordll = nullptr;}
	}

	LONG WINAPI SafeTerminateInSEH()
	{
		__try
		{
			Pitaya::Engine::Terminate();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			MessageBoxA(NULL, "Engine Terminate Crash", "Error", MB_OK);
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}
	LONG WINAPI CrashFilter(EXCEPTION_POINTERS*)
	{
		MessageBoxA(NULL, "program crash... TAT", "Error", MB_OK); 
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
			"Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
		SafeTerminateInSEH();
		FreeEditordll();
		return EXCEPTION_EXECUTE_HANDLER;
	}

	void CheckRuntimeEnvironment()
	{
		//Workspace Check
		{
			const std::filesystem::path workspace = Pitaya::Core::GetWorkspace();
			if (__argc > 1)
			{
				std::error_code ec;
				std::filesystem::path inputPath = std::filesystem::absolute(__argv[1]);
				inputPath = std::filesystem::weakly_canonical(inputPath, ec);
				if (!std::filesystem::exists(inputPath, ec) ||
					!std::filesystem::is_regular_file(inputPath, ec) ||
					inputPath.filename() != ".pitaya")
				{
					MessageBoxA(NULL, ("Unable to open this file: " + inputPath.string()).c_str(), "Error", MB_OK);
					exit(-1);
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
					MessageBoxA(NULL, ("Failed to create project marker file!\npath: " + pitayaFile.string() + "\nerror: " + ec.message()).c_str(), "Error", MB_OK);
					exit(-1);
				}
				file.close();
			}

			auto CheckFolder = [](const std::filesystem::path& folder)
				{
					std::error_code ec;
					if (!std::filesystem::create_directories(folder, ec) && ec)
					{
						MessageBoxA(NULL, ("Failed to create directory!\npath:" + folder.string() + "\nerror:" + ec.message()).c_str(), "Error", MB_OK);
						exit(-1);
					}
				};
			CheckFolder(workspace / "lib");
			CheckFolder(workspace / "Asset" / "Resource");
			CheckFolder(workspace / "Asset" / "Script");
			CheckFolder(workspace / "Asset" / "Scene");
		}

		//Resource Check
		{
			const constexpr std::string_view CheckList[] =
			{
				"fonts/fa-solid-900.ttf",
				"fonts/segoeui.ttf",
				"icon/default.png",
				"rendertarget/editor.rt",
				"rendertarget/game.rt",
				"shader/blit/blit.frag",
				"shader/blit/blit.shader",
				"shader/blit/blit.vert",
				"shader/GammaCorrection/GammaCorrection.frag",
				"shader/GammaCorrection/GammaCorrection.shader",
				"shader/GammaCorrection/GammaCorrection.vert",
			};

			const std::filesystem::path folder = Pitaya::Core::GetExecutableDirectory() / "resource";
			for (const std::string_view fileName : CheckList)
			{
				if (fileName.empty()) { continue; }
				std::filesystem::path absPath = folder / fileName;
				if (!std::filesystem::exists(absPath))
				{
					MessageBoxA(NULL, ("miss resource core file: " + absPath.string()).c_str(), "Error", MB_OK);
					exit(-1);
				}
			}
		}

		//Mono Check
		{
			const constexpr std::string_view CheckList[] =
			{
				"bin/mono-2.0-sgen.dll",
				"lib/mono/4.5/mscorlib.dll",	
				"lib/mono/4.5/System.dll",
				"lib/mono/4.5/System.Core.dll",
				"etc/mono/config",
				"etc/mono/4.5/machine.config"
			};

			const std::filesystem::path folder = Pitaya::Core::GetExecutableDirectory() / "mono";
			for (const std::string_view fileName : CheckList)
			{
				if (fileName.empty()) { continue; }
				std::filesystem::path absPath = folder / fileName;
				if (!std::filesystem::exists(absPath))
				{
					MessageBoxA(NULL, ("miss mono core file: " + absPath.string()).c_str(), "Error", MB_OK);
					exit(-1);
				}
			}
		}
	}
}

int Pitaya::Application::Execute()
{
	try
	{
		SetUnhandledExceptionFilter(CrashFilter);
		CheckRuntimeEnvironment();
		LoadEditordll();
		int32_t exitcode = Pitaya::Engine::Execute();
		FreeEditordll();
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
			"Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
		return exitcode;
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Error", MB_OK);
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
			"Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
		SafeTerminateInSEH();
		FreeEditordll();
		return -1;
	}
	catch (...)
	{
		MessageBoxA(NULL, "Unknown error!", "Error", MB_OK);
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
			"Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
		SafeTerminateInSEH();
		FreeEditordll();
		return -1;
	}
}
