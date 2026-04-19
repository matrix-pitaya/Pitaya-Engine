#include<Engine/Engine.h>
#include<Task/TaskScheduler.h>
#include<Log/Logger.h>
#include<Thread/ThreadTracker.h>
#include<GPU/RHIDevice.h>
#include<Time/Chronometer.h>
#include<Input/InputMonitor.h>
#include<Event/EventDispatcher.h>
#include<Config/Configurator.h>
#include<Window/Frontend/Window.h>
#include<Physics/Frontend/PhysicsSimulator.h>
#include<Render/Frontend/Renderer.h>
#include<Render/RenderPipeline.h>
#include<Asset/AssetHub.h>
#include<Script/ScriptRuntime.h>
#include<Game/GameWorld.h>
#include<Game/Scene/Scene.h>
#include<Game/Component/Transform.h>
#include<Game/Component/MeshRenderer.h>
#include<Game/Component/Camera.h>
#include<Hook/def.h>

#define NOMINMAX
#include<windows.h>

namespace
{
#pragma region Time
	float ENGINE_CALL Ondelta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->Getdelta();
	}
	float ENGINE_CALL OnFixdelta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->GetFixdelta();
	}
	float ENGINE_CALL OnUnscaledDelta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->GetUnscaledDelta();
	}
	float ENGINE_CALL OnTimeScale() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->GetTimeScale();
	}
	float ENGINE_CALL OnFramerate() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->GetFramerate();
	}
	double ENGINE_CALL OnSeconds() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->Seconds();
	}
	int64_t ENGINE_CALL OnMilliseconds() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->Milliseconds();
	}
#pragma endregion


#pragma region Log
	void ENGINE_CALL OnLog(Pitaya::Log::LogLevel level,std::string_view info) noexcept
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::Log::Logger>()->Write(level, info);
	}
#pragma endregion


#pragma region Thread
	Pitaya::Core::Thread::Identifier ENGINE_CALL OnRegisterThread(std::string_view name, void(*Thread)(void*, void*), void* bootstraper, void* args)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Thread::ThreadTracker>()->RegisterThread(name, Thread, bootstraper, args);
	}
	bool ENGINE_CALL OnUnregisterThread(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Thread::ThreadTracker>()->UnregisterThread(id);
	}
	std::string ENGINE_CALL OnGetThreadName(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Thread::ThreadTracker>()->GetThreadName(id);
	}
	bool ENGINE_CALL OnGetThreadIsRunning(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Thread::ThreadTracker>()->GetThreadIsRunning(id);
	}
#pragma endregion


#pragma region Event
	Pitaya::Event::EventToken ENGINE_CALL OnSubscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, const ::Pitaya::Event::Event&), void* listener) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Event::EventDispatcher>()->Subscribe(type, OnCallBack, listener);
	}
	bool ENGINE_CALL OnUnSubscribe(const Pitaya::Event::EventToken& eventToken) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Event::EventDispatcher>()->UnSubscribe(eventToken);
	}
	void ENGINE_CALL OnEmit(const Pitaya::Event::Event& event) noexcept
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::Event::EventDispatcher>()->Emit(event);
	}
#pragma endregion


#pragma region Input
	bool ENGINE_CALL OnGetKeyDown(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Input::InputMonitor>()->GetKeyDown(keyCode);
	}
	bool ENGINE_CALL OnGetKeyPressed(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Input::InputMonitor>()->GetKeyPressed(keyCode);
	}
	bool ENGINE_CALL OnGetKeyReleased(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Input::InputMonitor>()->GetKeyReleased(keyCode);
	}
#pragma endregion


#pragma region Task
	void ENGINE_CALL OnPostJob(std::function<void()> func, std::string_view name) noexcept
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::Task::TaskScheduler>()->PostJob(std::move(func), name);
	}
#pragma endregion


#pragma region Asset
	Pitaya::Core::Asset<Pitaya::Asset::Texture> ENGINE_CALL OnLoadTexture(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::Texture>(guid);
	}
	Pitaya::Core::Asset<Pitaya::Asset::Shader> ENGINE_CALL OnLoadShader(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::Shader>(guid);
	}
	Pitaya::Core::Asset<Pitaya::Asset::Mesh> ENGINE_CALL OnLoadMesh(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::Mesh>(guid);
	}
	Pitaya::Core::Asset<Pitaya::Asset::Material> ENGINE_CALL OnLoadMaterial(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::Material>(guid);
	}
	Pitaya::Core::Asset<Pitaya::Asset::RenderTarget> ENGINE_CALL OnLoadRenderTarget(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::RenderTarget>(guid);
	}
	bool ENGINE_CALL OnGetAssetPathByGUID(Pitaya::Core::GUID guid, std::filesystem::path& out)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->GetAssetPathByGUID(guid, out);
	}
	bool ENGINE_CALL OnGetAssetGUIDByPath(const std::filesystem::path& path, Pitaya::Core::GUID& out)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->GetAssetGUIDByPath(path, out);
	}
	bool ENGINE_CALL OnTransformToVirtualPath(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->TransformToVirtualPath(inputPath, basePath, out_virtualpath);
	}
	bool ENGINE_CALL OnRegisterExternalFile(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath, Pitaya::Core::GUID& out_guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->RegisterExternalFile(inputPath, basePath, out_virtualpath, out_guid);
	}
	void ENGINE_CALL OnSyncAssetToGPU()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->SyncAssetToGPU();
	}
	bool ENGINE_CALL OnIsUploadedToGPU()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->IsUploadedToGPU();
	}
#pragma endregion


#pragma region Config
	Pitaya::Render::API ENGINE_CALL OnGetRenderAPI() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetRenderAPI();
	}
	size_t ENGINE_CALL OnGetMaxFixupdataExecuteTimes() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetMaxFixupdataExecuteTimes();
	}
	uint32_t ENGINE_CALL OnGetMaxBonesPerInstance() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetMaxBonesPerInstance();
	}
	bool ENGINE_CALL OnGetEnableVSync() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetEnableVSync();
	}
#pragma endregion


#pragma region Window
	void* ENGINE_CALL OnGetNativeWindow()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Window::Window>()->GetNativeWindow();
	}
#pragma endregion


#pragma region GPU
	void ENGINE_CALL OnDestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyAllGPUResource(passkey);
	}

	Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> ENGINE_CALL OnCreateVertexArray()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateVertexArray();
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> ENGINE_CALL OnCreateVertexBuffer(float* vertices, uint32_t size)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateVertexBuffer(vertices, size);
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> ENGINE_CALL OnCreateIndexBuffer(uint32_t* indices, uint32_t count)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateIndexBuffer(indices, count);
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::Shader> ENGINE_CALL OnCreateShaderVF(const char* vertexSource, const char* fragmentSource)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateShader(vertexSource, fragmentSource);
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::Shader> ENGINE_CALL OnCreateShaderVFG(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateShader(vertexSource, fragmentSource, geometrySource);
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> ENGINE_CALL OnCreateTexture2D(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateTexture2D(data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> ENGINE_CALL OnCreateTextureCubemap(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateTextureCubemap(datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> ENGINE_CALL OnCreateUniformBuffer(uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateUniformBuffer(size, bindingPoint);
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> ENGINE_CALL OnCreateFrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateFrameBuffer(spec);
	}
	Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> ENGINE_CALL OnCreateShaderStorageBuffer(uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateShaderStorageBuffer(size, bindingPoint);
	}

	bool ENGINE_CALL OnDestroyVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyVertexArray(id);
	}
	bool ENGINE_CALL OnDestroyVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyVertexBuffer(id);
	}
	bool ENGINE_CALL OnDestroyIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyIndexBuffer(id);
	}
	bool ENGINE_CALL OnDestroyShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyShader(id);
	}
	bool ENGINE_CALL OnDestroyTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyTexture2D(id);
	}
	bool ENGINE_CALL OnDestroyTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyTextureCubemap(id);
	}
	bool ENGINE_CALL OnDestroyUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyUniformBuffer(id);
	}
	bool ENGINE_CALL OnDestroyFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyFrameBuffer(id);
	}
	bool ENGINE_CALL OnDestroyShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyShaderStorageBuffer(id);
	}

	Pitaya::GPU::VertexArray* ENGINE_CALL OnGetVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetVertexArray(id);
	}
	Pitaya::GPU::VertexBuffer* ENGINE_CALL OnGetVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetVertexBuffer(id);
	}
	Pitaya::GPU::IndexBuffer* ENGINE_CALL OnGetIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetIndexBuffer(id);
	}
	Pitaya::GPU::Shader* ENGINE_CALL OnGetShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetShader(id);
	}
	Pitaya::GPU::Texture2D* ENGINE_CALL OnGetTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetTexture2D(id);
	}
	Pitaya::GPU::TextureCubemap* ENGINE_CALL OnGetTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetTextureCubemap(id);
	}
	Pitaya::GPU::UniformBuffer* ENGINE_CALL OnGetUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetUniformBuffer(id);
	}
	Pitaya::GPU::FrameBuffer* ENGINE_CALL OnGetFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetFrameBuffer(id);
	}
	Pitaya::GPU::ShaderStorageBuffer* ENGINE_CALL OnGetShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetShaderStorageBuffer(id);
	}
#pragma endregion


#pragma region Game
	inline Pitaya::Game::Scene* ENGINE_CALL OnGetActiveScene() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Game::GameWorld>()->GetActiveScene();
	}
#pragma endregion
}

int Pitaya::Engine::Engine::Execute()
{
	if (!Initialize()) { return -1; }

	while (IsRunning())
	{
		BeginFrame();
		FixedUpdate();
		Update();
		LateUpdate();
		Render();
		EndFrame();
		FrameSync();
	}

	Release();
	return 0;
}
void Pitaya::Engine::Engine::Terminate()
{
	Release();
}

#define MODULE(T) modules.T
#define FUNCTABLE(T) funcTables.T

bool Pitaya::Engine::Engine::Initialize()
{
	//Fill Context
	do
	{
#define FILLCONTEXT(T) Pitaya::Engine::Context::Instance().T = &T

		FILLCONTEXT(MODULE(AssetHub));
		FILLCONTEXT(MODULE(Renderer));
		FILLCONTEXT(MODULE(RenderPipeline));
		FILLCONTEXT(MODULE(PhysicsSimulator));
		FILLCONTEXT(MODULE(Window));
		FILLCONTEXT(MODULE(InputMonitor));
		FILLCONTEXT(MODULE(Chronometer));
		FILLCONTEXT(MODULE(EventDispatcher));
		FILLCONTEXT(MODULE(Logger));
		FILLCONTEXT(MODULE(ThreadTracker));
		FILLCONTEXT(MODULE(TaskScheduler));
		FILLCONTEXT(MODULE(RHIDevice));
		FILLCONTEXT(MODULE(Configurator));
		FILLCONTEXT(MODULE(GameWorld));
		FILLCONTEXT(MODULE(ScriptRuntime));

		FILLCONTEXT(FUNCTABLE(AssetHub));
		FILLCONTEXT(FUNCTABLE(Configurator));
		FILLCONTEXT(FUNCTABLE(EventDispatcher));
		FILLCONTEXT(FUNCTABLE(RHIDevice));
		FILLCONTEXT(FUNCTABLE(InputMonitor));
		FILLCONTEXT(FUNCTABLE(Logger));
		FILLCONTEXT(FUNCTABLE(TaskScheduler));
		FILLCONTEXT(FUNCTABLE(ThreadTracker));
		FILLCONTEXT(FUNCTABLE(Chronometer));
		FILLCONTEXT(FUNCTABLE(Window));
		FILLCONTEXT(FUNCTABLE(GameWorld));

#undef FILLCONTEXT

		if (!Pitaya::Engine::Context::Instance().Check()) { return false; }
	} while (false);

	//Fill FuncTables
	do
	{
#pragma region Asset
		FUNCTABLE(AssetHub).OnLoadTexture = OnLoadTexture;
		FUNCTABLE(AssetHub).OnLoadShader = OnLoadShader;
		FUNCTABLE(AssetHub).OnLoadMesh = OnLoadMesh;
		FUNCTABLE(AssetHub).OnLoadMaterial = OnLoadMaterial;
		FUNCTABLE(AssetHub).OnLoadRenderTarget = OnLoadRenderTarget;
		FUNCTABLE(AssetHub).OnGetAssetPathByGUID = OnGetAssetPathByGUID;
		FUNCTABLE(AssetHub).OnGetAssetGUIDByPath = OnGetAssetGUIDByPath;
		FUNCTABLE(AssetHub).OnTransformToVirtualPath = OnTransformToVirtualPath;
		FUNCTABLE(AssetHub).OnRegisterExternalFile = OnRegisterExternalFile;
		FUNCTABLE(AssetHub).OnSyncAssetToGPU = OnSyncAssetToGPU;
		FUNCTABLE(AssetHub).OnIsUploadedToGPU = OnIsUploadedToGPU;
#pragma endregion


#pragma region Time
		FUNCTABLE(Chronometer).Ondelta = Ondelta;
		FUNCTABLE(Chronometer).OnFixdelta = OnFixdelta;
		FUNCTABLE(Chronometer).OnUnscaledDelta = OnUnscaledDelta;
		FUNCTABLE(Chronometer).OnTimeScale = OnTimeScale;
		FUNCTABLE(Chronometer).OnFramerate = OnFramerate;
		FUNCTABLE(Chronometer).OnMilliseconds = OnMilliseconds;
		FUNCTABLE(Chronometer).OnSeconds = OnSeconds;
#pragma endregion


#pragma region Log
		FUNCTABLE(Logger).OnLog = OnLog;
#pragma endregion


#pragma region Thread
		FUNCTABLE(ThreadTracker).OnRegisterThread = OnRegisterThread;
		FUNCTABLE(ThreadTracker).OnUnregisterThread = OnUnregisterThread;
		FUNCTABLE(ThreadTracker).OnGetThreadName = OnGetThreadName;
		FUNCTABLE(ThreadTracker).OnGetThreadIsRunning = OnGetThreadIsRunning;
#pragma endregion


#pragma region Event
		FUNCTABLE(EventDispatcher).OnSubscribe = OnSubscribe;
		FUNCTABLE(EventDispatcher).OnUnSubscribe = OnUnSubscribe;;
		FUNCTABLE(EventDispatcher).OnEmit = OnEmit;
#pragma endregion


#pragma region Input
		FUNCTABLE(InputMonitor).OnGetKeyDown = OnGetKeyDown;
		FUNCTABLE(InputMonitor).OnGetKeyPressed = OnGetKeyPressed;
		FUNCTABLE(InputMonitor).OnGetKeyReleased = OnGetKeyReleased;
#pragma endregion


#pragma region Task
		FUNCTABLE(TaskScheduler).OnPostJob = OnPostJob;
#pragma endregion


#pragma region Config
		FUNCTABLE(Configurator).OnGetRenderAPI = OnGetRenderAPI;
		FUNCTABLE(Configurator).OnGetMaxFixupdataExecuteTimes = OnGetMaxFixupdataExecuteTimes;
		FUNCTABLE(Configurator).OnGetMaxBonesPerInstance = OnGetMaxBonesPerInstance;
		FUNCTABLE(Configurator).OnGetEnableVSync = OnGetEnableVSync;
#pragma endregion


#pragma region Window
		FUNCTABLE(Window).OnGetNativeWindow = OnGetNativeWindow;
#pragma endregion


#pragma region GPU
		FUNCTABLE(RHIDevice).OnDestroyAllGPUResource = OnDestroyAllGPUResource;

		FUNCTABLE(RHIDevice).OnCreateVertexArray = OnCreateVertexArray;
		FUNCTABLE(RHIDevice).OnCreateVertexBuffer = OnCreateVertexBuffer;
		FUNCTABLE(RHIDevice).OnCreateIndexBuffer = OnCreateIndexBuffer;
		FUNCTABLE(RHIDevice).OnCreateShaderVF = OnCreateShaderVF;
		FUNCTABLE(RHIDevice).OnCreateShaderVFG = OnCreateShaderVFG;
		FUNCTABLE(RHIDevice).OnCreateTexture2D = OnCreateTexture2D;
		FUNCTABLE(RHIDevice).OnCreateTextureCubemap = OnCreateTextureCubemap;
		FUNCTABLE(RHIDevice).OnCreateUniformBuffer = OnCreateUniformBuffer;
		FUNCTABLE(RHIDevice).OnCreateFrameBuffer = OnCreateFrameBuffer;
		FUNCTABLE(RHIDevice).OnCreateShaderStorageBuffer = OnCreateShaderStorageBuffer;

		FUNCTABLE(RHIDevice).OnDestroyVertexArray = OnDestroyVertexArray;
		FUNCTABLE(RHIDevice).OnDestroyVertexBuffer = OnDestroyVertexBuffer;
		FUNCTABLE(RHIDevice).OnDestroyIndexBuffer = OnDestroyIndexBuffer;
		FUNCTABLE(RHIDevice).OnDestroyShader = OnDestroyShader;
		FUNCTABLE(RHIDevice).OnDestroyTexture2D = OnDestroyTexture2D;
		FUNCTABLE(RHIDevice).OnDestroyTextureCubemap = OnDestroyTextureCubemap;
		FUNCTABLE(RHIDevice).OnDestroyUniformBuffer = OnDestroyUniformBuffer;
		FUNCTABLE(RHIDevice).OnDestroyFrameBuffer = OnDestroyFrameBuffer;
		FUNCTABLE(RHIDevice).OnDestroyShaderStorageBuffer = OnDestroyShaderStorageBuffer;

		FUNCTABLE(RHIDevice).OnGetVertexArray = OnGetVertexArray;
		FUNCTABLE(RHIDevice).OnGetVertexBuffer = OnGetVertexBuffer;
		FUNCTABLE(RHIDevice).OnGetIndexBuffer = OnGetIndexBuffer;
		FUNCTABLE(RHIDevice).OnGetShader = OnGetShader;
		FUNCTABLE(RHIDevice).OnGetTexture2D = OnGetTexture2D;
		FUNCTABLE(RHIDevice).OnGetTextureCubemap = OnGetTextureCubemap;
		FUNCTABLE(RHIDevice).OnGetUniformBuffer = OnGetUniformBuffer;
		FUNCTABLE(RHIDevice).OnGetFrameBuffer = OnGetFrameBuffer;
		FUNCTABLE(RHIDevice).OnGetShaderStorageBuffer = OnGetShaderStorageBuffer;
#pragma endregion


#pragma region Game
		FUNCTABLE(GameWorld).OnGetActiveScene = OnGetActiveScene;
#pragma endregion
	} while (false);

	//Check FuncTables
	do
	{
		if (!FUNCTABLE(AssetHub).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [AssetHub]!"); }
		if (!FUNCTABLE(Configurator).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [Configurator]!"); }
		if (!FUNCTABLE(EventDispatcher).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [EventDispatcher]!"); }
		if (!FUNCTABLE(RHIDevice).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [RHIDevice]!"); }
		if (!FUNCTABLE(InputMonitor).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [InputMonitor]!"); }
		if (!FUNCTABLE(Logger).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [Logger]!"); }
		if (!FUNCTABLE(TaskScheduler).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [TaskScheduler]!"); }
		if (!FUNCTABLE(ThreadTracker).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [ThreadTracker]!"); }
		if (!FUNCTABLE(Chronometer).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [Chronometer]!"); }
		if (!FUNCTABLE(Window).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [Window]!"); }
		if (!FUNCTABLE(GameWorld).Check()) { throw std::runtime_error("Engine [FuncTable] Miss [GameWorld]!"); }
	} while (false);

	//Create Modules
	do
	{
		if (!MODULE(Chronometer).Create()) { throw std::runtime_error("Engine [Module] [Chronometer] reate Fail!"); }
		if (!MODULE(TaskScheduler).Create()) { throw std::runtime_error("Engine [Module] [TaskScheduler] Create Fail!"); }
		if (!MODULE(ThreadTracker).Create()) { throw std::runtime_error("Engine [Module] [ThreadTracker] Create Fail!"); }
		if (!MODULE(EventDispatcher).Create()) { throw std::runtime_error("Engine [Module] [EventDispatcher] Create Fail!"); }
		if (!MODULE(Logger).Create()) { throw std::runtime_error("Engine [Module] [Logger] Create Fail!"); }
		if (!MODULE(RHIDevice).Create()) { throw std::runtime_error("Engine [Module] [RHIDevice] Create Fail!"); }
		if (!MODULE(InputMonitor).Create()) { throw std::runtime_error("Engine [Module] [InputMonitor] Create Fail!"); }
		if (!MODULE(AssetHub).Create()) { throw std::runtime_error("Engine [Module] [AssetHub] Create Fail!"); }
		if (!MODULE(RenderPipeline).Create()) { throw std::runtime_error("Engine [Module] [RenderPipeline] Create Fail!"); }
		if (!MODULE(Configurator).Create()) { throw std::runtime_error("Engine [Module] [Configurator] Create Fail!"); }
		if (!MODULE(Renderer).Create(MODULE(Configurator)->GetRenderAPI())) { throw std::runtime_error("Engine [Module] [Renderer] Create Fail!"); }
		if (!MODULE(Window).Create(MODULE(Configurator)->GetWindowPlatform())) { throw std::runtime_error("Engine [Module] [Window] Create Fail!"); }
		if (!MODULE(PhysicsSimulator).Create(MODULE(Configurator)->GetPhysicsAPI())) { throw std::runtime_error("Engine [Module] [PhysicsSimulator] Create Fail!"); }
		if (!MODULE(GameWorld).Create()) { throw std::runtime_error("Engine [Module] [GameWorld] Create Fail!"); }
		if (!MODULE(ScriptRuntime).Create()) { throw std::runtime_error("Engine [Module] [ScriptRuntime] Create Fail!"); }
	} while (false);
	
	//Check Modules
	do
	{
		if (!MODULE(Configurator)) { throw std::runtime_error("Engine [Module] Miss [Configurator]!"); }
		if (!MODULE(EventDispatcher)) { throw std::runtime_error("Engine [Module] Miss [EventDispatcher]!"); }
		if (!MODULE(InputMonitor)) { throw std::runtime_error("Engine [Module] Miss [InputMonitor]!"); }
		if (!MODULE(ThreadTracker)) { throw std::runtime_error("Engine [Module] Miss [ThreadTracker]!"); }
		if (!MODULE(Logger)) { throw std::runtime_error("Engine [Module] Miss [Logger]!"); }
		if (!MODULE(TaskScheduler)) { throw std::runtime_error("Engine [Module] Miss [TaskScheduler]!"); }
		if (!MODULE(AssetHub)) { throw std::runtime_error("Engine [Module] Miss [AssetHub]!"); }
		if (!MODULE(PhysicsSimulator)) { throw std::runtime_error("Engine [Module] Miss [PhysicsSimulator]!"); }
		if (!MODULE(RHIDevice)) { throw std::runtime_error("Engine [Module] Miss [RHIDevice]!"); }
		if (!MODULE(Window)) { throw std::runtime_error("Engine [Module] Miss [Window]!"); }
		if (!MODULE(Renderer)) { throw std::runtime_error("Engine [Module] Miss [Renderer]!"); }
		if (!MODULE(Chronometer)) { throw std::runtime_error("Engine [Module] Miss [Chronometer]!"); }
		if (!MODULE(RenderPipeline)) { throw std::runtime_error("Engine [Module] Miss [RenderPipeline]!"); }
	} while (false);

	//Initialize Modules
	do
	{
		if (!MODULE(Configurator).Initialize()) { throw std::runtime_error("Engine [Configurator] Module Initialize Fail!"); }
		if (!MODULE(EventDispatcher).Initialize()) { throw std::runtime_error("Engine [EventDispatcher] Module Initialize Fail!"); }
		if (!MODULE(InputMonitor).Initialize()) { throw std::runtime_error("Engine [InputMonitor] Module Initialize Fail!"); }
		if (!MODULE(ThreadTracker).Initialize()) { throw std::runtime_error("Engine [ThreadTracker] Module Initialize Fail!"); }
		if (!MODULE(Logger).Initialize()) { throw std::runtime_error("Engine [Logger] Module Initialize Fail!"); }
		if (!MODULE(TaskScheduler).Initialize()) { throw std::runtime_error("Engine [TaskScheduler] Module Initialize Fail!"); }
		if (!MODULE(AssetHub).Initialize()) { throw std::runtime_error("Engine [AssetHub] Module Initialize Fail!"); }
		if (!MODULE(GameWorld).Initialize()) { throw std::runtime_error("Engine [GameWorld] Module Initialize Fail!"); }
		//if (!MODULE(ScriptRuntime).Initialize()) { throw std::runtime_error("Engine [ScriptRuntime] Module Initialize Fail!"); }
		if (!MODULE(PhysicsSimulator).Initialize()) { throw std::runtime_error("Engine [PhysicsSimulator] Module Initialize Fail!"); }
		if (!MODULE(RHIDevice).Initialize()) { throw std::runtime_error("Engine [RHIDevice] Module Initialize Fail!"); }
		if (!MODULE(Window).Initialize(MODULE(Configurator)->GetWindowWidth(), MODULE(Configurator)->GetWindowHeight(), MODULE(Configurator)->GetWindowName().c_str())) { throw std::runtime_error("Engine [Window] Module Initialize Fail!"); }
		if (!MODULE(RenderPipeline).Initialize()) { throw std::runtime_error("[RenderPipeline] Module Initialize Fail!"); }
		if (!MODULE(Renderer).Initialize(MODULE(Window)->GetNativeWindow())) { throw std::runtime_error("[Renderer] Module Initialize Fail!"); }
		if (!MODULE(Chronometer).Initialize()) { throw std::runtime_error("Engine [Chronometer] Module Initialize Fail!"); }
	} while (false);

	return true;
}
bool Pitaya::Engine::Engine::IsRunning() const
{
	return !MODULE(Window)->IsClose();
}
void Pitaya::Engine::Engine::BeginFrame()
{
	INVOKE_PREBEGINFRAME_HOOK

	MODULE(Chronometer)->Tick(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
	MODULE(InputMonitor)->PrepareNewFrame(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
	MODULE(Window)->PollEvents(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
	MODULE(InputMonitor)->UpdateSnapshots(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
}
void Pitaya::Engine::Engine::FixedUpdate()
{
	//TODO 移动至物理调度器内部，内部进行实际次数调度
	INVOKE_PREFIXEDUPDATE_HOOK

	MODULE(PhysicsSimulator).FixedUpdate();
}
void Pitaya::Engine::Engine::Update()
{
	INVOKE_PREUPDATE_HOOK
}
void Pitaya::Engine::Engine::LateUpdate()
{
	INVOKE_PRELATEUPDATE_HOOK

	MODULE(GameWorld).LateUpdate();
}
void Pitaya::Engine::Engine::Render()
{
	MODULE(RenderPipeline)->NewPipeline(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());

	//获取当前激活场景
	if (auto* scene = MODULE(GameWorld)->GetActiveScene())
	{
		//提交MeshRenderer
		for (auto [entity, meshrenderer, transform] : scene->GetGroup<Pitaya::Game::MeshRenderer>(entt::get<Pitaya::Game::Transform>).each())
		{
			const auto& mesh = meshrenderer.GetMesh();
			const auto& materials = meshrenderer.GetMaterials();
			if (mesh.IsReady() && mesh.GetNativeAssetData() && !mesh.GetNativeAssetData()->SubMeshs.empty())
			{
				for (uint32_t i = 0; i < mesh.GetNativeAssetData()->SubMeshs.size(); ++i)
				{
					uint32_t matIndex = mesh.GetNativeAssetData()->SubMeshs[i].MaterialIndex;
					Pitaya::Asset::Material* nativeMaterial =
						(matIndex < materials.size() && materials[matIndex].IsReady()) ? materials[matIndex].GetNativeAssetData() : nullptr;

					MODULE(RenderPipeline)->AddRenderItem(
						Pitaya::Core::PassKey<Pitaya::Engine::Engine>(),
						mesh.GetNativeAssetData(), nativeMaterial,
						meshrenderer.GetLayerMask(), transform.GetWorldMatrix(), i);
				}
			}
			else
			{
				//mesh 还没加载 → 传 nullptr，Submit 会 fallback 到异常立方体
				MODULE(RenderPipeline)->AddRenderItem(
					Pitaya::Core::PassKey<Pitaya::Engine::Engine>(),
					nullptr, nullptr,
					meshrenderer.GetLayerMask(), transform.GetWorldMatrix(), 0);
			}
		}
		
		//提交Pass
		for (auto [entity, transform, camera] : scene->GetView<Pitaya::Game::Transform, Pitaya::Game::Camera>().each())
		{
			if (camera.GetRenderTargetIsReady())
			{
				MODULE(RenderPipeline)->AddRenderPass(Pitaya::Core::PassKey<Pitaya::Engine::Engine>(),
					camera.GetCameraState().BuildSnapshot(transform.GetWorldPosition(), transform.GetWorldForward(), transform.GetWorldUp()),
					camera.GetRenderTarget(), camera.GetPostProcessSetting(), camera.GetCullingMask());
			}
		}
	}

	MODULE(RenderPipeline)->Execute(Pitaya::Core::PassKey<Pitaya::Engine::Engine>(), MODULE(Renderer).GetKernel());
}
void Pitaya::Engine::Engine::EndFrame()
{
	INVOKE_PREENDFRAME_HOOK

	MODULE(GameWorld).EndFrame();
}
void Pitaya::Engine::Engine::FrameSync()
{
	MODULE(Chronometer)->FrameSync(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
}
void Pitaya::Engine::Engine::Release()
{
	//Release Modules
	do
	{
		MODULE(Configurator).Release();
		MODULE(Chronometer).Release();
		MODULE(InputMonitor).Release();
		MODULE(GameWorld).Release();
		//MODULE(ScriptRuntime).Release();
		MODULE(PhysicsSimulator).Release();
		MODULE(TaskScheduler).Release();
		MODULE(RHIDevice).Release();
		MODULE(RenderPipeline).Release();
		MODULE(Renderer).Release();
		MODULE(AssetHub).Release();
		MODULE(Window).Release();
		MODULE(EventDispatcher).Release();
		MODULE(ThreadTracker).Release();
		MODULE(Logger).Release();
	} while (false);

	//Destroy Modules
	do
	{
		MODULE(ScriptRuntime).Destroy();
		MODULE(GameWorld).Destroy();
		MODULE(Chronometer).Destroy();
		MODULE(InputMonitor).Destroy();
		MODULE(PhysicsSimulator).Destroy();
		MODULE(TaskScheduler).Destroy();
		MODULE(RenderPipeline).Destroy();
		MODULE(Renderer).Destroy();
		MODULE(RHIDevice).Destroy();
		MODULE(Configurator).Destroy();
		MODULE(AssetHub).Destroy();
		MODULE(Window).Destroy();
		MODULE(EventDispatcher).Destroy();
		MODULE(ThreadTracker).Destroy();
		MODULE(Logger).Destroy();
	} while (false);

	//UnRegister FuncTables
	do
	{
		FUNCTABLE(AssetHub).UnRegister();
		FUNCTABLE(Configurator).UnRegister();
		FUNCTABLE(EventDispatcher).UnRegister();;
		FUNCTABLE(RHIDevice).UnRegister();
		FUNCTABLE(InputMonitor).UnRegister();
		FUNCTABLE(Logger).UnRegister();
		FUNCTABLE(TaskScheduler).UnRegister();
		FUNCTABLE(ThreadTracker).UnRegister();
		FUNCTABLE(Chronometer).UnRegister();
		FUNCTABLE(Window).UnRegister();
		FUNCTABLE(GameWorld).UnRegister();
	} while (false);

	//UnRegister Context
	Pitaya::Engine::Context::Instance().UnRegister();
}

#undef MODULE
#undef FUNCTABLE

template<>
Pitaya::Engine::Engine& ENGINE_CALL Pitaya::Core::Singleton<Pitaya::Engine::Engine>::Instance()
{
	static Pitaya::Engine::Engine instance;
	return instance;
}
