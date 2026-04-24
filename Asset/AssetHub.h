#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Container/ThreadSafe/ThreadSafeQueue.h>
#include<Core/Container/ThreadSafe/ThreadSafeHashMap.h>
#include<Core/Container/ThreadSafe/ThreadSafeBidirectionalMap.h>
#include<Core/Asset/Asset.h>
#include<Core/Utils/Time.h>

#include<Serialize/Serializable.h>
#include<Asset/Common/FuncTable.h>
#include<Log/Common/FuncTable.h>
#include<Task/Common/FuncTable.h>
#include<GPU/Common/ShaderType.h>
#include<Import/TextureImporter/Frontend/TextureImporter.h>
#include<Import/ShaderImporter/ShaderImporter.h>
#include<Asset/Common/AssetOperate.h>
#include<Asset/Common/Material.h>
#include<Asset/Common/Mesh.h>
#include<Asset/Common/RenderTarget.h>
#include<Asset/Common/Shader.h>
#include<Asset/Common/Texture.h>
#include<Asset/Common/AssetType.h>

#include<chrono>

namespace Pitaya::Asset
{
	class AssetHub
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<AssetHub>;
		private:
			static AssetHub* Create()
			{
				return PITAYA_NEW(AssetHub);
			}
			static void Destroy(AssetHub* assetHub)
			{
				PITAYA_DELETE(assetHub);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<AssetHub>;
		private:
			static bool Initialize(AssetHub* assetHub)
			{
				return assetHub->Initialize();
			}
			static void Release(AssetHub* assetHub)
			{
				assetHub->Release();
			}
		};

	private:
		struct AssetRegistry : public Pitaya::Serialize::Serializable
		{
			void Serialize(Pitaya::Serialize::SerializeContext&) const override;
			void Deserialize(const Pitaya::Serialize::DeserializeContext&) override;

			Pitaya::Core::ThreadSafeBidirectionalMap<Core::GUID, std::filesystem::path> Map =
			{
				{ Pitaya::Asset::Shader::Default, std::filesystem::path("engine:/shader/default/default.shader")},
				{ Pitaya::Asset::Texture::White, std::filesystem::path("engine:/texture/2D/white.png") },
				{ Pitaya::Asset::Material::Default, std::filesystem::path("engine:/material/default.mat") },
				{ Pitaya::Asset::Mesh::Cube, std::filesystem::path("engine:/mesh/cube/cube.obj") },
				{ Pitaya::Asset::Mesh::Panel, std::filesystem::path("engine:/mesh/panel/panel.obj") },
				{ Pitaya::Asset::Mesh::Sphere, std::filesystem::path("engine:/mesh/Sphere.obj") },

#pragma region TOREMOVE
				{ Pitaya::Asset::Texture::Box,std::filesystem::path("engine:/texture/2D/box.jpg") },
				{ Pitaya::Asset::Texture::Face,std::filesystem::path("engine:/texture/2D/face.png") },
				{ Pitaya::Asset::Texture::Gress,std::filesystem::path("engine:/texture/2D/gress.png") },
				{ Pitaya::Asset::Texture::Huolg,std::filesystem::path("engine:/texture/2D/Huolg.png") },
				{ Pitaya::Asset::Texture::Irony,std::filesystem::path("engine:/texture/2D/irony.png") },
				{ Pitaya::Asset::Texture::TrasnformWindow,std::filesystem::path("engine:/texture/2D/trasnform_window.png") },
				{ Pitaya::Asset::Texture::Wood,std::filesystem::path("engine:/texture/2D/wood.png") },
				{ Pitaya::Asset::Texture::Wall,std::filesystem::path("engine:/texture/2D/wall.jpg") },
				{ Pitaya::Asset::Texture::Glacier,std::filesystem::path("engine:/texture/Cubemap/glacier/glacier.cubemap") },
				{ Pitaya::Asset::Texture::Palace,std::filesystem::path("engine:/texture/Cubemap/palace/palace.cubemap") },
				{ Pitaya::Asset::Texture::Backpack_Ao,std::filesystem::path("engine:/mesh/backpack/ao.jpg") },
				{ Pitaya::Asset::Texture::Backpack_Diffuse,std::filesystem::path("engine:/mesh/backpack/diffuse.jpg") },
				{ Pitaya::Asset::Texture::Backpack_Normal,std::filesystem::path("engine:/mesh/backpack/normal.png") },
				{ Pitaya::Asset::Texture::Backpack_Roughness,std::filesystem::path("engine:/mesh/backpack/roughness.jpg") },
				{ Pitaya::Asset::Texture::Backpack_Specular,std::filesystem::path("engine:/mesh/backpack/specular.jpg") },
				{ Pitaya::Asset::Texture::Planet_Quom,std::filesystem::path("engine:/mesh/planet/planet_Quom1200.png") },
				{ Pitaya::Asset::Texture::Rock_Surface,std::filesystem::path("engine:/mesh/rock/Rock-Texture-Surface.jpg") },
				{ Pitaya::Asset::Material::Backpack,std::filesystem::path("engine:/mesh/backpack/Scene_-_Root.mat") },
				{ Pitaya::Asset::Material::Planet,std::filesystem::path("engine:/mesh/planet/Material.001.mat") },
				{ Pitaya::Asset::Material::Rock,std::filesystem::path("engine:/mesh/rock/Material.mat") },
				{Pitaya::Asset::Mesh::Backpack,std::filesystem::path("engine:/mesh/backpack/backpack.obj") },
				{Pitaya::Asset::Mesh::Planet,std::filesystem::path("engine:/mesh/planet/planet.obj")},
				{Pitaya::Asset::Mesh::Rock,std::filesystem::path("engine:/mesh/rock/rock.obj") }
#pragma endregion
			};
		};

	private:
		AssetHub() = default;
		~AssetHub() = default;

	public:
		AssetHub(const AssetHub&) = delete;
		AssetHub& operator=(const AssetHub&) = delete;
		AssetHub(AssetHub&&) = delete;
		AssetHub& operator=(AssetHub&&) = delete;

	private:
		bool Initialize();
		void Release();

	public:
		template<typename T>
		inline Pitaya::Core::Asset<T> LoadAsset(Pitaya::Core::GUID guid)
		{
			static_assert(std::is_same_v<T, Pitaya::Asset::Texture> ||
				std::is_same_v<T, Pitaya::Asset::Shader> ||
				std::is_same_v<T, Pitaya::Asset::Mesh> ||
				std::is_same_v<T, Pitaya::Asset::RenderTarget> ||
				std::is_same_v<T, Pitaya::Asset::Material>,
				"Unknow Asset!");

			std::filesystem::path path;
			if (!GetAssetPathByGUID(guid, path))
			{
				Pitaya::Log::Error(guid.ToString() + " asset cant find path");
				return nullptr;
			}
			return LoadAsset<T>(guid, path);
		}
		template<typename T>
		inline Pitaya::Core::Asset<T> LoadAsset(const std::filesystem::path& path)
		{
			static_assert(std::is_same_v<T, Pitaya::Asset::Texture> ||
				std::is_same_v<T, Pitaya::Asset::Shader> ||
				std::is_same_v<T, Pitaya::Asset::Mesh> ||
				std::is_same_v<T, Pitaya::Asset::RenderTarget> ||
				std::is_same_v<T, Pitaya::Asset::Material>,
				"Unknow Asset!");

			Pitaya::Core::GUID guid;
			if (!GetAssetGUIDByPath(path, guid))
			{
				Pitaya::Log::Error(path.string() + " asset cant find GUID");
				return nullptr;
			}
			return LoadAsset<T>(guid, path);
		}
		template<typename T>
		inline Pitaya::Core::Asset<T> LoadAsset(Pitaya::Core::GUID guid, const std::filesystem::path& path)
		{
			static_assert(std::is_same_v<T, Pitaya::Asset::Texture> ||
				std::is_same_v<T, Pitaya::Asset::Shader> ||
				std::is_same_v<T, Pitaya::Asset::Mesh> ||
				std::is_same_v<T, Pitaya::Asset::RenderTarget> ||
				std::is_same_v<T, Pitaya::Asset::Material>,
				"Unknow Asset!");

			if (!CheckIsVirtualPath(path))
			{
				Pitaya::Log::Error("load asset fail! path is not virtual path, path:" + path.string());
				return nullptr;
			}

			std::filesystem::path resolvePath = GetResolvePath(path);
			if (!CheckAssetValid(resolvePath))
			{
				Pitaya::Log::Error("load asset fail! path invalid, path:" + resolvePath.string());
				return nullptr;
			}

			std::string log;
			Pitaya::Log::LogLevel level = Pitaya::Log::LogLevel::Info;

			bool asyncLoad = false;
			typename Pitaya::Core::Asset<T> asset = nullptr;
			auto& map = GetAssetEntryMap<T>();
			map.FindOperateKVOrInsert(guid,
				[&asset, &asyncLoad, &log, &level](Pitaya::Core::GUID _guid, typename Pitaya::Core::Asset<T>::AssetEntry* _entry)
				{
					if (!_entry)
					{
						log = "asset entry is empty! GUID:" + _guid.ToString();
						level = Pitaya::Log::LogLevel::Error;
						return;
					}

					log = "asset entry existence, return exist asset entry GUID:" + _guid.ToString();
					level = Pitaya::Log::LogLevel::Info;
					asset = _entry;

					if (_entry->State == Pitaya::Core::AssetState::Unload)
					{
						//TODO 处理特殊情况 资产标记为unload后重新调用此函数加载 需要判断是否为Unload，如果是根据CPU、GPU状态进行不同处理
					}
				},
				[&asset, &asyncLoad, &log, &level](Pitaya::Core::GUID _guid) -> std::optional<std::pair<Pitaya::Core::GUID, typename Pitaya::Core::Asset<T>::AssetEntry*>>
				{
					level = Pitaya::Log::LogLevel::Info;
					log = "asset entry inexistence, create asset entry GUID:" + _guid.ToString();

					asyncLoad = true;
					typename Pitaya::Core::Asset<T>::AssetEntry* entry = new typename Pitaya::Core::Asset<T>::AssetEntry();
					entry->GUID = _guid;
					entry->Data.store(new T(), std::memory_order_release);
					asset = entry;
					return std::make_pair(_guid, entry);
				});
			Pitaya::Log::Write(level, log);
			if (asyncLoad) { AsyncLoadAsset<T>(guid, resolvePath); }
			return asset;
		}

		template<typename T>
		inline bool UnloadAsset(Pitaya::Core::GUID guid)
		{
			static_assert(std::is_same_v<T, Pitaya::Asset::Texture> ||
				std::is_same_v<T, Pitaya::Asset::Shader> ||
				std::is_same_v<T, Pitaya::Asset::Mesh> ||
				std::is_same_v<T, Pitaya::Asset::RenderTarget> ||
				std::is_same_v<T, Pitaya::Asset::Material>,
				"Unknow Asset!");

			std::string log;
			Pitaya::Log::LogLevel level = Pitaya::Log::LogLevel::Info;

			bool success = false;
			auto& map = GetAssetEntryMap<T>();
			map.FindOperateKV(guid,
				[&success, &log, &level](Pitaya::Core::GUID _guid, typename Pitaya::Core::Asset<T>::AssetEntry* _entry)
				{
					if (!_entry)
					{
						level = Pitaya::Log::LogLevel::Error;
						log = "asset entry is empty GUID:" + _guid.ToString();
						success = false;
						return;
					}

					_entry->State.SetBits(Pitaya::Core::AssetState::Unload);
					level = Pitaya::Log::LogLevel::Info;
					log = "asset entry set unloads";
					success = true;
				},
				[&success, &log, &level](Pitaya::Core::GUID _guid)
				{
					level = Pitaya::Log::LogLevel::Error;
					log = "cant find asset entry GUID:" + _guid.ToString();
					success = false;
				});
			Pitaya::Log::Write(level, log);
			return success;
		}

	private:
		template<typename T>
		inline void AsyncLoadAsset(Pitaya::Core::GUID guid, const std::filesystem::path& path)
		{
			static_assert(std::is_same_v<T, Pitaya::Asset::Texture> ||
				std::is_same_v<T, Pitaya::Asset::Shader> ||
				std::is_same_v<T, Pitaya::Asset::Mesh> ||
				std::is_same_v<T, Pitaya::Asset::RenderTarget> ||
				std::is_same_v<T, Pitaya::Asset::Material>,
				"Unknow Asset!");

			Pitaya::Task::PostJob(
				[this, guid, path]()
				{
					if constexpr (std::is_same_v<T, Pitaya::Asset::Texture>) { LoadTextureAsset(guid, path); }
					else if constexpr (std::is_same_v<T, Pitaya::Asset::Shader>) { LoadShaderAsset(guid, path); }
					else if constexpr (std::is_same_v<T, Pitaya::Asset::Mesh>) { LoadMeshAsset(guid, path); }
					else if constexpr (std::is_same_v<T, Pitaya::Asset::Material>) { LoadMaterialAsset(guid, path); }
					else if constexpr (std::is_same_v<T, Pitaya::Asset::RenderTarget>) { LoadRenderTarget(guid, path); }
					else { Pitaya::Log::Error("async load asset fail,GUID:" + guid.ToString() + " path:" + path.string()); }
				}, "Async Load Asset path: " + path.string() + " GUID: " + guid.ToString());
		}

	public:
		bool TransformToVirtualPath(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath) const;
		bool RegisterExternalFile(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath, Pitaya::Core::GUID& out_guid);

	private:
		template<typename T>
		Pitaya::Core::ThreadSafeHashMap<Pitaya::Core::GUID,
			typename Pitaya::Core::Asset<T>::AssetEntry*>& GetAssetEntryMap()
		{
			static_assert(std::is_same_v<T, Pitaya::Asset::Texture> ||
				std::is_same_v<T, Pitaya::Asset::Shader> ||
				std::is_same_v<T, Pitaya::Asset::Mesh> ||
				std::is_same_v<T, Pitaya::Asset::RenderTarget> ||
				std::is_same_v<T, Pitaya::Asset::Material>,
				"Unknow Asset!");

			if constexpr (std::is_same_v<T, Pitaya::Asset::Texture>) { return textures; }
			if constexpr (std::is_same_v<T, Pitaya::Asset::Shader>) { return shaders; }
			if constexpr (std::is_same_v<T, Pitaya::Asset::Mesh>) { return meshes; }
			if constexpr (std::is_same_v<T, Pitaya::Asset::Material>) { return materials; }
			if constexpr (std::is_same_v<T, Pitaya::Asset::RenderTarget>) { return rendertargets; }
		}

	public:
		inline bool GetAssetPathByGUID(Pitaya::Core::GUID guid, std::filesystem::path& out_path)
		{
			std::string log;
			bool success = false;
			registry.Map.FindOperateKV(guid,
				[&success, &out_path](Pitaya::Core::GUID _guid, const std::filesystem::path& _path)
				{
					out_path = _path;
					success = true;
				},
				[&log](Pitaya::Core::GUID _guid)
				{
					log = "find path fail, GUID:" + _guid.ToString();
				});
			if (!log.empty()) { Pitaya::Log::Error(log); }
			return success;
		}
		inline bool GetAssetGUIDByPath(const std::filesystem::path& path, Pitaya::Core::GUID& out_guid)
		{
			if (!CheckIsVirtualPath(path))
			{
				Pitaya::Log::Debug("get guid by path fail, path not virtual, path:" + path.string());
				return false;
			}

			std::string log;
			bool success = false;
			registry.Map.FindOperateKV(path,
				[&success, &out_guid](const std::filesystem::path& _path, Pitaya::Core::GUID _guid)
				{
					out_guid = _guid;
					success = true;
				},
				[&log](const std::filesystem::path& _path)
				{
					log = "find guid fail, path:" + _path.string();
				});
			if (!log.empty()) { Pitaya::Log::Error(log); }
			return success;
		}

	private:
		inline std::filesystem::path GetResolvePath(const std::filesystem::path& virtualPath) const
		{
			std::string virtualPathStr = virtualPath.string();
			if (virtualPathStr.starts_with("engine:/"))
			{
				return engineRoot / virtualPathStr.substr(8);
			}

			if (virtualPathStr.starts_with("project:/"))
			{
				return projectRoot / virtualPathStr.substr(9);
			}

			return projectRoot / virtualPath;
		}

	private:
		inline bool IsBuildInAsset(const std::filesystem::path& virtualPath)
		{
			return virtualPath.string().starts_with("engine:/");
		}

	private:
		Pitaya::Asset::AssetType GetAssetType(const std::filesystem::path& path) const;

	private:
		//TODO 实现函数 每一帧遍历16-32个资源，判断能否销毁
		//TODO 实现函数 强制遍历所有资源，判断能否销毁

	private:
		void LoadTextureAsset(Core::GUID guid, const std::filesystem::path& path);
		bool LoadTexture2DAsset(Core::GUID guid, const std::filesystem::path& path, Pitaya::Import::Texture2DImportResult& result);
		bool LoadTextureCubemapAsset(Core::GUID guid, const std::filesystem::path& path, const std::vector<std::filesystem::path>& paths, Pitaya::Import::TextureCubemapImportResult& result);

	private:
		void LoadShaderAsset(Core::GUID guid, const std::filesystem::path& folder);
		bool LoadVFShaderAsset(Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath,
			const std::filesystem::path& fragmentPath, Pitaya::Import::ShaderImportResult& cpuOpResult_Inner);
		bool LoadVFGShaderAsset(Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath,
			const std::filesystem::path& fragmentPath, const std::filesystem::path& geometry, Pitaya::Import::ShaderImportResult& cpuOpResult_Inner);

	private:
		void LoadMeshAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file);
		bool LoadStaticMeshAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& detectResult, Pitaya::Import::StaticMeshImportResult& out);
		bool LoadSkinnedMeshAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& detectResult, Pitaya::Import::SkinnedMeshImportResult& out);

	private:
		void LoadMaterialAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file);

	private:
		void LoadRenderTarget(Pitaya::Core::GUID guid, const std::filesystem::path& file);

	private:
		bool CheckIsVirtualPath(const std::filesystem::path& path) const;
		bool CheckAssetValid(const std::filesystem::path& path) const;

	private:
		bool CheckIsValidTexture2DFile(const std::filesystem::path& file) const;
		bool CheckIsValidTextureCubeMapFolder(const std::filesystem::path& folder, std::vector<std::filesystem::path>& out) const;
		bool CheckIsValidShaderFloder(const std::filesystem::path& folder, std::unordered_map<Pitaya::GPU::ShaderType, std::filesystem::path>& out) const;
		bool CheckIsValidMaterialFile(const std::filesystem::path&) const;
		bool CheckIsValidRenderTargetFile(const std::filesystem::path&) const;
		bool CheckIsValidMeshFile(const std::filesystem::path& file) const;

	public:
		inline void CommitAssetOperate(const Pitaya::Asset::AssetOperate& operate)
		{
			assetOperateQueue.Push(operate);
		}

	public:
		inline void SyncAssetToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
		{
			// 每一帧最多处理5个资源操作 避免过多资源操作导致的卡顿
			if (cacheAssetOperateQueue.empty()) { cacheAssetOperateQueue = assetOperateQueue.PopN(5); }

			// 在时间预算内处理资产
			Pitaya::Core::InvokeWithTimeBudget(
				[this]() ->bool { return cacheAssetOperateQueue.empty(); },
				[this, passkey]() { std::visit([this, passkey](auto& result_Inner) { this->SyncAssetOperate(passkey,result_Inner); }, cacheAssetOperateQueue.front().Data); cacheAssetOperateQueue.pop(); },
				std::chrono::milliseconds(2));
		}
		inline bool IsUploadedToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer>)
		{
			return !cacheAssetOperateQueue.empty() || !assetOperateQueue.Empty();
		}

	private:
		void SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, std::monostate&);
		void SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Import::Texture2DImportResult&);
		void SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Import::TextureCubemapImportResult&);
		void SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Import::ShaderImportResult&);
		void SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Import::StaticMeshImportResult&);
		void SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Import::SkinnedMeshImportResult&);
		void SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Import::RenderTargetImportResult&);
		void SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Asset::Texture2DUnloadRequire&);

	private:
		const std::unordered_set<std::string> TextureExtensions =
		{ ".png", ".jpg", ".jpeg", ".bmp", ".tga",	".gif",				//普通位图
		  ".hdr", ".exr" };												//高动态范围图（仍属于2D纹理）
		const std::unordered_map<std::string, Pitaya::GPU::ShaderType> ShaderExtensions =
		{ {".vert", Pitaya::GPU::ShaderType::Vertex},{".frag", Pitaya::GPU::ShaderType::Fragment},
		  {".geom", Pitaya::GPU::ShaderType::Geometry}, };
		const std::unordered_set<std::string> MeshExtensions =
		{ ".obj" };
		const std::unordered_set<std::string> MaterialExtensions =
		{ ".mat" };
		const std::unordered_set<std::string> RenderTargetExtensions =
		{ ".rt" };

	private:
		inline static constexpr const char* fileName = "asset.cfg";

	private:
		std::filesystem::path engineRoot;
		std::filesystem::path projectRoot;
		
		Pitaya::Core::ThreadSafeHashMap<Pitaya::Core::GUID,
			Pitaya::Core::Asset<Pitaya::Asset::Mesh>::AssetEntry*> meshes;
		Pitaya::Core::ThreadSafeHashMap<Pitaya::Core::GUID,
			Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry*> textures;
		Pitaya::Core::ThreadSafeHashMap<Pitaya::Core::GUID,
			Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry*> shaders;
		Pitaya::Core::ThreadSafeHashMap<Pitaya::Core::GUID,
			Pitaya::Core::Asset<Pitaya::Asset::Material>::AssetEntry*> materials;
		Pitaya::Core::ThreadSafeHashMap<Pitaya::Core::GUID,
			Pitaya::Core::Asset<Pitaya::Asset::RenderTarget>::AssetEntry*> rendertargets;

		//存储CPU操作资源结果（任务线程添加 → 渲染线程处理）
		Pitaya::Core::ThreadSafeQueue<Pitaya::Asset::AssetOperate> assetOperateQueue;
		std::queue<Pitaya::Asset::AssetOperate> cacheAssetOperateQueue;	//渲染线程处理资源操作时的临时队列 避免长时间占用锁

		Pitaya::Asset::AssetHub::AssetRegistry registry;
	};
}


/*
bool CanUnloadAsset(AssetEntry* entry) {
	auto state = entry->State.load(std::memory_order_acquire);
		return (entry->RefCount == 0)          // 无任何引用
		&& (state & AssetState::Unload)    // 标记了Unload
		&& !(state & AssetState::CPULoading) // 无CPU加载中
		&& !(state & AssetState::GPULoading) // 无GPU加载中
		&& !(state & AssetState::CPULoading) // 重复？不，是确认无加载中
		// 补充：如果已生成GPU数据，需先提交删除指令，再删CPU数据
		&& ( (state & AssetState::GPULoaded) ? SubmitGPUUnloadCmd(entry) : true );
}
*/
