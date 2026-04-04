#include<Script/ScriptRuntime.h>
#include<Log/Common/FuncTable.h>
#include<Core/Utils/File/File.h>

namespace
{
	static void CSharp_Call_Cpp_LogInfo(MonoString* monoStr)
	{
		// char* cStr = mono_string_to_utf8(monoStr);
		// PITAYA_CORE_INFO("C# 打印: {0}", cStr);
		// mono_free(cStr);
	}
}

bool Pitaya::Script::ScriptRuntime::Initialize()
{
	if (!mono.Initialize()) 
	{
		Pitaya::Log::Error("mono initialize fail");
		return false;
	}

	// TODO 获取加载路径 加载路径可由配置中心读取，此处硬编码做演示
	if (!mono.LoadAssembly("GameBinaries/ScriptLogic.dll")) 
	{ 
		Pitaya::Log::Error("mono load assembly fail");
		return false;
	}

	if (!bridge.Initialize(&mono)) 
	{
		Pitaya::Log::Error("bridge initialize fail");
		return false; 
	}

	return true;
}
void Pitaya::Script::ScriptRuntime::Release()
{
	bridge.Release();
	mono.Release();
}
void Pitaya::Script::ScriptRuntime::BeginFrame() 
{
	bridge.BeginFrame();
}
void Pitaya::Script::ScriptRuntime::FixedUpdate()
{
	bridge.FixedUpdate();
}
void Pitaya::Script::ScriptRuntime::Update()
{
	bridge.Update();
}
void Pitaya::Script::ScriptRuntime::LateUpdate()
{
	bridge.LateUpdate();
}
void Pitaya::Script::ScriptRuntime::EndFrame()
{
	bridge.EndFrame();
}

bool Pitaya::Script::ScriptRuntime::MonoVM::Initialize()
{
	//设置底层库目录
	const std::filesystem::path exedir = std::filesystem::path(Pitaya::Core::GetExeDir());
	std::filesystem::path monoLibPath = exedir / "resource" / "mono" / "lib";
	std::filesystem::path monoEtcPath = exedir / "resource" / "mono" / "etc";
	std::filesystem::path gamelogicPath = exedir / "bin" / "GameLogic.dll";
	mono_set_dirs(monoLibPath.string().c_str(), monoEtcPath.string().c_str());
	mono_set_assemblies_path(gamelogicPath.string().c_str());
	rootDomain = mono_jit_init("PitayaScriptJIT");
	return rootDomain != nullptr;
}
bool Pitaya::Script::ScriptRuntime::MonoVM::LoadAssembly(const char* dllPath)
{
	//保留AppDomain机制 便于未来的dll热更新
	char domainName[] = "PitayaGameDomain";
	appDomain = mono_domain_create_appdomain(domainName, nullptr);
	mono_domain_set(appDomain, true);

	gameAssembly = mono_domain_assembly_open(appDomain, dllPath);
	if (!gameAssembly)
	{
		Pitaya::Log::Error("mono domain assembly open falil, path: " + std::string(dllPath));
		return false;
	}

	gameImage = mono_assembly_get_image(gameAssembly);
	if (!gameImage)
	{
		Pitaya::Log::Error("mono assembly get image fail, path: " + std::string(dllPath));
		return false;
	}

	return true;
}
void Pitaya::Script::ScriptRuntime::MonoVM::Release()
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
}

bool Pitaya::Script::ScriptRuntime::Bridge::Initialize(ScriptRuntime::MonoVM* monoVM)
{
	this->monoVM = monoVM;
	BindEngineAPI();
	CacheScriptAPI();
	return true;
}
void Pitaya::Script::ScriptRuntime::Bridge::Release()
{
	updateMethod = nullptr;
	fixedUpdateMethod = nullptr;
	monoVM = nullptr;
}
void Pitaya::Script::ScriptRuntime::Bridge::BindEngineAPI()
{
	// 注册 C++ 方法给 C# 
	mono_add_internal_call("Pitaya.Core.Log::Info(string)", (void*)CSharp_Call_Cpp_LogInfo);
}
void Pitaya::Script::ScriptRuntime::Bridge::CacheScriptAPI()
{
	MonoImage* image = monoVM->GetGameImage();
	if (!image)
	{
		Pitaya::Log::Error("script bridge get image is empty!");
		return;
	}

	// 假设 C# 端定义了 Pitaya.Logic.GameManager 类及其 Update 方法
	MonoClass* gameMgrClass = mono_class_from_name(image, "Pitaya.Logic", "GameManager");
	if (gameMgrClass)
	{
		updateMethod = mono_class_get_method_from_name(gameMgrClass, "Update", 1);
		fixedUpdateMethod = mono_class_get_method_from_name(gameMgrClass, "FixedUpdate", 1);
	}
}
void Pitaya::Script::ScriptRuntime::Bridge::Update()
{
	if (!updateMethod) return;

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
void Pitaya::Script::ScriptRuntime::Bridge::FixedUpdate()
{
	if (!fixedUpdateMethod) { return; }

	float fixedDt = 0.02f;
	void* args[1] = { &fixedDt };

	mono_runtime_invoke(fixedUpdateMethod, nullptr, args, nullptr);
}
void Pitaya::Script::ScriptRuntime::Bridge::BeginFrame() 
{

}
void Pitaya::Script::ScriptRuntime::Bridge::LateUpdate() 
{

}
void Pitaya::Script::ScriptRuntime::Bridge::EndFrame() 
{

}