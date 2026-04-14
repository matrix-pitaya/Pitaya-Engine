#include<Script/ScriptRuntime.h>
#include<Log/Common/FuncTable.h>
#include<Core/Utils/File.h>

#include<fstream>
#include<sstream>
#include<cstdlib>

namespace
{
	static void CSharp_Call_Cpp_LogInfo(MonoString* monoStr)
	{
		if (!monoStr) { return; }
		char* cStr = mono_string_to_utf8(monoStr);
		Pitaya::Log::Info(cStr);
		mono_free(cStr);
	}

	//编译.cs文件函数
	static bool CompileScriptsToDll(const std::filesystem::path& outputDll)
	{
		std::filesystem::path scriptDir = Pitaya::Core::GetWorkspace() / "Asset" / "Script";
		if (!std::filesystem::exists(scriptDir))
		{
			Pitaya::Log::Error("Script directory not found, path: " + scriptDir.string());
			return false;
		}

		std::vector<std::string> sources;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(scriptDir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".cs")
			{
				sources.push_back(entry.path().string());
			}
		}

		if (sources.empty())
		{
			Pitaya::Log::Info("No .cs files found in " + scriptDir.string());
			return false;
		}

		std::filesystem::create_directories(outputDll.parent_path());

		// 定位内置 Mono 编译器
		std::filesystem::path exeDir = Pitaya::Core::GetExecutableDirectory();
		std::filesystem::path monoBin = exeDir / "resource" / "mono" / "bin";
		std::filesystem::path monoExe = monoBin / "mono.exe";
		std::filesystem::path mcsDir = exeDir / "resource" / "mono" / "lib" / "mono" / "4.5";	// mcs.exe 实际位于 lib/mono/4.5 下
		std::filesystem::path mcsExe = mcsDir / "mcs.exe";
		if (!std::filesystem::exists(monoExe) || !std::filesystem::exists(mcsExe))
		{
			Pitaya::Log::Error("Mono compiler not found. mono.exe path: " + monoExe.string() + " mcs.exe path: " + mcsExe.string());
			return false;
		}

		//构建命令：mono.exe mcs.exe -target:library -out:outputDll sources...
		std::stringstream cmd;
		cmd << "\"" << monoExe.string() << "\" \"" << mcsExe.string() << "\" ";
		cmd << "-target:library -out:\"" << outputDll.string() << "\" ";
		cmd << "-lib:\"" << mcsDir.string() << "\" ";	//显式指定框架程序集路径（mcs.exe 所在目录即包含 mscorlib.dll 等）
		for (const auto& src : sources)
		{
			cmd << "\"" << src << "\" ";
		}

		//重定向错误输出到日志文件，便于调试
		std::filesystem::path logFile = outputDll.parent_path() / "compile.log";
		cmd << "> \"" << logFile.string() << "\" 2>&1";

		// TODO 改成弹出进度条
		Pitaya::Log::Info("Compiling scripts...");
		// 调试时可输出完整命令（避免日志太长可注释）
		// Pitaya::Log::Info("Command: " + cmd.str());

		int ret = std::system(cmd.str().c_str());
		if (ret != 0)
		{
			//尝试读取编译日志并输出
			std::ifstream logStream(logFile);
			if (logStream.is_open())
			{
				std::stringstream buffer;
				buffer << logStream.rdbuf();
				Pitaya::Log::Error("Compilation failed with exit code: " + std::to_string(ret) + "\n" + buffer.str());
			}
			else
			{
				Pitaya::Log::Error("Compilation failed with exit code: " + std::to_string(ret));
			}
			return false;
		}

		Pitaya::Log::Info("GameLogic.dll compiled successfully");
		return true;
	}
}

bool Pitaya::Script::ScriptRuntime::Initialize()
{
	if(!InitMonoJit())
	{
		Pitaya::Log::Error("mono jit init fail");
		return false;
	}

	BindEngineAPI();

	std::filesystem::path dllPath = Pitaya::Core::GetWorkspace() / "lib" / "GameLogic.dll";
	if (std::filesystem::exists(dllPath))
	{
		if (!LoadAssembly(dllPath))
		{
			Pitaya::Log::Error("mono load assembly fail");
			return false;
		}
		CacheScriptAPI();
	}
	return true;
}
void Pitaya::Script::ScriptRuntime::Release()
{
	if (appDomain)
	{
		mono_domain_set(rootDomain, true);
		mono_domain_unload(appDomain);
		appDomain = nullptr;
	}

	if (rootDomain)
	{
		mono_jit_cleanup(rootDomain);
		rootDomain = nullptr;
	}

	rootDomain = nullptr;
	appDomain = nullptr;
	gameAssembly = nullptr;
	gameImage = nullptr;
	updateMethod = nullptr;
	fixedUpdateMethod = nullptr;
}
void Pitaya::Script::ScriptRuntime::BeginFrame() 
{
	
}
void Pitaya::Script::ScriptRuntime::FixedUpdate()
{
	if (!fixedUpdateMethod) { return; }

	float fixedDt = 0.02f;
	void* args[1] = { &fixedDt };

	mono_runtime_invoke(fixedUpdateMethod, nullptr, args, nullptr);
}
void Pitaya::Script::ScriptRuntime::Update()
{
	if (!updateMethod) { return; }

	float dt = 0.0166f; // 此处建议接入：Pitaya::Engine::Time::GetDeltaTime()
	void* args[1] = { &dt };

	MonoObject* exc = nullptr;
	mono_runtime_invoke(updateMethod, nullptr, args, &exc);

	if (exc)
	{
		// 发生 C# 异常，通过 mono_print_unhandled_exception 打印或推给引擎 Log
		// mono_print_unhandled_exception(exc);
	}
}
void Pitaya::Script::ScriptRuntime::LateUpdate()
{
	
}
void Pitaya::Script::ScriptRuntime::EndFrame()
{
	
}
bool Pitaya::Script::ScriptRuntime::InitMonoJit()
{
	//Mono 依赖库和配置路径 exe/mono/
	const std::filesystem::path exeDir = Pitaya::Core::GetExecutableDirectory();
	std::filesystem::path monoLibPath = exeDir / "mono" / "lib";
	std::filesystem::path monoEtcPath = exeDir / "mono" / "etc";
	mono_set_dirs(monoLibPath.string().c_str(), monoEtcPath.string().c_str());

	//程序集搜索目录 workspace/lib/
	std::filesystem::path assembliesDir = Pitaya::Core::GetWorkspace() / "lib";
	mono_set_assemblies_path(assembliesDir.string().c_str());
	rootDomain = mono_jit_init("PitayaScriptJIT");
	return rootDomain != nullptr;
}
bool Pitaya::Script::ScriptRuntime::LoadAssembly(const std::filesystem::path& dllPath)
{
	//保留AppDomain机制 便于未来的dll热更新
	char domainName[] = "PitayaGameDomain";
	appDomain = mono_domain_create_appdomain(domainName, nullptr);
	if (!appDomain)
	{
		Pitaya::Log::Error("failed to create app domain");
		return false;
	}
	mono_domain_set(appDomain, true);

	gameAssembly = mono_domain_assembly_open(appDomain, dllPath.string().c_str());
	if (!gameAssembly)
	{
		Pitaya::Log::Error("mono domain assembly open falil, path: " + dllPath.string());
		return false;
	}

	gameImage = mono_assembly_get_image(gameAssembly);
	if (!gameImage)
	{
		Pitaya::Log::Error("mono assembly get image fail, path: " + dllPath.string());
		return false;
	}

	return true;
}
void Pitaya::Script::ScriptRuntime::BindEngineAPI()
{
	// 注册 C++ 方法给 C# 
	mono_add_internal_call("Pitaya.Core.Log::Info(string)", (void*)CSharp_Call_Cpp_LogInfo);
}
void Pitaya::Script::ScriptRuntime::CacheScriptAPI()
{
	if (!gameImage)
	{
		Pitaya::Log::Error("CacheScriptAPI: gameImage is null");
		return;
	}

	// C# 端需定义 Pitaya.Logic.GameManager 类及其 Update/FixedUpdate 方法
	MonoClass* gameMgrClass = mono_class_from_name(gameImage, "Pitaya.Logic", "GameManager");
	if (gameMgrClass)
	{
		updateMethod = mono_class_get_method_from_name(gameMgrClass, "Update", 1);
		fixedUpdateMethod = mono_class_get_method_from_name(gameMgrClass, "FixedUpdate", 1);
	}
	else
	{
		Pitaya::Log::Warning("CacheScriptAPI: class Pitaya.Logic.GameManager not found");
	}
}
bool Pitaya::Script::ScriptRuntime::CompileAndReload()	//TODO 放到Editor.dll ,重新编译成功后调用ScriptRuntime提供的ReloadAssembly接口
{
	std::filesystem::path dllPath = Pitaya::Core::GetWorkspace() / "lib" / "GameLogic.dll";
	return CompileScriptsToDll(dllPath) && ReloadAssembly(dllPath.string().c_str());
}
bool Pitaya::Script::ScriptRuntime::ReloadAssembly(const std::filesystem::path& dllPath)
{
	//卸载旧AppDomain
	if (appDomain)
	{
		mono_domain_set(rootDomain, true);
		mono_domain_unload(appDomain);
		appDomain = nullptr;
		gameAssembly = nullptr;
		gameImage = nullptr;
		updateMethod = nullptr;
		fixedUpdateMethod = nullptr;
	}

	//创建新域并加载
	char domainName[] = "PitayaGameDomain";
	appDomain = mono_domain_create_appdomain(domainName, nullptr);
	if (!appDomain)
	{
		Pitaya::Log::Error("Reload: failed to create app domain");
		return false;
	}
	mono_domain_set(appDomain, true);

	gameAssembly = mono_domain_assembly_open(appDomain, dllPath.string().c_str());
	if (!gameAssembly)
	{
		Pitaya::Log::Error("Reload: failed to open assembly" + dllPath.string());
		return false;
	}

	gameImage = mono_assembly_get_image(gameAssembly);
	if (!gameImage)
	{
		Pitaya::Log::Error("Reload: failed to get image from assembly");
		return false;
	}

	// 重新绑定内部调用（确保新域可用）
	BindEngineAPI();
	CacheScriptAPI();

	Pitaya::Log::Info("Assembly reloaded successfully, path:" + dllPath.string());
	return true;
}
