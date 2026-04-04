#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>

#include<mono/jit/jit.h>
#include<mono/metadata/assembly.h>
#include<mono/metadata/debug-helpers.h>

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
		class MonoVM
		{
		public:
			bool Initialize();
			void Release();

		public:
			bool LoadAssembly(const char* dllPath);

		public:
			inline MonoDomain* GetAppDomain() const noexcept
			{
				return appDomain;
			}
			inline MonoImage* GetGameImage() const noexcept
			{
				return gameImage;
			}

		private:
			MonoDomain* rootDomain = nullptr;
			MonoDomain* appDomain = nullptr;
			MonoAssembly* gameAssembly = nullptr;
			MonoImage* gameImage = nullptr;
		};
		class Bridge
		{
		public:
			bool Initialize(MonoVM* monoVM);
			void Release();

		public:
			void BeginFrame();
			void FixedUpdate();
			void Update();
			void LateUpdate();
			void EndFrame();

		private:
			void BindEngineAPI();
			void CacheScriptAPI();

		private:
			MonoVM* monoVM = nullptr;
			MonoMethod* updateMethod = nullptr;
			MonoMethod* fixedUpdateMethod = nullptr;
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

	public:
		Bridge bridge;
		MonoVM mono;
	};
}