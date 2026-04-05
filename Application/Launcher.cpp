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

	HMODULE editordll = nullptr;
	void LoadEditordll()
	{
		UnMountEngineAllHook();
		if (editordll) { FreeLibrary(editordll); editordll = nullptr; }
		const std::filesystem::path dllPath = Pitaya::Core::GetExecutableDirectory() / "Editor.dll";
		editordll = LoadLibraryA(dllPath.string().c_str());
		if (editordll)
		{
			auto func = reinterpret_cast<void(EDITOR_CALL *)()>(GetProcAddress(editordll, "MountEngineHook"));
			if (func) { func(); }
		}
	}
	void FreeEditordll()
	{
		Pitaya::Core::GenerateFile("hook.state", "Pitaya Engine Hook State File", HOOK_STATE);
		UnMountEngineAllHook();
		if (editordll) { FreeLibrary(editordll); editordll = nullptr;}
	}

	LONG WINAPI SafeTerminateInSEH()
	{
		__try
		{
			Pitaya::Engine::Terminate();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			//防止二次崩溃，此处仅做极简操作，不要加复杂逻辑
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}
	LONG WINAPI CrashFilter(EXCEPTION_POINTERS*)
	{
		MessageBoxA(NULL, "program crash... TAT", "Error", MB_OK); 
		Pitaya::Core::GenerateFile("memory.profile", "Pitaya Engine Memory Analysis File",
			Pitaya::Core::GetMemoryState().c_str());
		SafeTerminateInSEH();
		FreeEditordll();
		return EXCEPTION_EXECUTE_HANDLER;
	}
}

int Pitaya::Application::Execute(int argc, char** argv)
{
	try
	{
		SetUnhandledExceptionFilter(CrashFilter);
		LoadEditordll();
		int32_t exitcode = Pitaya::Engine::Execute(argc, argv);
		FreeEditordll();
		Pitaya::Core::GenerateFile("memory.profile", "Pitaya Engine Memory Analysis File", 
			Pitaya::Core::GetMemoryState().c_str());
		return exitcode;
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Error", MB_OK);
		Pitaya::Core::GenerateFile("memory.profile", "Pitaya Engine Memory Analysis File", 
			Pitaya::Core::GetMemoryState().c_str());
		SafeTerminateInSEH();
		FreeEditordll();
		return -1;
	}
	catch (...)
	{
		MessageBoxA(NULL, "Unknown error!", "Error", MB_OK);
		Pitaya::Core::GenerateFile("memory.profile", "Pitaya Engine Memory Analysis File", 
			Pitaya::Core::GetMemoryState().c_str());
		SafeTerminateInSEH();
		FreeEditordll();
		return -1;
	}
}
