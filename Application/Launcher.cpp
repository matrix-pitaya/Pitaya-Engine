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
				std::ofstream file(pitayaFile);
				if (!file.is_open())
				{
					MessageBoxA(NULL, "Failed to create project marker file!", "Error", MB_OK);
					exit(-1);
				}
			}

			auto CheckFolder = [](const std::filesystem::path& folder)
				{
					std::error_code ec;
					if (!std::filesystem::create_directories(folder, ec) && ec)
					{
						MessageBoxA(NULL, ("Failed to create directory: " + folder.string()).c_str(), "Error", MB_OK);
						exit(-1);
					}
				};
			CheckFolder(workspace / "lib");
			CheckFolder(workspace / "Asset" / "Resource");
			CheckFolder(workspace / "Asset" / "Script");
			CheckFolder(workspace / "Asset" / "Scene");
		}

		//Resource/Mono Check
		{
			auto CheckFolder = [](const std::filesystem::path& folder)
				{
					const std::filesystem::path checkListFile = folder / "CheckList.req";
					if (!std::filesystem::exists(checkListFile))
					{
						MessageBoxA(NULL, "miss checklist!", "Error", MB_OK);
						exit(-1);
					}
					std::ifstream inFile(checkListFile);
					if (!inFile.is_open())
					{
						MessageBoxA(NULL, "checklist open fail!", "Error", MB_OK);
						exit(-1);
					}
					std::string line;
					while (std::getline(inFile, line))
					{
						line.erase(line.find_last_not_of(" \n\r\t") + 1);	//去除行末潜在的空白字符
						if (line.empty()) { continue; }
						std::filesystem::path absPath = folder / line;		//拼接绝对路径
						if (!std::filesystem::exists(absPath))				//检测文件是否存在
						{
							MessageBoxA(NULL, ("miss resource file: " + absPath.string()).c_str(), "Error", MB_OK);
							exit(-1);
						}
					}
					inFile.close();
				};
			CheckFolder(Pitaya::Core::GetExecutableDirectory() / "resource");
			CheckFolder(Pitaya::Core::GetExecutableDirectory() / "mono");
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
