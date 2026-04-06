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
	void CheckParameterValidity()
	{
		std::filesystem::path workspace = Pitaya::Core::GetWorkspace();
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

		//创建标识文件
		std::filesystem::path pitayaFile = workspace / ".pitaya";
		if (!std::filesystem::exists(pitayaFile))
		{
			std::ofstream file(pitayaFile);
			if (!file.is_open())
			{
				MessageBoxA(NULL, "Failed to create project marker file!", "Error", MB_OK);
				exit(-1);
			}
		}

		//创建目录结构
		std::filesystem::create_directories(workspace / "lib");
		std::filesystem::create_directories(workspace / "Asset" / "Resource");
		std::filesystem::create_directories(workspace / "Asset" / "Script");
	}
	void UnMountEngineAllHook()
	{
		MOUNT_PREBEGINFRAME_HOOK(nullptr, nullptr)
		MOUNT_PREFIXEDUPDATE_HOOK(nullptr, nullptr)
		MOUNT_PREUPDATE_HOOK(nullptr, nullptr)
		MOUNT_PRELATEUPDATE_HOOK(nullptr, nullptr)
		MOUNT_PREENDFRAME_HOOK(nullptr, nullptr)
		MOUNT_POSTRENDERERINTIALIZE_HOOK(nullptr, nullptr)
		MOUNT_POSTRENDERERRELEASE_HOOK(nullptr, nullptr)
		MOUNT_POSTRENDERERPARSECOMMAND_HOOK(nullptr, nullptr)
		MOUNT_POSTRENDERERSWAPBUFFER_HOOK(nullptr, nullptr)
		MOUNT_POSTRENDERERBEGINRENDERFRAME_HOOK(nullptr, nullptr)
		MOUNT_PRERENDERERENDRENDERFRAME_HOOK(nullptr, nullptr)
		MOUNT_POSTRENDERCONTEXTINITIALIZED_HOOK(nullptr, nullptr)
		MOUNT_PRERENDERCONTEXTINRELEASED_HOOK(nullptr, nullptr)
		MOUNT_PRERENDERPIPELINEEXECUTE_HOOK(nullptr, nullptr)
		MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK(nullptr, nullptr)
		MOUNT_POSTCHRONOMETERTICK_HOOK(nullptr, nullptr)
		MOUNT_POSTLOG_HOOK(nullptr, nullptr)
	}

	HMODULE Editordll = nullptr;
	void LoadEditordll()
	{
		UnMountEngineAllHook();
		if (Editordll) { FreeLibrary(Editordll); Editordll = nullptr; }
		const std::filesystem::path dllPath = Pitaya::Core::GetExecutableDirectory() / "Editor.dll";
		Editordll = LoadLibraryA(dllPath.string().c_str());
		if (Editordll)
		{
			auto func = reinterpret_cast<void(EDITOR_CALL *)()>(GetProcAddress(Editordll, "MountEngineHook"));
			if (func) { func(); }
		}
	}
	void FreeEditordll()
	{
		Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "hook.state",
			"Pitaya Engine Hook State File", HOOK_STATE);
		UnMountEngineAllHook();
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
}

int Pitaya::Application::Execute()
{
	try
	{
		SetUnhandledExceptionFilter(CrashFilter);
		CheckParameterValidity();
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
