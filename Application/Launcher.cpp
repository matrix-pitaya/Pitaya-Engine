#include<Application/Launcher.h>
#include<Engine/API/func.h>
#include<Hook/def.h>

#include<mimalloc.h>
#include<windows.h>

#include<exception>
#include<stdexcept>
#include<filesystem>
#include<fstream>

namespace
{
	std::filesystem::path GetExecutableDirectory()
	{
		char path[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, path, MAX_PATH);
		std::filesystem::path exePath(path);
		return exePath.parent_path();
	}
	void WriteFileHeader(std::ofstream& ofs, const std::string& title)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		ofs << "========================================\n";
		ofs << title << "\n";
		ofs << "Generated: " << st.wYear << "-" << st.wMonth << "-" << st.wDay << " "
			<< st.wHour << ":" << st.wMinute << ":" << st.wSecond << "\n";
		ofs << "========================================\n\n";
	}
	void GenerateMemoryAnalysisFile()
	{
		const std::filesystem::path path = GetExecutableDirectory() / "memory.profile";
		std::ofstream ofs(path, std::ios::out | std::ios::trunc);
		if (!ofs.is_open())
		{
			MessageBoxA(NULL, ("Open Memory Analysis File Fail! Path: " + path.string()).c_str(), "Error", MB_OK);
			return;
		}
		WriteFileHeader(ofs,"Pitaya Engine Memory Analysis File");
		std::string memoryState;
		mi_stats_print_out([](const char* message, void* arg)
			{
				if (message == nullptr || arg == nullptr) { return; }
				static_cast<std::string*>(arg)->append(message);
			}, &memoryState);
		ofs << memoryState;
		ofs.flush();
		ofs.close();
	}
	void GenerateEngineHookStateFile()
	{
		const std::filesystem::path path = GetExecutableDirectory() / "hook.state";
		std::ofstream ofs(path, std::ios::out | std::ios::trunc);
		if (!ofs.is_open())
		{
			MessageBoxA(NULL, ("Open Hook State File Fail! Path: " + path.string()).c_str(), "Error", MB_OK);
			return;
		}
		WriteFileHeader(ofs, "Pitaya Engine Hook State File");
		ofs << HOOK_STATE;
		ofs.flush();
		ofs.close();
	}

	void UnMountEngineAllHook()
	{
		MOUNT_BEGINFRAME_HOOK(nullptr)
		MOUNT_BEGINFRAME_HOOK(nullptr)
		MOUNT_FIXEDUPDATE_HOOK(nullptr)
		MOUNT_UPDATE_HOOK(nullptr)
		MOUNT_LATEUPDATE_HOOK(nullptr)
		MOUNT_ENDFRAME_HOOK(nullptr)
		MOUNT_POSTRENDERERINTIALIZE_HOOK(nullptr)
		MOUNT_POSTRENDERERRELEASE_HOOK(nullptr)
		MOUNT_POSTRENDERERPARSECOMMAND_HOOK(nullptr)
		MOUNT_POSTRENDERERSWAPBUFFER_HOOK(nullptr)
		MOUNT_POSTRENDERCONTEXTINITIALIZED_HOOK(nullptr)
		MOUNT_PRERENDERCONTEXTINRELEASED_HOOK(nullptr)
		MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK(nullptr)
		MOUNT_POSTCHRONOMETERTICK_HOOK(nullptr)
	}

	HMODULE editordll = nullptr;
	void LoadEditordll()
	{
		UnMountEngineAllHook();
		if (editordll) { FreeLibrary(editordll); editordll = nullptr; }
		const std::filesystem::path dllPath = GetExecutableDirectory() / "Editor.dll";
		editordll = LoadLibraryA(dllPath.string().c_str());
		if (editordll)
		{
			auto func = reinterpret_cast<void(ENGINE_CALL *)()>(GetProcAddress(editordll, "MountEngineHook"));
			if (func) { func(); }
		}
	}
	void FreeEditordll()
	{
		GenerateEngineHookStateFile();
		UnMountEngineAllHook();
		if (editordll) { FreeLibrary(editordll); editordll = nullptr;}
	}

	LONG WINAPI CrashFilter(EXCEPTION_POINTERS*)
	{
		MessageBoxA(NULL, "program crash... TAT", "Error", MB_OK);
		GenerateMemoryAnalysisFile();
		__try
		{
			Pitaya::Engine::Terminate();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			//防止二次崩溃
		}
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
		GenerateMemoryAnalysisFile();
		return exitcode;
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Error", MB_OK);
		GenerateMemoryAnalysisFile();
		Pitaya::Engine::Terminate();
		FreeEditordll();
		return -1;
	}
	catch (...)
	{
		MessageBoxA(NULL, "Unknown error!", "Error", MB_OK);
		GenerateMemoryAnalysisFile();
		Pitaya::Engine::Terminate();
		FreeEditordll();
		return -1;
	}
}
