#include<Engine/Engine.h>
#include<Hook/def.h>
#include<Task/TaskScheduler.h>
#include<Log/Logger.h>
#include<Thread/ThreadTracker.h>
#include<GPU/RHIDevice.h>
#include<Time/Chronometer.h>
#include<Input/InputMonitor.h>
#include<Asset/AssetHub.h>
#include<Event/EventDispatcher.h>
#include<Config/Configurator.h>
#include<Window/Frontend/Window.h>
#include<Physics/Frontend/PhysicsSimulator.h>
#include<Render/Frontend/Renderer.h>
#include<Render/RenderPipeline.h>
#include<Project/Workspace.h>
#ifdef PITAYA_EDITOR
#include<Editor/Editor.h>
#endif

#include<Core/Console/Console.h>

#pragma region TOREMOVE
//TODO REMOVE TEST -------------------------------------
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/quaternion.hpp>
#include<gtx/quaternion.hpp>
#include<Game/Component/MeshRenderer.h>
#include<Asset/Common/RenderTarget.h>
#include<random>
#include<vector>
Pitaya::Game::MeshRenderer testmeshrenderer;
uint32_t testCount = 10;
const float MAX_TRANSLATE = 30.0f;    // 最大平移量（单位：米/单位长度）
const float MAX_ROTATE = glm::pi<float>() / 4.0f; // 最大旋转角（45度）
const float MIN_SCALE = 0.8f;        // 最小缩放系数
const float MAX_SCALE = 1.2f;        // 最大缩放系数
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> trans_dist(-MAX_TRANSLATE, MAX_TRANSLATE);
static std::uniform_real_distribution<float> rot_dist(-MAX_ROTATE, MAX_ROTATE);
static std::uniform_real_distribution<float> scale_dist(MIN_SCALE, MAX_SCALE);
std::vector<glm::mat4> models;
inline glm::mat4 GenerateModuleChangeMatrix()
{
	glm::mat4 changeMatrix = glm::mat4(1.0f);

	// 2. 随机缩放（模块大小变化）
	float scale_x = scale_dist(gen);
	float scale_y = scale_dist(gen);
	float scale_z = scale_dist(gen);
	changeMatrix = glm::scale(changeMatrix, glm::vec3(scale_x, scale_y, scale_z));

	// 3. 随机旋转（模块姿态变化，绕X/Y/Z轴分别旋转）
	float rot_x = rot_dist(gen); // 绕X轴旋转角度
	float rot_y = rot_dist(gen); // 绕Y轴旋转角度
	float rot_z = rot_dist(gen); // 绕Z轴旋转角度
	changeMatrix = glm::rotate(changeMatrix, rot_x, glm::vec3(1.0f, 0.0f, 0.0f));
	changeMatrix = glm::rotate(changeMatrix, rot_y, glm::vec3(0.0f, 1.0f, 0.0f));
	changeMatrix = glm::rotate(changeMatrix, rot_z, glm::vec3(0.0f, 0.0f, 1.0f));

	// 4. 随机平移（模块位置变化）
	float trans_x = trans_dist(gen);
	float trans_y = trans_dist(gen);
	float trans_z = trans_dist(gen);
	changeMatrix = glm::translate(changeMatrix, glm::vec3(trans_x, trans_y, trans_z));

	return changeMatrix;
}
//TODO REMOVE TEST -------------------------------------
#pragma endregion


namespace
{
#pragma region Time
	inline float Ondelta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->Getdelta();
	}
	inline float OnFixdelta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->GetFixdelta();
	}
	inline float OnUnscaledDelta() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->GetUnscaledDelta();
	}
	inline float OnTimeScale() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->GetTimeScale();
	}
	inline float OnFramerate() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->GetFramerate();
	}
	inline double OnSeconds() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->Seconds();
	}
	inline int64_t OnMilliseconds() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Time::Chronometer>()->Milliseconds();
	}
#pragma endregion


#pragma region Log
	inline void OnLog(Pitaya::Log::LogLevel level,std::string_view info) noexcept
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::Log::Logger>()->Write(level, info);
	}
#pragma endregion


#pragma region Thread
	inline Pitaya::Core::Thread::Identifier OnRegisterThread(std::string_view name, void(*Thread)(void*, void*), void* bootstraper, void* args)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Thread::ThreadTracker>()->RegisterThread(name, Thread, bootstraper, args);
	}
	inline bool OnUnregisterThread(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Thread::ThreadTracker>()->UnregisterThread(id);
	}
	inline std::string OnGetThreadName(Pitaya::Core::Thread::Identifier id) noexcept 
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Thread::ThreadTracker>()->GetThreadName(id);
	}
	inline bool OnGetThreadIsRunning(Pitaya::Core::Thread::Identifier id) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Thread::ThreadTracker>()->GetThreadIsRunning(id);
	}
#pragma endregion


#pragma region Event
	inline Pitaya::Event::EventToken OnSubscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, const ::Pitaya::Event::Event&), void* listener) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Event::EventDispatcher>()->Subscribe(type, OnCallBack, listener);
	}
	inline bool OnUnSubscribe(const Pitaya::Event::EventToken& eventToken) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Event::EventDispatcher>()->UnSubscribe(eventToken);
	}
	inline void OnEmit(const Pitaya::Event::Event& event) noexcept
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::Event::EventDispatcher>()->Emit(event);
	}
#pragma endregion


#pragma region Input
	inline bool OnGetKeyDown(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Input::InputMonitor>()->GetKeyDown(keyCode);
	}
	inline bool OnGetKeyPressed(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Input::InputMonitor>()->GetKeyPressed(keyCode);
	}
	inline bool OnGetKeyReleased(Pitaya::Input::KeyCode keyCode) noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Input::InputMonitor>()->GetKeyReleased(keyCode);
	}
#pragma endregion


#pragma region Task
	inline void OnPostJob(std::function<void()> func, std::string_view name) noexcept
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::Task::TaskScheduler>()->PostJob(std::move(func), name);
	}
#pragma endregion


#pragma region Asset
	inline Pitaya::Core::Asset<Pitaya::Asset::Texture> OnLoadTexture(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::Texture>(guid);
	}
	inline Pitaya::Core::Asset<Pitaya::Asset::Shader> OnLoadShader(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::Shader>(guid);
	}
	inline Pitaya::Core::Asset<Pitaya::Asset::Mesh> OnLoadMesh(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::Mesh>(guid);
	}
	inline Pitaya::Core::Asset<Pitaya::Asset::Material> OnLoadMaterial(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::Material>(guid);
	}
	inline Pitaya::Core::Asset<Pitaya::Asset::RenderTarget> OnLoadRenderTarget(Pitaya::Core::GUID guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->LoadAsset<Pitaya::Asset::RenderTarget>(guid);
	}
	inline bool OnGetAssetPathByGUID(Pitaya::Core::GUID guid, std::filesystem::path& out)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->GetAssetPathByGUID(guid, out);
	}
	inline bool OnGetAssetGUIDByPath(const std::filesystem::path& path, Pitaya::Core::GUID& out)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->GetAssetGUIDByPath(path, out);
	}
	inline bool OnTransformToVirtualPath(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->TransformToVirtualPath(inputPath, basePath, out_virtualpath);
	}
	inline bool OnRegisterExternalFile(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath, Pitaya::Core::GUID& out_guid)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->RegisterExternalFile(inputPath, basePath, out_virtualpath, out_guid);
	}
	inline void OnSyncAssetToGPU()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->SyncAssetToGPU();
	}
	inline bool OnIsUploadedToGPU()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Asset::AssetHub>()->IsUploadedToGPU();
	}
#pragma endregion


#pragma region Config
	inline Pitaya::Render::API OnGetRenderAPI() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetRenderAPI();
	}
	inline size_t OnGetMaxFixupdataExecuteTimes() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetMaxFixupdataExecuteTimes();
	}
	inline uint32_t OnGetMaxInstancesCount() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetMaxInstancesCount();
	}
	inline uint32_t OnGetMaxBonesPerInstance() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetMaxBonesPerInstance();
	}
	inline bool OnGetEnableVSync() noexcept
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Config::Configurator>()->GetEnableVSync();
	}
#pragma endregion


#pragma region Window
	inline void* OnGetNativeWindow()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::Window::Window>()->GetNativeWindow();
	}
#pragma endregion


#pragma region GPU
	inline void OnDestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyAllGPUResource(passkey);
	}

	inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> OnCreateVertexArray()
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateVertexArray();
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> OnCreateVertexBuffer(float* vertices, uint32_t size)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateVertexBuffer(vertices, size);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> OnCreateIndexBuffer(uint32_t* indices, uint32_t count)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateIndexBuffer(indices, count);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> OnCreateShaderVF(const char* vertexSource, const char* fragmentSource)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateShader(vertexSource, fragmentSource);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> OnCreateShaderVFG(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateShader(vertexSource, fragmentSource, geometrySource);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> OnCreateTexture2D(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateTexture2D(data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> OnCreateTextureCubemap(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateTextureCubemap(datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> OnCreateUniformBuffer(uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateUniformBuffer(size, bindingPoint);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> OnCreateFrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateFrameBuffer(spec);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> OnCreateShaderStorageBuffer(uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->CreateShaderStorageBuffer(size, bindingPoint);
	}

	inline bool OnDestroyVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyVertexArray(id);
	}
	inline bool OnDestroyVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyVertexBuffer(id);
	}
	inline bool OnDestroyIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyIndexBuffer(id);
	}
	inline bool OnDestroyShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyShader(id);
	}
	inline bool OnDestroyTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyTexture2D(id);
	}
	inline bool OnDestroyTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyTextureCubemap(id);
	}
	inline bool OnDestroyUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyUniformBuffer(id);
	}
	inline bool OnDestroyFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyFrameBuffer(id);
	}
	inline bool OnDestroyShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->DestroyShaderStorageBuffer(id);
	}

	inline Pitaya::GPU::VertexArray* OnGetVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetVertexArray(id);
	}
	inline Pitaya::GPU::VertexBuffer* OnGetVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetVertexBuffer(id);
	}
	inline Pitaya::GPU::IndexBuffer* OnGetIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetIndexBuffer(id);
	}
	inline Pitaya::GPU::Shader* OnGetShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetShader(id);
	}
	inline Pitaya::GPU::Texture2D* OnGetTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetTexture2D(id);
	}
	inline Pitaya::GPU::TextureCubemap* OnGetTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetTextureCubemap(id);
	}
	inline Pitaya::GPU::UniformBuffer* OnGetUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetUniformBuffer(id);
	}
	inline Pitaya::GPU::FrameBuffer* OnGetFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetFrameBuffer(id);
	}
	inline Pitaya::GPU::ShaderStorageBuffer* OnGetShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetModule<Pitaya::GPU::RHIDevice>()->GetShaderStorageBuffer(id);
	}
#pragma endregion
}

int Pitaya::Engine::Engine::Execute(int argc, char** argv)
{
	if (!Initialize(argc, argv)) { return -1; }

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

bool Pitaya::Engine::Engine::Initialize(int argc, char** argv)
{
	if (!FillContext()) { throw std::runtime_error("Fill Context Fail!"); }
	if (!FillFuncTables()) { throw std::runtime_error("Fill FuncTables Fail!"); }
	if (!funcTables.Check()) { throw std::runtime_error("FuncTables Check Fail!"); }
	if (!modules.Create()) { throw std::runtime_error("Modules Create Fail!"); }
	if (!modules.Check()) { throw std::runtime_error("Modules Check Fail!"); }
	if (!modules.Initialize(argc, argv)) { throw std::runtime_error("Modules Initialize Fail!"); }
	return true;
}
bool Pitaya::Engine::Engine::IsRunning() const
{
	return modules.IsRunning();
}
void Pitaya::Engine::Engine::BeginFrame()
{
	INVOKE_PREBEGINFRAME_HOOK
	modules.BeginFrame();
}
void Pitaya::Engine::Engine::FixedUpdate()
{
	INVOKE_PREFIXEDUPDATE_HOOK
	modules.FixedUpdate();
}
void Pitaya::Engine::Engine::Update()
{
	INVOKE_PREUPDATE_HOOK
	modules.Update();
}
void Pitaya::Engine::Engine::LateUpdate()
{
	INVOKE_PRELATEUPDATE_HOOK
	modules.LateUpdate();
}
void Pitaya::Engine::Engine::Render()
{
	modules.Render();
}
void Pitaya::Engine::Engine::EndFrame()
{
	INVOKE_PREENDFRAME_HOOK
	modules.EndFrame();
}
void Pitaya::Engine::Engine::FrameSync()
{
	modules.FrameSync();
}
void Pitaya::Engine::Engine::Release()
{
	if (!modules.Configurator->Export(Pitaya::Core::PassKey<Pitaya::Engine::Engine>())) { Pitaya::Core::Print(Pitaya::Core::Color::Red, "Config Export Fail!"); Pitaya::Core::PopupMessageBox("Warning","Config Export Fail"); }
	modules.Release();
	modules.Destroy();
	funcTables.UnRegister();
	Pitaya::Engine::Context::Instance().UnRegister();
}
bool Pitaya::Engine::Engine::FillContext()
{
	auto& context = Pitaya::Engine::Context::Instance();
	auto& modules = context.modules;
	modules.AssetHub = &this->modules.AssetHub;
	modules.Renderer = &this->modules.Renderer;
	modules.RenderPipeline = &this->modules.RenderPipeline;
	modules.PhysicsSimulator = &this->modules.PhysicsSimulator;
	modules.Window = &this->modules.Window;
	modules.InputMonitor = &this->modules.InputMonitor;
	modules.Chronometer = &this->modules.Chronometer;
	modules.EventDispatcher = &this->modules.EventDispatcher;
	modules.Logger = &this->modules.Logger;
	modules.ThreadTracker = &this->modules.ThreadTracker;
	modules.TaskScheduler = &this->modules.TaskScheduler;
	modules.Workspace = &this->modules.Workspace;
	modules.RHIDevice = &this->modules.RHIDevice;
	modules.Configurator = &this->modules.Configurator;
	
	auto& funcTables = context.funcTables;
	funcTables.AssetHub = &this->funcTables.AssetHub;
	funcTables.Configurator = &this->funcTables.Configurator;
	funcTables.EventDispatcher = &this->funcTables.EventDispatcher;
	funcTables.RHIDevice = &this->funcTables.RHIDevice;
	funcTables.InputMonitor = &this->funcTables.InputMonitor;
	funcTables.Logger = &this->funcTables.Logger;
	funcTables.TaskScheduler = &this->funcTables.TaskScheduler;
	funcTables.ThreadTracker = &this->funcTables.ThreadTracker;
	funcTables.Chronometer = &this->funcTables.Chronometer;
	funcTables.Window = &this->funcTables.Window;
	return context.Check();
}
bool Pitaya::Engine::Engine::FillFuncTables()
{
#pragma region Asset
	auto& asset = funcTables.AssetHub;
	asset.OnLoadTexture = OnLoadTexture;
	asset.OnLoadShader = OnLoadShader;
	asset.OnLoadMesh = OnLoadMesh;
	asset.OnLoadMaterial = OnLoadMaterial;
	asset.OnLoadRenderTarget = OnLoadRenderTarget;
	asset.OnGetAssetPathByGUID = OnGetAssetPathByGUID;
	asset.OnGetAssetGUIDByPath = OnGetAssetGUIDByPath;
	asset.OnTransformToVirtualPath = OnTransformToVirtualPath;
	asset.OnRegisterExternalFile = OnRegisterExternalFile;
	asset.OnSyncAssetToGPU = OnSyncAssetToGPU;
	asset.OnIsUploadedToGPU = OnIsUploadedToGPU;
#pragma endregion


#pragma region Time
	auto& time = funcTables.Chronometer;
	time.Ondelta = Ondelta;
	time.OnFixdelta = OnFixdelta;
	time.OnUnscaledDelta = OnUnscaledDelta;
	time.OnTimeScale = OnTimeScale;
	time.OnFramerate = OnFramerate;
	time.OnMilliseconds = OnMilliseconds;
	time.OnSeconds = OnSeconds;
#pragma endregion


#pragma region Log
	auto& log = funcTables.Logger;
	log.OnLog = OnLog;
#pragma endregion


#pragma region Thread
	auto& thread = funcTables.ThreadTracker;
	thread.OnRegisterThread = OnRegisterThread;
	thread.OnUnregisterThread = OnUnregisterThread;
	thread.OnGetThreadName = OnGetThreadName;
	thread.OnGetThreadIsRunning = OnGetThreadIsRunning;
#pragma endregion
	

#pragma region Event
	auto& event = funcTables.EventDispatcher;
	event.OnSubscribe = OnSubscribe;
	event.OnUnSubscribe = OnUnSubscribe;;
	event.OnEmit = OnEmit;
#pragma endregion


#pragma region Input
	auto& input = funcTables.InputMonitor;
	input.OnGetKeyDown = OnGetKeyDown;
	input.OnGetKeyPressed = OnGetKeyPressed;
	input.OnGetKeyReleased = OnGetKeyReleased;
#pragma endregion


#pragma region Task
	auto& task = funcTables.TaskScheduler;
	task.OnPostJob = OnPostJob;
#pragma endregion


#pragma region Config
	auto& config = funcTables.Configurator;
	config.OnGetRenderAPI = OnGetRenderAPI;
	config.OnGetMaxFixupdataExecuteTimes = OnGetMaxFixupdataExecuteTimes;
	config.OnGetMaxInstancesCount = OnGetMaxInstancesCount;
	config.OnGetMaxBonesPerInstance = OnGetMaxBonesPerInstance;
	config.OnGetEnableVSync = OnGetEnableVSync;
#pragma endregion


#pragma region Window
	auto& window = funcTables.Window;
	window.OnGetNativeWindow = OnGetNativeWindow;
#pragma endregion


#pragma region GPU
	auto& gpu = funcTables.RHIDevice;
	gpu.OnDestroyAllGPUResource = OnDestroyAllGPUResource;

	gpu.OnCreateVertexArray = OnCreateVertexArray;
	gpu.OnCreateVertexBuffer = OnCreateVertexBuffer;
	gpu.OnCreateIndexBuffer = OnCreateIndexBuffer;
	gpu.OnCreateShaderVF = OnCreateShaderVF;
	gpu.OnCreateShaderVFG = OnCreateShaderVFG;
	gpu.OnCreateTexture2D = OnCreateTexture2D;
	gpu.OnCreateTextureCubemap = OnCreateTextureCubemap;
	gpu.OnCreateUniformBuffer = OnCreateUniformBuffer;
	gpu.OnCreateFrameBuffer = OnCreateFrameBuffer;
	gpu.OnCreateShaderStorageBuffer = OnCreateShaderStorageBuffer;

	gpu.OnDestroyVertexArray = OnDestroyVertexArray;
	gpu.OnDestroyVertexBuffer = OnDestroyVertexBuffer;
	gpu.OnDestroyIndexBuffer = OnDestroyIndexBuffer;
	gpu.OnDestroyShader = OnDestroyShader;
	gpu.OnDestroyTexture2D = OnDestroyTexture2D;
	gpu.OnDestroyTextureCubemap = OnDestroyTextureCubemap;
	gpu.OnDestroyUniformBuffer = OnDestroyUniformBuffer;
	gpu.OnDestroyFrameBuffer = OnDestroyFrameBuffer;
	gpu.OnDestroyShaderStorageBuffer = OnDestroyShaderStorageBuffer;

	gpu.OnGetVertexArray = OnGetVertexArray;
	gpu.OnGetVertexBuffer = OnGetVertexBuffer;
	gpu.OnGetIndexBuffer = OnGetIndexBuffer;
	gpu.OnGetShader = OnGetShader;
	gpu.OnGetTexture2D = OnGetTexture2D;
	gpu.OnGetTextureCubemap = OnGetTextureCubemap;
	gpu.OnGetUniformBuffer = OnGetUniformBuffer;
	gpu.OnGetFrameBuffer = OnGetFrameBuffer;
	gpu.OnGetShaderStorageBuffer = OnGetShaderStorageBuffer;
#pragma endregion
	return true;
}

bool Pitaya::Engine::Engine::Modules::Create()
{
	if (!Configurator.Create()) { throw std::runtime_error("Engine [Module] [Configurator] Create Fail!"); }
	if (!Configurator->Import(Pitaya::Core::PassKey<Pitaya::Engine::Engine>())) { Pitaya::Core::Print(Pitaya::Core::Color::Red, "Config Import Fail!"); Pitaya::Core::PopupMessageBox("Warning", "Config Import Fail!"); }
	if (!Chronometer.Create()) { throw std::runtime_error("Engine [Module] [Chronometer] reate Fail!"); }
	if (!TaskScheduler.Create()) { throw std::runtime_error("Engine [Module] [TaskScheduler] Create Fail!"); }
	if (!ThreadTracker.Create()) { throw std::runtime_error("Engine [Module] [ThreadTracker] Create Fail!"); }
	if (!EventDispatcher.Create()) { throw std::runtime_error("Engine [Module] [EventDispatcher] Create Fail!"); }
	if (!Logger.Create()) { throw std::runtime_error("Engine [Module] [Logger] Create Fail!"); }
	if (!RHIDevice.Create()) { throw std::runtime_error("Engine [Module] [RHIDevice] Create Fail!"); }
	if (!InputMonitor.Create()) { throw std::runtime_error("Engine [Module] [InputMonitor] Create Fail!"); }
	if (!AssetHub.Create()) { throw std::runtime_error("Engine [Module] [AssetHub] Create Fail!"); }
	if (!Workspace.Create()) { throw std::runtime_error("Engine [Module] [Workspace] Create Fail!"); }
	if (!RenderPipeline.Create()) { throw std::runtime_error("Engine [Module] [RenderPipeline] Create Fail!"); }
	if (!Renderer.Create(Configurator->GetRenderAPI())) { throw std::runtime_error("Engine [Module] [Renderer] Create Fail!"); }
	if (!Window.Create(Configurator->GetWindowPlatform())) { throw std::runtime_error("Engine [Module] [Window] Create Fail!"); }
	if(!PhysicsSimulator.Create(Configurator->GetPhysicsAPI())){ throw std::runtime_error("Engine [Module] [PhysicsSimulator] Create Fail!"); }
	return true;
}
bool Pitaya::Engine::Engine::Modules::Check()
{
	if (!Workspace) { throw std::runtime_error("Engine [Module] Miss [Workspace]!"); }
	if (!Configurator) { throw std::runtime_error("Engine [Module] Miss [Configurator]!"); }
	if (!EventDispatcher) { throw std::runtime_error("Engine [Module] Miss [EventDispatcher]!"); }
	if (!InputMonitor) { throw std::runtime_error("Engine [Module] Miss [InputMonitor]!"); }
	if (!ThreadTracker) { throw std::runtime_error("Engine [Module] Miss [ThreadTracker]!"); }
	if (!Logger) { throw std::runtime_error("Engine [Module] Miss [Logger]!"); }
	if (!TaskScheduler) { throw std::runtime_error("Engine [Module] Miss [TaskScheduler]!"); }
	if (!AssetHub) { throw std::runtime_error("Engine [Module] Miss [AssetHub]!"); }
	if (!PhysicsSimulator) { throw std::runtime_error("Engine [Module] Miss [PhysicsSimulator]!"); }
	if (!RHIDevice) { throw std::runtime_error("Engine [Module] Miss [RHIDevice]!"); }
	if (!Window) { throw std::runtime_error("Engine [Module] Miss [Window]!"); }
	if (!Renderer) { throw std::runtime_error("Engine [Module] Miss [Renderer]!"); }
	if (!Chronometer) { throw std::runtime_error("Engine [Module] Miss [Chronometer]!"); }
	if (!RenderPipeline) { throw std::runtime_error("Engine [Module] Miss [RenderPipeline]!"); }
	return true;
}
bool Pitaya::Engine::Engine::Modules::Initialize(int argc, char** argv)
{
	if (!Workspace.Initialize(argc, argv)) { throw std::runtime_error("Engine [Workspace] Module Initialize Fail!"); }
	if (!Configurator.Initialize()) { throw std::runtime_error("Engine [Configurator] Module Initialize Fail!"); }
	if (!EventDispatcher.Initialize()) { throw std::runtime_error("Engine [EventDispatcher] Module Initialize Fail!"); }
	if (!InputMonitor.Initialize()) { throw std::runtime_error("Engine [InputMonitor] Module Initialize Fail!"); }
	if (!ThreadTracker.Initialize()) { throw std::runtime_error("Engine [ThreadTracker] Module Initialize Fail!"); }
	if (!Logger.Initialize()) { throw std::runtime_error("Engine [Logger] Module Initialize Fail!"); }
	if (!TaskScheduler.Initialize()) { throw std::runtime_error("Engine [TaskScheduler] Module Initialize Fail!"); }
	if (!AssetHub.Initialize()) { throw std::runtime_error("Engine [AssetHub] Module Initialize Fail!"); }
	if (!PhysicsSimulator.Initialize()) { throw std::runtime_error("Engine [PhysicsSimulator] Module Initialize Fail!"); }
	if (!RHIDevice.Initialize()) { throw std::runtime_error("Engine [RHIDevice] Module Initialize Fail!"); }
	if (!Window.Initialize(Configurator->GetWindowWidth(), Configurator->GetWindowHeight(), Configurator->GetWindowName().c_str())) { throw std::runtime_error("Engine [Window] Module Initialize Fail!"); }
	if (!RenderPipeline.Initialize()) { throw std::runtime_error("[RenderPipeline] Module Initialize Fail!"); }
	if (!Renderer.Initialize(Window->GetNativeWindow())) { throw std::runtime_error("[Renderer] Module Initialize Fail!"); }
	if (!Chronometer.Initialize()) { throw std::runtime_error("Engine [Chronometer] Module Initialize Fail!"); }

	//TODO REMOVE TEST -----------------------------------------------------------------------------
	testmeshrenderer.mesh = AssetHub->LoadAsset<Pitaya::Asset::Mesh>(Pitaya::Asset::Mesh::Backpack);
	testmeshrenderer.materials.emplace_back(AssetHub->LoadAsset<Pitaya::Asset::Material>(Pitaya::Asset::Material::Default));
	testmeshrenderer.materials.emplace_back(AssetHub->LoadAsset<Pitaya::Asset::Material>(Pitaya::Asset::Material::Backpack));
	
	models.resize(testCount);
	for (uint32_t i = 0; i < testCount; i++)
	{
		models[i] = GenerateModuleChangeMatrix();
	}
	//TODO REMOVE TEST -----------------------------------------------------------------------------

	return true;
}
bool Pitaya::Engine::Engine::Modules::IsRunning() const
{
	return !Window->IsClose();
}
void Pitaya::Engine::Engine::Modules::BeginFrame()
{
	Chronometer->Tick(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
	InputMonitor->PrepareNewFrame(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
	Window->PollEvents(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
	InputMonitor->UpdateSnapshots(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
}
void Pitaya::Engine::Engine::Modules::FixedUpdate()
{
	PhysicsSimulator.FixedUpdate();
}
void Pitaya::Engine::Engine::Modules::Update()
{

}
void Pitaya::Engine::Engine::Modules::LateUpdate()
{

}
void Pitaya::Engine::Engine::Modules::Render()
{
	RenderPipeline->NewPipeline(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());

	//TODO 获取当前激活场景中所有 MeshRenderer 组件
	auto& mesh = testmeshrenderer.GetMesh();
	auto& materials = testmeshrenderer.GetMaterials();
	for (auto& model : models)
	{
		if (mesh.IsReady() && mesh.GetNativeAssetData() && !mesh.GetNativeAssetData()->SubMeshs.empty())
		{
			for (uint32_t i = 0; i < mesh.GetNativeAssetData()->SubMeshs.size(); ++i)
			{
				uint32_t matIndex = mesh.GetNativeAssetData()->SubMeshs[i].MaterialIndex;
				Pitaya::Asset::Material* nativeMaterial =
					(matIndex < materials.size() && materials[matIndex].IsReady()) ?  materials[matIndex].GetNativeAssetData() : nullptr;

				RenderPipeline->AddRenderItem(
					Pitaya::Core::PassKey<Pitaya::Engine::Engine>(),
					mesh.GetNativeAssetData(), nativeMaterial,
					testmeshrenderer.GetLayerMask(), model, i);
			}
		}
		else
		{
			//mesh 还没加载 → 传 nullptr，Submit 会 fallback 到异常立方体
			RenderPipeline->AddRenderItem(
				Pitaya::Core::PassKey<Pitaya::Engine::Engine>(),
				nullptr, nullptr,
				testmeshrenderer.GetLayerMask(), model, 0);
		}
	}

	//TODO 获取全局激活摄像机，遍历摄像机（一个摄像机一个Pass），渲染所有MeshRenderer组件

	RenderPipeline->Execute(Pitaya::Core::PassKey<Pitaya::Engine::Engine>(), Renderer.GetKernel());
}
void Pitaya::Engine::Engine::Modules::EndFrame()
{

}
void Pitaya::Engine::Engine::Modules::FrameSync()
{
	Chronometer->FrameSync(Pitaya::Core::PassKey<Pitaya::Engine::Engine>());
}
void Pitaya::Engine::Engine::Modules::Release()
{
	Workspace.Release();
	Chronometer.Release();
	InputMonitor.Release();
	PhysicsSimulator.Release();
	TaskScheduler.Release();
	Configurator.Release();
	RHIDevice.Release();
	RenderPipeline.Release();
	Renderer.Release();
	AssetHub.Release();
	Window.Release();
	EventDispatcher.Release();
	ThreadTracker.Release();
	Logger.Release();
}
void Pitaya::Engine::Engine::Modules::Destroy()
{
	Workspace.Destroy();
	Chronometer.Destroy();
	InputMonitor.Destroy();
	PhysicsSimulator.Destroy(); 
	TaskScheduler.Destroy();
	RenderPipeline.Destroy();
	Renderer.Destroy();
	RHIDevice.Destroy();
	Configurator.Destroy();
	AssetHub.Destroy();
	Window.Destroy();
	EventDispatcher.Destroy();
	ThreadTracker.Destroy();
	Logger.Destroy();
}

bool Pitaya::Engine::Engine::FuncTables::Check() const
{
	if (!AssetHub.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [AssetHub]!"); }
	if (!Configurator.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [Configurator]!"); }
	if (!EventDispatcher.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [EventDispatcher]!"); }
	if (!RHIDevice.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [RHIDevice]!"); }
	if (!InputMonitor.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [InputMonitor]!"); }
	if (!Logger.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [Logger]!"); }
	if (!TaskScheduler.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [TaskScheduler]!"); }
	if (!ThreadTracker.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [ThreadTracker]!"); }
	if (!Chronometer.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [Chronometer]!"); }
	if (!Window.Check()) { throw std::runtime_error("Engine [FuncTable] Miss [Window]!"); }
	return true;
}
void Pitaya::Engine::Engine::FuncTables::UnRegister()
{
	AssetHub.UnRegister();
	Configurator.UnRegister();
	EventDispatcher.UnRegister();;
	RHIDevice.UnRegister();
	InputMonitor.UnRegister();
	Logger.UnRegister();
	TaskScheduler.UnRegister();
	ThreadTracker.UnRegister();
	Chronometer.UnRegister();
	Window.UnRegister();
}
