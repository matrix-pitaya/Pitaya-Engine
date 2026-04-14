#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>

#include<mono/jit/jit.h>
#include<mono/metadata/assembly.h>
#include<mono/metadata/debug-helpers.h>

#include<filesystem>

namespace Pitaya::Script
{
	class ScriptRuntime
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<ScriptRuntime>;
		private:
			static ScriptRuntime* Create()
			{
				return PITAYA_NEW(ScriptRuntime);
			}
			static void Destroy(ScriptRuntime* scriptRuntime)
			{
				PITAYA_DELETE(scriptRuntime);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<ScriptRuntime>;
		private:
			static bool Initialize(ScriptRuntime* scriptRuntime)
			{
				return scriptRuntime->Initialize();
			}
			static void Release(ScriptRuntime* scriptRuntime)
			{
				scriptRuntime->Release();
			}
			static void BeginFrame(ScriptRuntime* scriptRuntime)
			{
				scriptRuntime->BeginFrame();
			}
			static void EndFrame(ScriptRuntime* scriptRuntime)
			{
				scriptRuntime->EndFrame();
			}
			static void FixedUpdate(ScriptRuntime* scriptRuntime)
			{
				scriptRuntime->FixedUpdate();
			}
			static void Update(ScriptRuntime* scriptRuntime)
			{
				scriptRuntime->Update();
			}
			static void LateUpdate(ScriptRuntime* scriptRuntime)
			{
				scriptRuntime->LateUpdate();
			}
		};

	private:
		ScriptRuntime() = default;
		~ScriptRuntime() = default;

	public:
		ScriptRuntime(const ScriptRuntime&) = delete;
		ScriptRuntime& operator=(const ScriptRuntime&) = delete;
		ScriptRuntime(ScriptRuntime&&) = delete;
		ScriptRuntime& operator=(ScriptRuntime&&) = delete;

	private:
		bool Initialize();
		void Release();
		void BeginFrame();
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void EndFrame();

	private:
		bool InitMonoJit();
		bool LoadAssembly(const std::filesystem::path& dllPath);
		void BindEngineAPI();
		void CacheScriptAPI();
		bool CompileAndReload();	//热重载 先编译 Asset/Script 下的 .cs 为 GameLogic.dll 然后重新加载程序集
		bool ReloadAssembly(const std::filesystem::path& dllPath);	//仅重载已存在的DLL 不编译

	public:
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;
		MonoAssembly* gameAssembly = nullptr;
		MonoImage* gameImage = nullptr;
		MonoMethod* updateMethod = nullptr;
		MonoMethod* fixedUpdateMethod = nullptr;
	};
}