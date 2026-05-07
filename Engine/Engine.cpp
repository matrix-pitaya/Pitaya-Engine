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
#include<Game/Component/MaterialOverride.h>
#include<Game/Component/Disabled.h>
#include<Game/Component/Light.h>
#include<Hook/def.h>

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
	Pitaya::Event::EventToken ENGINE_CALL OnSubscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, Pitaya::Event::Event), void* listener) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Event::EventDispatcher>()->Subscribe(type, OnCallBack, listener);
	}
	bool ENGINE_CALL OnUnSubscribe(Pitaya::Event::EventToken eventToken) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Event::EventDispatcher>()->UnSubscribe(eventToken);
	}
	void ENGINE_CALL OnEmit(Pitaya::Event::Event event) noexcept
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
	void ENGINE_CALL OnSyncAssetToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->SyncAssetToGPU(passkey);
	}
	bool ENGINE_CALL OnIsUploadedToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->IsUploadedToGPU(passkey);
	}
#pragma endregion


#pragma region Config
	Pitaya::Render::API ENGINE_CALL OnGetRenderAPI() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetRenderAPI();
	}
	bool ENGINE_CALL OnGetEnableVSync() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetEnableVSync();
	}
	Pitaya::GPU::FrameBufferSpecification OnGetMainSceneSpec() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetMainSceneSpec();
	}
	Pitaya::GPU::FrameBufferSpecification OnGetMainPingPongSpec() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetMainPingPongSpec();
	}
	Pitaya::GPU::FrameBufferSpecification OnGetMainFinalSpec() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetMainFinalSpec();
	}
#pragma endregion


#pragma region Window
	void* ENGINE_CALL OnGetNativeWindow()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Window::Window>()->GetNativeWindow();
	}
	glm::uvec2 ENGINE_CALL OnGetWindowSize()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Window::Window>()->GetWindowSize();
	}
#pragma endregion


#pragma region GPU
	void ENGINE_CALL OnDestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyAllGPUResource(passkey);
	}

	bool ENGINE_CALL OnLinkVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle vaoHandle,
		Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle vboHandle, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle eboHandle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->LinkVertexArray(passkey, vaoHandle, vboHandle, eboHandle);
	}

	Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle ENGINE_CALL OnCreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::VertexArray>(passkey);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle ENGINE_CALL OnCreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, float* vertices, uint32_t size, Pitaya::GPU::BufferLayout layout)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::VertexBuffer>(passkey, vertices, size, layout);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle ENGINE_CALL OnCreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t* indices, uint32_t count)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::IndexBuffer>(passkey, indices, count);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle ENGINE_CALL OnCreateShaderVF(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::Shader>(passkey, vertexSource, fragmentSource);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle ENGINE_CALL OnCreateShaderVFG(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::Shader>(passkey, vertexSource, fragmentSource, geometrySource);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle ENGINE_CALL OnCreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::Texture2D>(passkey, data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle ENGINE_CALL OnCreateTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::TextureCubemap>(passkey, datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle ENGINE_CALL OnCreateTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, int width, int height, int layers, bool isDepth)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::Texture2DArray>(passkey, width, height, layers, isDepth);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle ENGINE_CALL OnCreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::UniformBuffer>(passkey, size, bindingPoint);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle ENGINE_CALL OnCreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const Pitaya::GPU::FrameBufferSpecification& spec)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::FrameBuffer>(passkey, spec);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle ENGINE_CALL OnCreateEmptyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::FrameBuffer>(passkey);
	}
	Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle ENGINE_CALL OnCreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Create<Pitaya::GPU::ShaderStorageBuffer>(passkey, size, bindingPoint);
	}

	bool ENGINE_CALL OnDestroyVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::VertexArray>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::VertexBuffer>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::IndexBuffer>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::Shader>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::Texture2D>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::TextureCubemap>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::Texture2DArray>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::UniformBuffer>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::FrameBuffer>(passkey, handle);
	}
	bool ENGINE_CALL OnDestroyShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Destroy<Pitaya::GPU::ShaderStorageBuffer>(passkey, handle);
	}

	bool ENGINE_CALL OnGetVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle, Pitaya::GPU::VertexArray& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::VertexArray>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle, Pitaya::GPU::VertexBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::VertexBuffer>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle, Pitaya::GPU::IndexBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::IndexBuffer>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle, Pitaya::GPU::Shader& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::Shader>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle, Pitaya::GPU::Texture2D& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::Texture2D>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle, Pitaya::GPU::TextureCubemap& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::TextureCubemap>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle, Pitaya::GPU::Texture2DArray& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::Texture2DArray>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle, Pitaya::GPU::UniformBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::UniformBuffer>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle, Pitaya::GPU::FrameBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::FrameBuffer>(passkey, handle, outItem);
	}
	bool ENGINE_CALL OnGetShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle, Pitaya::GPU::ShaderStorageBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->Get<Pitaya::GPU::ShaderStorageBuffer>(passkey, handle, outItem);
	}
#pragma endregion


#pragma region Game
	inline Pitaya::Game::Scene* ENGINE_CALL OnGetActiveScene() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Game::GameWorld>()->GetActiveScene();
	}
#pragma endregion
}

int Pitaya::Engine::Engine::Execute(int argc, char** argv)
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

		if (!Pitaya::Engine::Context::Instance().Verify()) { return false; }
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
		FUNCTABLE(Configurator).OnGetEnableVSync = OnGetEnableVSync;
		FUNCTABLE(Configurator).OnGetMainSceneSpec = OnGetMainSceneSpec;
		FUNCTABLE(Configurator).OnGetMainPingPongSpec = OnGetMainPingPongSpec;
		FUNCTABLE(Configurator).OnGetMainFinalSpec = OnGetMainFinalSpec;
#pragma endregion


#pragma region Window
		FUNCTABLE(Window).OnGetNativeWindow = OnGetNativeWindow;
		FUNCTABLE(Window).OnGetWindowSize = OnGetWindowSize;
#pragma endregion


#pragma region GPU
		FUNCTABLE(RHIDevice).OnDestroyAllGPUResource = OnDestroyAllGPUResource;

		FUNCTABLE(RHIDevice).OnLinkVertexArray = OnLinkVertexArray;

		FUNCTABLE(RHIDevice).OnCreateVertexArray = OnCreateVertexArray;
		FUNCTABLE(RHIDevice).OnCreateVertexBuffer = OnCreateVertexBuffer;
		FUNCTABLE(RHIDevice).OnCreateIndexBuffer = OnCreateIndexBuffer;
		FUNCTABLE(RHIDevice).OnCreateShaderVF = OnCreateShaderVF;
		FUNCTABLE(RHIDevice).OnCreateShaderVFG = OnCreateShaderVFG;
		FUNCTABLE(RHIDevice).OnCreateTexture2D = OnCreateTexture2D;
		FUNCTABLE(RHIDevice).OnCreateTextureCubemap = OnCreateTextureCubemap;
		FUNCTABLE(RHIDevice).OnCreateTexture2DArray = OnCreateTexture2DArray;
		FUNCTABLE(RHIDevice).OnCreateUniformBuffer = OnCreateUniformBuffer;
		FUNCTABLE(RHIDevice).OnCreateFrameBuffer = OnCreateFrameBuffer;
		FUNCTABLE(RHIDevice).OnCreateEmptyFrameBuffer = OnCreateEmptyFrameBuffer;
		FUNCTABLE(RHIDevice).OnCreateShaderStorageBuffer = OnCreateShaderStorageBuffer;

		FUNCTABLE(RHIDevice).OnDestroyVertexArray = OnDestroyVertexArray;
		FUNCTABLE(RHIDevice).OnDestroyVertexBuffer = OnDestroyVertexBuffer;
		FUNCTABLE(RHIDevice).OnDestroyIndexBuffer = OnDestroyIndexBuffer;
		FUNCTABLE(RHIDevice).OnDestroyShader = OnDestroyShader;
		FUNCTABLE(RHIDevice).OnDestroyTexture2D = OnDestroyTexture2D;
		FUNCTABLE(RHIDevice).OnDestroyTextureCubemap = OnDestroyTextureCubemap;
		FUNCTABLE(RHIDevice).OnDestroyTexture2DArray = OnDestroyTexture2DArray;
		FUNCTABLE(RHIDevice).OnDestroyUniformBuffer = OnDestroyUniformBuffer;
		FUNCTABLE(RHIDevice).OnDestroyFrameBuffer = OnDestroyFrameBuffer;
		FUNCTABLE(RHIDevice).OnDestroyShaderStorageBuffer = OnDestroyShaderStorageBuffer;

		FUNCTABLE(RHIDevice).OnGetVertexArray = OnGetVertexArray;
		FUNCTABLE(RHIDevice).OnGetVertexBuffer = OnGetVertexBuffer;
		FUNCTABLE(RHIDevice).OnGetIndexBuffer = OnGetIndexBuffer;
		FUNCTABLE(RHIDevice).OnGetShader = OnGetShader;
		FUNCTABLE(RHIDevice).OnGetTexture2D = OnGetTexture2D;
		FUNCTABLE(RHIDevice).OnGetTextureCubemap = OnGetTextureCubemap;
		FUNCTABLE(RHIDevice).OnGetTexture2DArray = OnGetTexture2DArray;
		FUNCTABLE(RHIDevice).OnGetUniformBuffer = OnGetUniformBuffer;
		FUNCTABLE(RHIDevice).OnGetFrameBuffer = OnGetFrameBuffer;
		FUNCTABLE(RHIDevice).OnGetShaderStorageBuffer = OnGetShaderStorageBuffer;
#pragma endregion


#pragma region Game
		FUNCTABLE(GameWorld).OnGetActiveScene = OnGetActiveScene;
#pragma endregion
	} while (false);

	//Verify FuncTables
	do
	{
		if (!FUNCTABLE(AssetHub).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [AssetHub]!"); }
		if (!FUNCTABLE(Configurator).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [Configurator]!"); }
		if (!FUNCTABLE(EventDispatcher).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [EventDispatcher]!"); }
		if (!FUNCTABLE(RHIDevice).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [RHIDevice]!"); }
		if (!FUNCTABLE(InputMonitor).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [InputMonitor]!"); }
		if (!FUNCTABLE(Logger).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [Logger]!"); }
		if (!FUNCTABLE(TaskScheduler).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [TaskScheduler]!"); }
		if (!FUNCTABLE(ThreadTracker).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [ThreadTracker]!"); }
		if (!FUNCTABLE(Chronometer).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [Chronometer]!"); }
		if (!FUNCTABLE(Window).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [Window]!"); }
		if (!FUNCTABLE(GameWorld).Verify()) { throw std::runtime_error("Engine [FuncTable] Miss [GameWorld]!"); }
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
		if (!MODULE(Renderer).Create()) { throw std::runtime_error("Engine [Module] [Renderer] Create Fail!"); }
		if (!MODULE(Window).Create()) { throw std::runtime_error("Engine [Module] [Window] Create Fail!"); }
		if (!MODULE(PhysicsSimulator).Create()) { throw std::runtime_error("Engine [Module] [PhysicsSimulator] Create Fail!"); }
		if (!MODULE(GameWorld).Create()) { throw std::runtime_error("Engine [Module] [GameWorld] Create Fail!"); }
		if (!MODULE(ScriptRuntime).Create()) { throw std::runtime_error("Engine [Module] [ScriptRuntime] Create Fail!"); }
	} while (false);
	
	//Verify Modules
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
		if (!MODULE(Window).Initialize(MODULE(Configurator)->GetWindowWidth(), MODULE(Configurator)->GetWindowHeight(), MODULE(Configurator)->GetWindowName().data())) { throw std::runtime_error("Engine [Window] Module Initialize Fail!"); }
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
	INVOKE_TERMINATEFIXEDUPDATE_HOOK

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
		//提交Light
		for (auto [entity, light, transform] : scene->GetView<Pitaya::Game::Light, Pitaya::Game::Transform>(entt::exclude<Pitaya::Game::Disabled>).each())
		{
			MODULE(RenderPipeline)->AddSceneLight(Pitaya::Core::PassKey<Pitaya::Engine::Engine>(), 
				{ glm::vec4(transform.GetWorldPosition(), static_cast<float>(light.GetType())) ,
				  glm::vec4(transform.GetWorldForward(), 0.0f) ,
				  glm::vec4(light.GetColor(), light.GetIntensity()),
				  glm::vec4(light.GetRadius(), glm::cos(glm::radians(light.GetInnerAngle())), glm::cos(glm::radians(light.GetOuterAngle())), 0.0f) });
		}
		
		//提交MeshRenderer
		for (auto [entity, meshrenderer, transform] : scene->GetGroup<Pitaya::Game::MeshRenderer>(entt::get<Pitaya::Game::Transform>, entt::exclude<Pitaya::Game::Disabled>).each())
		{
			//尝试获取材质覆盖组件
			auto* materialOverride = scene->GetComponent<Pitaya::Game::MaterialOverride>(entity);
			const auto& mesh = meshrenderer.GetMesh();
			const auto& materials = materialOverride ? materialOverride->GetOverrideMaterials() : meshrenderer.GetMaterials();	//如果有材质覆盖组件 优先使用覆盖的材质列表 否则使用MeshRenderer自带的材质列表
			if (mesh.IsReady() && mesh.GetNativeAssetData() && !mesh.GetNativeAssetData()->SubMeshs.empty())
			{
				for (uint32_t i = 0; i < mesh.GetNativeAssetData()->SubMeshs.size(); ++i)
				{
					uint32_t matIndex = mesh.GetNativeAssetData()->SubMeshs[i].MaterialIndex;
					Pitaya::Asset::Material* nativeMaterial =
						(matIndex < materials.size() && materials[matIndex].IsReady()) ? materials[matIndex].GetNativeAssetData() : nullptr;

					MODULE(RenderPipeline)->AddRenderItem(
						Pitaya::Core::PassKey<Pitaya::Engine::Engine>(),
						mesh.GetNativeAssetData(), nativeMaterial, meshrenderer.GetLayerMask(), transform.GetWorldMatrix(), 
						i, meshrenderer.GetEnableShadowCast(), meshrenderer.GetReceiveShadow());
				}
			}
			else
			{
				//mesh 还没加载 → 传 nullptr, Submit 会 fallback 到异常立方体
				MODULE(RenderPipeline)->AddRenderItem(
					Pitaya::Core::PassKey<Pitaya::Engine::Engine>(), nullptr, nullptr, 
					meshrenderer.GetLayerMask(), transform.GetWorldMatrix(), 0, true, false);
			}
		}
		
		if (INVOKE_SHOULDSUBMITSCENECAMERAPASS_HOOK)
		{
			//提交Pass
			for (auto [entity, camera, transform] : scene->GetView<Pitaya::Game::Camera, Pitaya::Game::Transform>(entt::exclude<Pitaya::Game::Disabled>).each())
			{
				if (camera.GetIsRenderToMainDisplayRT())
				{
					MODULE(RenderPipeline)->AddRenderPass(Pitaya::Core::PassKey<Pitaya::Engine::Engine>(),
						camera.GetCameraState().BuildSnapshot(transform.GetWorldPosition(), transform.GetWorldForward(), transform.GetWorldUp()),
						camera.GetPostProcessSetting(), camera.GetCullingMask(), nullptr, //提交nullptr渲染到MainDisplayRT
						camera.GetCameraState().NearClip, camera.GetCameraState().FarClip);
				}
				else if(camera.GetRenderTargetIsReady())
				{
					MODULE(RenderPipeline)->AddRenderPass(Pitaya::Core::PassKey<Pitaya::Engine::Engine>(),
						camera.GetCameraState().BuildSnapshot(transform.GetWorldPosition(), transform.GetWorldForward(), transform.GetWorldUp()),
						camera.GetPostProcessSetting(), camera.GetCullingMask(), camera.GetNativeRenderTarget(), //提交资产RT
						camera.GetCameraState().NearClip, camera.GetCameraState().FarClip);
				}
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

	//Nullify FuncTables
	do
	{
		FUNCTABLE(AssetHub).Nullify();
		FUNCTABLE(Configurator).Nullify();
		FUNCTABLE(EventDispatcher).Nullify();;
		FUNCTABLE(RHIDevice).Nullify();
		FUNCTABLE(InputMonitor).Nullify();
		FUNCTABLE(Logger).Nullify();
		FUNCTABLE(TaskScheduler).Nullify();
		FUNCTABLE(ThreadTracker).Nullify();
		FUNCTABLE(Chronometer).Nullify();
		FUNCTABLE(Window).Nullify();
		FUNCTABLE(GameWorld).Nullify();
	} while (false);

	//Nullify Context
	Pitaya::Engine::Context::Instance().Nullify();
}

#undef MODULE
#undef FUNCTABLE

template<>
Pitaya::Engine::Engine& ENGINE_CALL Pitaya::Core::Singleton<Pitaya::Engine::Engine>::Instance()
{
	static Pitaya::Engine::Engine instance;
	return instance;
}
