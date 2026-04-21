#include<Asset/AssetHub.h>
#include<GPU/Common/FuncTable.h>
#include<GPU/Common/TextureType.h>
#include<GPU/Common/MeshType.h>
#include<GPU/Common/TextureCubemapFace.h>
#include<GPU/Frontend/Shader/Shader.h>
#include<GPU/Frontend/Buffer/VertexArray.h>
#include<GPU/Frontend/Buffer/FrameBuffer.h>
#include<Import/Import.h>
#include<Core/Utils/File.h>

bool Pitaya::Asset::AssetHub::Initialize()
{
	engineRoot = Pitaya::Core::GetExecutableDirectory() / "resource";
	projectRoot = Pitaya::Core::GetWorkspace() / "Asset" / "Resource";
	//registry.DeserializeFromFile();	//TOOD 反序列化数据
	return true;
}
void Pitaya::Asset::AssetHub::Release()
{
	meshes.ForEachCheckErase(
		[](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Mesh>::AssetEntry*& _entry)
		{
			if (_entry)
			{
				delete _entry;
				_entry = nullptr;
			}
			return true;
		});

	textures.ForEachCheckErase(
		[](Pitaya::Core::GUID _guid,Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry*& _entry)
		{
			if (_entry)
			{
				delete _entry;
				_entry = nullptr;
			}
			return true;
		});

	shaders.ForEachCheckErase(
		[](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry*& _entry)
		{
			if (_entry)
			{
				delete _entry;
				_entry = nullptr;
			}
			return true;
		});

	materials.ForEachCheckErase(
		[](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Material>::AssetEntry*& _entry)
		{
			if (_entry)
			{
				delete _entry;
				_entry = nullptr;
			}
			return true;
		});

	rendertargets.ForEachCheckErase(
		[](Pitaya::Core::GUID, Pitaya::Core::Asset<Pitaya::Asset::RenderTarget>::AssetEntry*& _entry)
		{
			if (_entry)
			{
				delete _entry;
				_entry = nullptr;
			}
			return true;
		});

	//registry.SerializeToFile();  //TOOD 序列化数据
}

bool Pitaya::Asset::AssetHub::TransformToVirtualPath(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath) const
{
	std::filesystem::path absolutePath = inputPath.is_absolute() ? inputPath : basePath / inputPath;
	try
	{
		absolutePath = std::filesystem::canonical(absolutePath);
	}
	catch(...)
	{
		try 
		{
			absolutePath = std::filesystem::weakly_canonical(absolutePath);
		}
		catch (...) 
		{
			absolutePath = std::filesystem::absolute(absolutePath);
		}
	}

	//如果是引擎内置资源
	if (Pitaya::Core::IsSubPath(absolutePath, engineRoot))
	{
		std::filesystem::path relative = std::filesystem::relative(absolutePath, engineRoot);
		out_virtualpath = "engine:/" + relative.generic_string();
		return true;
	}

	//如果是项目资源
	if (Pitaya::Core::IsSubPath(absolutePath, projectRoot))
	{
		std::filesystem::path relative = std::filesystem::relative(absolutePath, projectRoot);
		out_virtualpath = "project:/" + relative.generic_string();
		return true;
	}

	Pitaya::Log::Error("transform absolute to virtual path fail, absolute not engine or project, path:" + absolutePath.string());
	return false;
}
bool Pitaya::Asset::AssetHub::RegisterExternalFile(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath, Pitaya::Core::GUID& out_guid)
{
	if (!TransformToVirtualPath(inputPath, basePath, out_virtualpath))
	{
		Pitaya::Log::Error("register external file fail, transform fail, path:" + inputPath.string());
		return false;
	}

	//如果是引擎内置资源
	if (IsBuildInAsset(out_virtualpath.string()))
	{
		Pitaya::Log::Error("register external file fail, cant registe engine buildin asset, path:" + inputPath.string());
		return false;
	}
	
	//如果已经包含该路径
	bool hasRegisted = false;
	registry.Map.FindOperateKV(out_virtualpath,
		[&hasRegisted, &out_guid](const std::filesystem::path& _path, Pitaya::Core::GUID _guid)
		{
			hasRegisted = true;
			out_guid = _guid;
		},
		[](const std::filesystem::path& _path)
		{

		});
	if (hasRegisted)
	{
		Pitaya::Log::Info("erternal asset has registerd, path:" + inputPath.string());
		return true;
	}

	out_guid = Pitaya::Core::GUID::New();
	registry.Map.Insert(out_guid, out_virtualpath);
	return true;
}

Pitaya::Asset::AssetType Pitaya::Asset::AssetHub::GetAssetType(const std::filesystem::path& path) const
{
	if (!CheckIsVirtualPath(path))
	{
		Pitaya::Log::Error("get asset type fail! path is not virtual path, path:" + path.string());
		return Pitaya::Asset::AssetType::Unknown;
	}

	std::filesystem::path resolvePath = GetResolvePath(path);
	if (!CheckAssetValid(resolvePath))
	{
		Pitaya::Log::Error("get asset type fail! path invalid, path:" + resolvePath.string());
		return Pitaya::Asset::AssetType::Unknown;
	}

	if (std::filesystem::is_regular_file(resolvePath))
	{
		std::string ext = resolvePath.extension().string();
		Pitaya::Core::ToLower(ext);

		//Texture
		if (TextureExtensions.contains(ext) || ext == ".cubemap") { return Pitaya::Asset::AssetType::Texture; }

		//Shader
		if (ext == ".shader") {return Pitaya::Asset::AssetType::Shader;}

		//Material
		if (MaterialExtensions.contains(ext)) { return Pitaya::Asset::AssetType::Material; }

		//Mesh
		if (MeshExtensions.contains(ext)) { return Pitaya::Asset::AssetType::Mesh; }

		//RenderTarget
		if (RenderTargetExtensions.contains(ext)) { return Pitaya::Asset::AssetType::RenderTarget; }
	}

	Pitaya::Log::Error("unknwon path:" + path.string());
	return Pitaya::Asset::AssetType::Unknown;
}

void Pitaya::Asset::AssetHub::SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, std::monostate&)
{
	Pitaya::Log::Error("unknown cpu operate result");
}
void Pitaya::Asset::AssetHub::SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Import::Texture2DImportResult& cpuOpResult_Inner)
{
	std::string log;
	Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* entry = nullptr;
	textures.FindOperateKV(cpuOpResult_Inner.GUID,
		[&log, &entry, &cpuOpResult_Inner](Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "Texture AssetEntry Is Empty when GPU Process CPU Texture2D Load Result GUID: " + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "Texture asset marked as Unload, abort GPU load" + _guid.ToString();
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::GPULoading);
			entry = _entry;
		},
		[&log](Core::GUID _guid)
		{
			log = "Texture AssetEntry not found when GPU Process CPU Texture2D Load Result GUID: " + _guid.ToString();
		});
	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return; }

	auto* texture2D = entry->Data.load(std::memory_order_acquire);
	if (!texture2D)
	{
		Pitaya::Log::Error("texture asset entry data is Empty!" + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	Pitaya::GPU::Identifier gpuIdentifier = Pitaya::GPU::CreateTexture2D(passkey, cpuOpResult_Inner.Data.data(), cpuOpResult_Inner.Width, cpuOpResult_Inner.Height, cpuOpResult_Inner.Channels, cpuOpResult_Inner.IsGenerateMipmap, cpuOpResult_Inner.IsSRGB, cpuOpResult_Inner.isNearest);
	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("Texture asset marked as Unload, abort GPU load" + cpuOpResult_Inner.GUID.ToString());
		if (!Pitaya::GPU::DestroyTexture2D(passkey, gpuIdentifier))
		{
			Pitaya::Log::Error("destroy texture2D asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	texture2D->Id = static_cast<uint64_t>(gpuIdentifier);
	texture2D->Type = Pitaya::GPU::TextureType::Texture2D;
	entry->State.ModifyBits(Pitaya::Core::AssetState::GPULoaded, Pitaya::Core::AssetState::GPULoading);
}
void Pitaya::Asset::AssetHub::SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Import::TextureCubemapImportResult& cpuOpResult_Inner)
{
	std::string log;
	Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* entry = nullptr;
	textures.FindOperateKV(cpuOpResult_Inner.GUID,
		[&log, &entry, &cpuOpResult_Inner](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "texture asset entry is empty when GPU process CPU texture cubemap load result GUID: " + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "texture asset marked as Unload, abort GPU load" + _guid.ToString();
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::GPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "texture cubemap asset entry not found when GPU process CPU texture cubemap load result GUID: " + _guid.ToString();
		});
	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return; }

	auto* textureCubemap = entry->Data.load(std::memory_order_acquire);
	if (!textureCubemap)
	{
		Pitaya::Log::Error("texture cubemap asset entry data is Empty!" + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	unsigned char* datas[6] = {};
	for (uint32_t i = 0; i < 6; i++)
	{
		datas[i] = cpuOpResult_Inner.Data[i].data();
	}
	Pitaya::GPU::Identifier gpuIdentifier = Pitaya::GPU::CreateTextureCubemap(passkey, datas, cpuOpResult_Inner.Width, cpuOpResult_Inner.Height, cpuOpResult_Inner.Channels, cpuOpResult_Inner.IsGenerateMipmap, cpuOpResult_Inner.IsSRGB, cpuOpResult_Inner.isNearest);
	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("texture cubemap asset marked as Unload, abort GPU load" + cpuOpResult_Inner.GUID.ToString());
		if (!Pitaya::GPU::DestroyTextureCubemap(passkey, gpuIdentifier))
		{
			Pitaya::Log::Error("destroy texture cubemap asset fail, GUID:" + cpuOpResult_Inner.GUID.ToString());
		}
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	textureCubemap->Id = static_cast<uint64_t>(gpuIdentifier);
	textureCubemap->Type = Pitaya::GPU::TextureType::TextureCubemap;
	entry->State.ModifyBits(Pitaya::Core::AssetState::GPULoaded, Pitaya::Core::AssetState::GPULoading);
}
void Pitaya::Asset::AssetHub::SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Import::ShaderImportResult& cpuOpResult_Inner)
{
	std::string log;
	Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry* entry = nullptr;
	shaders.FindOperateKV(cpuOpResult_Inner.GUID,
		[&log,&entry,&cpuOpResult_Inner](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "Shader AssetEntry Is Empty when GPU Process CPU Shader Load Result GUID: " + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "Shader asset marked as Unload, abort GPU load" + _guid.ToString();
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::GPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "Shader AssetEntry not found when GPU Process CPU Shader Load Result GUID: " + _guid.ToString();
		});
	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return; }

	auto* shader = entry->Data.load(std::memory_order_acquire);
	if (!shader)
	{
		Pitaya::Log::Error("Shader asset entry data is Empty!" + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	Pitaya::GPU::Identifier gpuIdentifier = Pitaya::GPU::Identifier<Pitaya::GPU::Shader>::Invalid;
	if (cpuOpResult_Inner.Type == Pitaya::GPU::Shader::VF)
	{
		if (!cpuOpResult_Inner.VertexSource.empty() && !cpuOpResult_Inner.FragmentSource.empty())
		{
			gpuIdentifier = Pitaya::GPU::CreateShader(passkey, cpuOpResult_Inner.VertexSource.c_str(), cpuOpResult_Inner.FragmentSource.c_str());
		}
		else
		{
			Pitaya::Log::Error("cpu load shader result is empty guid:" + cpuOpResult_Inner.GUID.ToString());
		}
	}
	else if (cpuOpResult_Inner.Type == Pitaya::GPU::Shader::VFG)
	{
		if (!cpuOpResult_Inner.VertexSource.empty() && !cpuOpResult_Inner.FragmentSource.empty() && !cpuOpResult_Inner.GeometrySource.empty())
		{
			gpuIdentifier = Pitaya::GPU::CreateShader(passkey, cpuOpResult_Inner.VertexSource.c_str(), cpuOpResult_Inner.FragmentSource.c_str(), cpuOpResult_Inner.GeometrySource.c_str());
		}
		else
		{
			Pitaya::Log::Error("cpu load shader result is empty guid:" + cpuOpResult_Inner.GUID.ToString());
		}
	}
	else
	{
		Pitaya::Log::Error("erro shader type guid:" + cpuOpResult_Inner.GUID.ToString());
	}

	//无效句柄未加载GPU数据
	if (!gpuIdentifier)
	{
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		Pitaya::Log::Error("cpu load shader result is empty guid:" + cpuOpResult_Inner.GUID.ToString());
		return;
	}

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		Pitaya::Log::Error("Shader asset marked as Unload, abort GPU load" + cpuOpResult_Inner.GUID.ToString());
		if (!Pitaya::GPU::DestroyShader(passkey, gpuIdentifier))
		{
			Pitaya::Log::Error("Shader asset destroy fali GUID:" + cpuOpResult_Inner.GUID.ToString());
		}
		return;
	}

	Pitaya::Log::Info("shader gpu load success, GUID:" + cpuOpResult_Inner.GUID.ToString());
	entry->Data.load(std::memory_order_acquire)->ID = gpuIdentifier;
	entry->State.ModifyBits(Pitaya::Core::AssetState::GPULoaded, Pitaya::Core::AssetState::GPULoading);
}
void Pitaya::Asset::AssetHub::SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Import::StaticMeshImportResult& cpuOpResult_Inner)
{
	std::string log;
	Pitaya::Core::Asset<Pitaya::Asset::Mesh>::AssetEntry* entry = nullptr;
	meshes.FindOperateKV(cpuOpResult_Inner.GUID,
		[&log, &entry, &cpuOpResult_Inner](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Mesh>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "mesh asset entry is empty when GPU process CPU mesh load result GUID: " + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "mesh asset marked as Unload, abort GPU load: " + _guid.ToString();
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::GPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "mesh asset entry not found when GPU process CPU mesh load result GUID: " + _guid.ToString();
		});

	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return; }

	if (!cpuOpResult_Inner.IsValid || cpuOpResult_Inner.Vertices.empty() || cpuOpResult_Inner.Indices.empty())
	{
		Pitaya::Log::Error("Invalid CPU mesh data for GUID: " + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("mesh asset marked as Unload, abort GPU load" + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	auto* mesh = entry->Data.load(std::memory_order_acquire);
	if (!mesh)
	{
		Pitaya::Log::Error("Failed to get mesh entry data GUID: " + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	Pitaya::GPU::Identifier vaoId = Pitaya::GPU::CreateVertexArray(passkey);
	if (!vaoId)
	{
		Pitaya::Log::Error("failed to create VAO for GUID: " + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	Pitaya::GPU::VertexArray* vao = Pitaya::GPU::GetVertexArray(passkey, vaoId);
	if (!vao)
	{
		Pitaya::Log::Error("failed to get VAO instance for GUID: " + cpuOpResult_Inner.GUID.ToString());
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	vao->Bind();

	uint32_t vboSize = static_cast<uint32_t>(cpuOpResult_Inner.Vertices.size() * sizeof(float));
	Pitaya::GPU::Identifier vboId = Pitaya::GPU::CreateVertexBuffer(passkey, cpuOpResult_Inner.Vertices.data(), vboSize);
	if (!vboId)
	{
		Pitaya::Log::Error("failed to create VBO for GUID: " + cpuOpResult_Inner.GUID.ToString());
		vao->Unbind();
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	Pitaya::GPU::VertexBuffer* vbo = Pitaya::GPU::GetVertexBuffer(passkey, vboId);
	if (!vbo)
	{
		Pitaya::Log::Error("Failed to get VBO instance for GUID: " + cpuOpResult_Inner.GUID.ToString());
		vao->Unbind();
		Pitaya::GPU::DestroyVertexBuffer(passkey, vboId);
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	vbo->SetLayout(cpuOpResult_Inner.VertexLayout);
	vao->AddVertexBuffer(vbo);

	uint32_t indexCount = static_cast<uint32_t>(cpuOpResult_Inner.Indices.size());
	Pitaya::GPU::Identifier iboId = Pitaya::GPU::CreateIndexBuffer(passkey, cpuOpResult_Inner.Indices.data(), indexCount);
	if (!iboId)
	{
		Pitaya::Log::Error("failed to create IBO for GUID: " + cpuOpResult_Inner.GUID.ToString());
		vao->Unbind();
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	Pitaya::GPU::IndexBuffer* ibo = Pitaya::GPU::GetIndexBuffer(passkey, iboId);
	if (!ibo)
	{
		Pitaya::Log::Error("Failed to get IBO instance for GUID: " + cpuOpResult_Inner.GUID.ToString());
		vao->Unbind();
		Pitaya::GPU::DestroyIndexBuffer(passkey, iboId);
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	vao->SetIndexBuffer(ibo);

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("StaticMesh marked as Unload during upload, cleaning up GPU resources: " + cpuOpResult_Inner.GUID.ToString());
		if (!Pitaya::GPU::DestroyVertexArray(passkey, vaoId))
		{
			Pitaya::Log::Error("StaticMesh VAO destry fail GUID" + cpuOpResult_Inner.GUID.ToString());
		}
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	mesh->VertexArray = vaoId;
	mesh->BoundingBox = cpuOpResult_Inner.BoundingBox;
	mesh->SubMeshs = std::move(cpuOpResult_Inner.SubMeshs);
	mesh->Type = Pitaya::GPU::MeshType::Static;
	for (const auto& materialGUID : cpuOpResult_Inner.MaterialGUIDs)
	{
		mesh->Materials.emplace_back(Pitaya::Asset::LoadAsset<Pitaya::Asset::Material>(materialGUID));
	}
	vao->Unbind();

	Pitaya::Log::Info("Successfully created GPU resources for static mesh GUID: " + cpuOpResult_Inner.GUID.ToString());
	entry->State.ModifyBits(Pitaya::Core::AssetState::GPULoaded, Pitaya::Core::AssetState::GPULoading);
}
void Pitaya::Asset::AssetHub::SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Import::SkinnedMeshImportResult& cpuOpResult_Inner)
{
	std::string log;
	Pitaya::Core::Asset<Pitaya::Asset::Mesh>::AssetEntry* entry = nullptr;
	meshes.FindOperateKV(cpuOpResult_Inner.GUID,
		[&log, &entry, &cpuOpResult_Inner](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Mesh>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "mesh asset entry is empty when GPU process CPU mesh load result GUID: " + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "mesh asset marked as Unload, abort GPU load: " + _guid.ToString();
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::GPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "mesh asset entry not found when GPU process CPU mesh load result GUID: " + _guid.ToString();
		});

	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return; }

	if (!cpuOpResult_Inner.IsValid || cpuOpResult_Inner.Vertices.empty() || cpuOpResult_Inner.Indices.empty())
	{
		Pitaya::Log::Error("Invalid CPU skinned mesh data for GUID: " + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("skinned mesh asset marked as Unload, abort GPU load" + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	auto* mesh = entry->Data.load(std::memory_order_acquire);
	if (!mesh)
	{
		Pitaya::Log::Error("Failed to get skinned mesh entry data GUID: " + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	Pitaya::GPU::Identifier vaoId = Pitaya::GPU::CreateVertexArray(passkey);
	if (!vaoId)
	{
		Pitaya::Log::Error("failed to create VAO for skinned mesh GUID: " + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	Pitaya::GPU::VertexArray* vao = Pitaya::GPU::GetVertexArray(passkey, vaoId);
	if (!vao)
	{
		Pitaya::Log::Error("failed to get VAO instance for skinned mesh GUID: " + cpuOpResult_Inner.GUID.ToString());
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	vao->Bind();

	uint32_t vboSize = static_cast<uint32_t>(cpuOpResult_Inner.Vertices.size() * sizeof(float));
	Pitaya::GPU::Identifier vboId = Pitaya::GPU::CreateVertexBuffer(passkey, cpuOpResult_Inner.Vertices.data(), vboSize);
	if (!vboId)
	{
		Pitaya::Log::Error("failed to create VBO for skinned mesh GUID: " + cpuOpResult_Inner.GUID.ToString());
		vao->Unbind();
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	Pitaya::GPU::VertexBuffer* vbo = Pitaya::GPU::GetVertexBuffer(passkey, vboId);
	if (!vbo)
	{
		Pitaya::Log::Error("Failed to get VBO instance for skinned mesh GUID: " + cpuOpResult_Inner.GUID.ToString());
		vao->Unbind();
		Pitaya::GPU::DestroyVertexBuffer(passkey, vboId);
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	vbo->SetLayout(cpuOpResult_Inner.VertexLayout);
	vao->AddVertexBuffer(vbo);

	uint32_t indexCount = static_cast<uint32_t>(cpuOpResult_Inner.Indices.size());
	Pitaya::GPU::Identifier iboId = Pitaya::GPU::CreateIndexBuffer(passkey, cpuOpResult_Inner.Indices.data(), indexCount);
	if (!iboId)
	{
		Pitaya::Log::Error("failed to create IBO for skinned mesh GUID: " + cpuOpResult_Inner.GUID.ToString());
		vao->Unbind();
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	Pitaya::GPU::IndexBuffer* ibo = Pitaya::GPU::GetIndexBuffer(passkey, iboId);
	if (!ibo)
	{
		Pitaya::Log::Error("Failed to get IBO instance for skinned mesh GUID: " + cpuOpResult_Inner.GUID.ToString());
		vao->Unbind();
		Pitaya::GPU::DestroyIndexBuffer(passkey, iboId);
		Pitaya::GPU::DestroyVertexArray(passkey, vaoId);
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}
	vao->SetIndexBuffer(ibo);
	
	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("SkinnedMesh marked as Unload during upload, cleaning up GPU resources: " + cpuOpResult_Inner.GUID.ToString());
		if (!Pitaya::GPU::DestroyVertexArray(passkey, vaoId))
		{
			Pitaya::Log::Error("SkinnedMesh VAO destry fail GUID" + cpuOpResult_Inner.GUID.ToString());
		}
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	mesh->VertexArray = vaoId;
	mesh->SubMeshs = std::move(cpuOpResult_Inner.SubMeshs);

	mesh->BoneInverseMatrices = std::move(cpuOpResult_Inner.BoneInverseMatrices);
	mesh->BoneCount = cpuOpResult_Inner.BoneCount;
	mesh->BoundingBox = cpuOpResult_Inner.BoundingBox;
	for (const auto& materialGUID : cpuOpResult_Inner.MaterialGUIDs)
	{
		mesh->Materials.emplace_back(Pitaya::Asset::LoadAsset<Pitaya::Asset::Material>(materialGUID));
	}
	mesh->Type = Pitaya::GPU::MeshType::SkinnedMesh;

	vao->Unbind();
	Pitaya::Log::Info("successfully created GPU resources for skinned mesh GUID: " + cpuOpResult_Inner.GUID.ToString());
	entry->State.ModifyBits(Pitaya::Core::AssetState::GPULoaded, Pitaya::Core::AssetState::GPULoading);
}
void Pitaya::Asset::AssetHub::SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Import::RenderTargetImportResult& cpuOpResult_Inner)
{
	std::string log;
	Pitaya::Core::Asset<Pitaya::Asset::RenderTarget>::AssetEntry* entry = nullptr;
	rendertargets.FindOperateKV(cpuOpResult_Inner.GUID,
		[&log, &entry, &cpuOpResult_Inner](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::RenderTarget>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "rendertarget asset entry is empty when GPU process CPU mesh load result GUID: " + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "rendertarget asset marked as Unload, abort GPU load: " + _guid.ToString();
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::GPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "rendertarget asset entry not found when GPU process CPU mesh load result GUID: " + _guid.ToString();
		});

	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return; }

	auto* rendertarget = entry->Data.load(std::memory_order_acquire);
	if (!rendertarget)
	{
		Pitaya::Log::Error("rendertarget asset entry data is Empty!" + cpuOpResult_Inner.GUID.ToString());
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	//Scene
	Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> sceneGPUIdentifier = Pitaya::GPU::CreateFrameBuffer(passkey, cpuOpResult_Inner.SceneFrameBufferSpecification);
	//PingPong
	Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> pingPongGPUIdentifier[2] = { Pitaya::GPU::CreateFrameBuffer(passkey, cpuOpResult_Inner.PingPongFrameBufferSpecification), Pitaya::GPU::CreateFrameBuffer(passkey, cpuOpResult_Inner.PingPongFrameBufferSpecification) };
	//Final
	Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> finalGPUIdentifier = Pitaya::GPU::CreateFrameBuffer(passkey, cpuOpResult_Inner.FinalFrameBufferSpecification);
	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("rendertarget asset marked as Unload, abort GPU load" + cpuOpResult_Inner.GUID.ToString());
		if (!Pitaya::GPU::DestroyFrameBuffer(passkey, sceneGPUIdentifier))
		{
			Pitaya::Log::Error("destroy scene frambuffer asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		if (!Pitaya::GPU::DestroyFrameBuffer(passkey, pingPongGPUIdentifier[0]))
		{
			Pitaya::Log::Error("destroy pingPong A frambuffer asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		if (!Pitaya::GPU::DestroyFrameBuffer(passkey, pingPongGPUIdentifier[1]))
		{
			Pitaya::Log::Error("destroy pingPong B frambuffer asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		if (!Pitaya::GPU::DestroyFrameBuffer(passkey, finalGPUIdentifier))
		{
			Pitaya::Log::Error("destroy final frambuffer asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	Pitaya::GPU::FrameBuffer* sceneFrambuffer = Pitaya::GPU::GetFrameBuffer(passkey, sceneGPUIdentifier);
	Pitaya::GPU::FrameBuffer* pingPongFrambuffer[2] = { Pitaya::GPU::GetFrameBuffer(passkey, pingPongGPUIdentifier[0]), Pitaya::GPU::GetFrameBuffer(passkey, pingPongGPUIdentifier[1]) };
	Pitaya::GPU::FrameBuffer* finalFrambuffer = Pitaya::GPU::GetFrameBuffer(passkey, finalGPUIdentifier);
	if (!sceneFrambuffer || !pingPongFrambuffer[0] || !pingPongFrambuffer[1] || !finalFrambuffer)
	{
		Pitaya::Log::Error("framebuffer is empty GUID:" + cpuOpResult_Inner.GUID.ToString());
		if (!Pitaya::GPU::DestroyFrameBuffer(passkey, sceneGPUIdentifier))
		{
			Pitaya::Log::Error("destroy scene frambuffer asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		if (!Pitaya::GPU::DestroyFrameBuffer(passkey, pingPongGPUIdentifier[0]))
		{
			Pitaya::Log::Error("destroy pingPong A frambuffer asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		if (!Pitaya::GPU::DestroyFrameBuffer(passkey, pingPongGPUIdentifier[1]))
		{
			Pitaya::Log::Error("destroy pingPong B frambuffer asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		if (!Pitaya::GPU::DestroyFrameBuffer(passkey, finalGPUIdentifier))
		{
			Pitaya::Log::Error("destroy final frambuffer asset gail,GUID: " + cpuOpResult_Inner.GUID.ToString());
		}
		entry->State.ModifyBits(Pitaya::Core::AssetState::GPUFailed, Pitaya::Core::AssetState::GPULoading);
		return;
	}

	Pitaya::Log::Info("successfully created GPU resources for framebuffer GUID: " + cpuOpResult_Inner.GUID.ToString());
	rendertarget->SceneFrameBuffer = sceneGPUIdentifier;
	rendertarget->SceneInternalFrameBuffer = sceneFrambuffer->GetInternalGPUIdentifier();
	rendertarget->SceneColorAttachment = sceneFrambuffer->GetColorAttachmentGPUIdentifier();
	rendertarget->PingPongFrameBuffers[0] = pingPongGPUIdentifier[0];
	rendertarget->PingPongColorAttachments[0] = pingPongFrambuffer[0]->GetColorAttachmentGPUIdentifier();
	rendertarget->PingPongFrameBuffers[1] = pingPongGPUIdentifier[1];
	rendertarget->PingPongColorAttachments[1] = pingPongFrambuffer[1]->GetColorAttachmentGPUIdentifier();
	rendertarget->FinalFrameBuffer = finalGPUIdentifier;
	rendertarget->FinalColorAttachment = finalFrambuffer->GetColorAttachmentGPUIdentifier();
	entry->State.ModifyBits(Pitaya::Core::AssetState::GPULoaded, Pitaya::Core::AssetState::GPULoading);
}
void Pitaya::Asset::AssetHub::SyncAssetOperate(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Asset::Texture2DUnloadRequire& cpuOpResult_Inner)
{
	Pitaya::GPU::DestroyTexture2D(passkey, cpuOpResult_Inner.ID);
}

bool Pitaya::Asset::AssetHub::CheckIsVirtualPath(const std::filesystem::path& path) const
{
	std::string pathstr = path.string();
	return pathstr.starts_with("engine:/") || pathstr.starts_with("project:/");
}
bool Pitaya::Asset::AssetHub::CheckAssetValid(const std::filesystem::path& path) const
{
	//TODO 后续可升级为检测文件是否符合加载条件，如果不符合不会加载
	return !path.empty() && std::filesystem::exists(path);
}

bool Pitaya::Asset::AssetHub::CheckIsValidTexture2DFile(const std::filesystem::path& file) const
{
	if (!std::filesystem::is_regular_file(file)) { return false; }

	std::string ext = file.extension().string();
	Pitaya::Core::ToLower(ext);
	if (TextureExtensions.find(ext) == TextureExtensions.end())
	{
		Pitaya::Log::Warning("debug texture2D extension" + ext + ", path:" + file.string());
		return false;
	}
	return true;
}
bool Pitaya::Asset::AssetHub::CheckIsValidTextureCubeMapFolder(const std::filesystem::path& folder, std::vector<std::filesystem::path>& out) const
{
	if (!std::filesystem::is_directory(folder)) { return false; }

	out.clear();
	out.resize(static_cast<size_t>(Pitaya::GPU::TextureCubemapFace::Unknown));

	uint32_t validCount = 0;
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(folder))
		{
			if (!entry.is_regular_file()) { continue; }

			std::filesystem::path path = entry.path();
			std::string ext = path.extension().string();;
			Pitaya::Core::ToLower(ext);
			if (ext == ".cubemap") { continue; }
			if (TextureExtensions.find(ext) == TextureExtensions.end())
			{
				Pitaya::Log::Warning("texturecubemap extension" + ext + ", path:" + path.string());
				continue;
			}

			std::string name = path.stem().string();
			Pitaya::GPU::TextureCubemapFace face = Pitaya::GPU::ToEnum<Pitaya::GPU::TextureCubemapFace>(name);
			if (face != Pitaya::GPU::TextureCubemapFace::Unknown && out[static_cast<size_t>(face)].empty())
			{
				validCount++;
				out[static_cast<size_t>(face)] = std::move(path);
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		Pitaya::Log::Error("traverse directory fail in CheckIsValidTextureCubeMapFolder! path:" + folder.string() + ", error:" + e.what());
	}
	catch (...)
	{
		Pitaya::Log::Error("traverse directory fail in CheckIsValidTextureCubeMapFolder! path:" + folder.string() + ", unknwon error");
	}
	return validCount == 6;
}
bool Pitaya::Asset::AssetHub::CheckIsValidShaderFloder(const std::filesystem::path& folder, std::unordered_map<Pitaya::GPU::ShaderType, std::filesystem::path>& out) const
{
	if (!std::filesystem::is_directory(folder)) { return false; }

	out.clear();
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(folder))
		{
			if (!entry.is_regular_file()) { continue; }

			std::string ext = entry.path().extension().string();
			Pitaya::Core::ToLower(ext);
			if (ext == ".shader") { continue; }

			auto extIt = ShaderExtensions.find(ext);
			if (extIt == ShaderExtensions.end())
			{
				Pitaya::Log::Warning("unsupported shader file extension: " + ext + " in folder: " + folder.string());
				continue;
			}

			Pitaya::GPU::ShaderType shaderType = extIt->second;
			auto shaderIt = out.find(shaderType);
			if (shaderIt != out.end())
			{
				Pitaya::Log::Error(
					"Duplicate shader type in folder: " + folder.string() + "\n"
					"  New path: " + entry.path().string() + "\n"
					"  Old path: " + shaderIt->second.string());
			}
			out[shaderType] = entry.path();
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		Pitaya::Log::Error("traverse directory fail in CheckIsValidShaderFloder! path:" + folder.string() + ", error:" + e.what());
	}
	catch (...)
	{
		Pitaya::Log::Error("traverse directory fail in CheckIsValidTextureCubeMapFolder! path:" + folder.string() + ", unknwon error");
	}
	return out.find(Pitaya::GPU::ShaderType::Vertex) != out.end() &&
		out.find(Pitaya::GPU::ShaderType::Fragment) != out.end(); //至少需要顶点着色器和片元着色器
}
bool Pitaya::Asset::AssetHub::CheckIsValidMaterialFile(const std::filesystem::path& file) const
{
	if (!std::filesystem::is_regular_file(file)) { return false; }

	std::string ext = file.extension().string();
	Pitaya::Core::ToLower(ext);
	return MaterialExtensions.contains(ext);
}
bool Pitaya::Asset::AssetHub::CheckIsValidRenderTargetFile(const std::filesystem::path& file) const
{
	if (!std::filesystem::is_regular_file(file)) { return false; }

	std::string ext = file.extension().string();
	Pitaya::Core::ToLower(ext);
	return RenderTargetExtensions.contains(ext);
}
bool Pitaya::Asset::AssetHub::CheckIsValidMeshFile(const std::filesystem::path& path) const
{
	if (!std::filesystem::is_regular_file(path)) 
	{ 
		Pitaya::Log::Error("mesh asset not file, path:" + path.string());
		return false; 
	}
	
	std::string ext = path.extension().string();
	Pitaya::Core::ToLower(ext);
	if (MeshExtensions.find(ext) == MeshExtensions.end())
	{
		Pitaya::Log::Warning("error mesh file extension" + ext);
		return false;
	}
	return true;
}

void Pitaya::Asset::AssetHub::LoadTextureAsset(Pitaya::Core::GUID guid, const std::filesystem::path& path)
{
	//文件为Texture2D
	if (CheckIsValidTexture2DFile(path))
	{
		Pitaya::Import::Texture2DImportResult cpuOpResult_Inner;
		if (LoadTexture2DAsset(guid, path, cpuOpResult_Inner))
		{
			Pitaya::Log::Info(path.string() + "Load Success");
			Pitaya::Asset::AssetOperate cpuOpResult_Out;
			cpuOpResult_Out.Data = cpuOpResult_Inner;
			assetOperateQueue.Push(cpuOpResult_Out);
		}
		else
		{
			Pitaya::Log::Error(path.string() + "Load Fail");
		}
		return;
	}

	//文件为TextureCubemap
	std::vector<std::filesystem::path> outFaceFiles;
	if (CheckIsValidTextureCubeMapFolder(path.parent_path(), outFaceFiles))
	{
		Pitaya::Import::TextureCubemapImportResult cpuOpResult_Inner;
		if (LoadTextureCubemapAsset(guid, path, outFaceFiles, cpuOpResult_Inner))
		{
			Pitaya::Asset::AssetOperate cpuOpResult_Out;
			cpuOpResult_Out.Data = cpuOpResult_Inner;
			assetOperateQueue.Push(cpuOpResult_Out);
		}
		else
		{
			Pitaya::Log::Error(path.string() + "Load Fail");
		}
		return;
	}

	//文件格式异常 标记CPU加载失败
	std::string log;
	textures.FindOperateKV(guid,
		[&log, &path](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = _guid.ToString() + "Texture AssetEntry is empty path:" + path.string();
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
		},
		[&log, &path](Pitaya::Core::GUID _guid)
		{
			log = _guid.ToString() + "Texture AssetEntry not found" + path.string();
		});
	if (!log.empty()) { Pitaya::Log::Error(log); }
	Pitaya::Log::Error("unknow texture file , path: " + path.string());
}
bool Pitaya::Asset::AssetHub::LoadTexture2DAsset(Pitaya::Core::GUID guid, const std::filesystem::path& path, Pitaya::Import::Texture2DImportResult& result)
{
	std::string log;
	Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* entry = nullptr;
	textures.FindOperateKV(guid,
		[&path, &log, &entry](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "Texture AssetEntry not found" + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "Texture asset marked as Unload, abort CPU load: " + path.string();
				_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "Texture AssetEntry not found" + _guid.ToString();
		});
	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return false; }

	if (!Pitaya::Import::Import(guid, path, true, true, result))
	{
		Pitaya::Log::Error("texture load failed: " + path.string());
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("Texture asset marked as Unload, abort CPU load: " + path.string());
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	entry->State.ModifyBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::CPULoading);
	return true;
}
bool Pitaya::Asset::AssetHub::LoadTextureCubemapAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file,const std::vector<std::filesystem::path>& paths, Pitaya::Import::TextureCubemapImportResult& result)
{
	std::string log;
	Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* entry = nullptr;
	textures.FindOperateKV(guid,
		[&file, &log, &entry](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "texture cubemap asset entry not found" + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "texture cubemap asset marked as unload, abort CPU load: " + file.string();
				_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "texture cubemap asset entry not found" + _guid.ToString();
		});
	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return false; }

	if (!Pitaya::Import::Import(guid, file, paths,true, true, result))
	{
		Pitaya::Log::Error("texture cubemap load failed: " + file.string());
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("texture cubemap asset marked as Unload, abort CPU load: " + file.string());
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	entry->State.ModifyBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::CPULoading);
	return true;
}

void Pitaya::Asset::AssetHub::LoadShaderAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file)
{
	std::filesystem::path folder = file.parent_path();
	std::unordered_map<Pitaya::GPU::ShaderType, std::filesystem::path> out_paths;
	if (!CheckIsValidShaderFloder(folder, out_paths))
	{
		Pitaya::Log::Error(guid.ToString() + " shader asset is invalid path: " + folder.string());
		return;
	}

	std::filesystem::path vertexPath;
	std::filesystem::path fragmentPath;
	std::filesystem::path geometryPath;

	Pitaya::Core::StateFlags<Pitaya::GPU::ShaderType> state = Pitaya::GPU::ShaderType::Empty;
	auto iterator = out_paths.begin();
	while (iterator != out_paths.end())
	{
		switch (iterator->first)
		{
			case Pitaya::GPU::ShaderType::Vertex:   vertexPath = iterator->second;   break;
			case Pitaya::GPU::ShaderType::Fragment: fragmentPath = iterator->second; break;
			case Pitaya::GPU::ShaderType::Geometry: geometryPath = iterator->second; break;

			case Pitaya::GPU::ShaderType::Empty: Pitaya::Log::Error("empty state mark!"); break;
			default:							 Pitaya::Log::Error("empty state mark!"); break;
		}

		state.SetBits(iterator->first);
		iterator++;
	}

	if (state == Pitaya::GPU::Shader::VF)
	{
		Pitaya::Import::ShaderImportResult cpuOpResult_Inner;
		if (LoadVFShaderAsset(guid, folder, vertexPath, fragmentPath, cpuOpResult_Inner))
		{
			Pitaya::Log::Info(folder.string() + " load success, in shader VF");
			Pitaya::Asset::AssetOperate cpuOpResult_Out;
			cpuOpResult_Out.Data = cpuOpResult_Inner;
			assetOperateQueue.Push(cpuOpResult_Out);
		}
		else
		{
			Pitaya::Log::Error(folder.string() + " load fail");
		}
		return;
	}

	if (state == Pitaya::GPU::Shader::VFG)
	{
		Pitaya::Import::ShaderImportResult cpuOpResult_Inner;
		if (LoadVFGShaderAsset(guid, folder, vertexPath, fragmentPath, geometryPath,cpuOpResult_Inner))
		{
			Pitaya::Log::Info(folder.string() + " load success");
			Pitaya::Asset::AssetOperate cpuOpResult_Out;
			cpuOpResult_Out.Data = cpuOpResult_Inner;
			assetOperateQueue.Push(cpuOpResult_Out);
		}
		else
		{
			Pitaya::Log::Error(folder.string() + " load fail");
		}
		return;
	}

	//异常状态Shader 标记CPU加载失败
	std::string log;
	shaders.FindOperateKV(guid,
		[&log, &folder](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = _guid.ToString() + "Shader AssetEntry is empty path:" + folder.string();
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
		},
		[&log, &folder](Pitaya::Core::GUID _guid)
		{
			log = _guid.ToString() + "Shader AssetEntry not found" + folder.string();
		});
	if (!log.empty()) { Pitaya::Log::Error(log); }
	Pitaya::Log::Error("Unknow shader file, path: " + folder.string());
}
bool Pitaya::Asset::AssetHub::LoadVFShaderAsset(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath,
	const std::filesystem::path& fragmentPath, Pitaya::Import::ShaderImportResult& cpuOpResult_Inner)
{
	std::string log;
	std::string strPath = folder.string();
	
	Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry* entry = nullptr;
	shaders.FindOperateKV(guid,
		[strPath, &log, &entry](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "shader AssetEntry not found" + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "shader asset marked as Unload, abort CPU load: " + strPath;
				_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "Shader AssetEntry not found" + _guid.ToString();
		});

	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return false; }

	if (!Pitaya::Import::Import(guid, folder, vertexPath, fragmentPath, cpuOpResult_Inner))
	{
		Pitaya::Log::Error("shader load failed: " + strPath);
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("shader asset marked as Unload, abort CPU load: " + strPath);
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	entry->State.ModifyBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::CPULoading);
	return true;
}
bool Pitaya::Asset::AssetHub::LoadVFGShaderAsset(Pitaya::Core::GUID guid, const std::filesystem::path& folder, const std::filesystem::path& vertexPath,
	const std::filesystem::path& fragmentPath, const std::filesystem::path& geometry, Pitaya::Import::ShaderImportResult& cpuOpResult_Inner)
{
	std::string log;
	std::string strPath = folder.string();
	
	Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry* entry = nullptr;
	shaders.FindOperateKV(guid,
		[strPath, &log, &entry](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "shader assetEntry not found" + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "shader asset marked as Unload, abort CPU load: " + strPath;
				_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "Shader AssetEntry not found" + _guid.ToString();
		});

	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return false; }

	if (!Pitaya::Import::Import(guid, folder, vertexPath, fragmentPath, geometry, cpuOpResult_Inner))
	{
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("Shader asset marked as Unload, abort CPU load: " + strPath);
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	entry->State.ModifyBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::CPULoading);
	return true;
}

void Pitaya::Asset::AssetHub::LoadMeshAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file)
{
	if (!CheckIsValidMeshFile(file))
	{
		Pitaya::Log::Error(guid.ToString() + " mesh path error, path:" + file.string());
		return;
	}

	Pitaya::Import::MeshPreloadResult preloadResult;
	if (!Pitaya::Import::Import(guid, file, preloadResult))
	{
		Pitaya::Log::Error("GUID:" + guid.ToString() + " preload fail, path:" + file.string());
		return;
	}

	if (preloadResult.Type == Pitaya::GPU::MeshType::Static)
	{
		Pitaya::Import::StaticMeshImportResult cpuOpResult_Inner;
		if (LoadStaticMeshAsset(guid, file, preloadResult, cpuOpResult_Inner))
		{	
			Pitaya::Log::Info("static mesh asset load success in cpu");
			Pitaya::Asset::AssetOperate cpuOpResult_Out;
			cpuOpResult_Out.Data = cpuOpResult_Inner;
			assetOperateQueue.Push(cpuOpResult_Out);
		}
		else
		{
			Pitaya::Log::Error("static mesh load fail in cpu");
		}
		return;
	}

	if (preloadResult.Type == Pitaya::GPU::MeshType::SkinnedMesh)
	{
		//TODO 加载SkinedMesh
		return;
	}

	Pitaya::Log::Error("load emsh fail,unknwon mesh type, GUID:" + guid.ToString() + " ,path:" + file.string());
}
bool Pitaya::Asset::AssetHub::LoadStaticMeshAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& preloadResult, Pitaya::Import::StaticMeshImportResult& out)
{
	std::string log;
	std::string path = file.string();
	Pitaya::Core::Asset<Pitaya::Asset::Mesh>::AssetEntry* entry = nullptr;
	meshes.FindOperateKV(guid,
		[path, &log, &entry](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Mesh>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "mesh asset entry is empty, GUID:" + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "mesh asset marked as Unload, abort CPU load: " + path;
				_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "mesh asset entry not found" + _guid.ToString();
		});

	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return false; }

	if (!Pitaya::Import::Import(guid, file, preloadResult, out))
	{
		Pitaya::Log::Error("static mesh load fail, GUID:" + guid.ToString() + ", path:" + path);
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
	{
		Pitaya::Log::Error("mesh asset marked as Unload, abort CPU load: " + path);
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return false;
	}

	entry->State.ModifyBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::CPULoading);
	return true;
}
bool Pitaya::Asset::AssetHub::LoadSkinnedMeshAsset(Pitaya::Core::GUID guid, const std::filesystem::path& path, const Pitaya::Import::MeshPreloadResult& detectResult,Pitaya::Import::SkinnedMeshImportResult& out)
{
	//TODO 加载骨骼动画
	Pitaya::Log::Debug("current engine version cant load skinned mesh asset, TAT... GUID:" +
		guid.ToString() + ".path:" + path.string());
	return false;
}

void Pitaya::Asset::AssetHub::LoadMaterialAsset(Pitaya::Core::GUID guid, const std::filesystem::path& file)
{
	if (!CheckIsValidMaterialFile(file))
	{
		Pitaya::Log::Error("material invalid, path:" + file.string());
		return;
	}

	std::string log;
	std::string path = file.string();
	Pitaya::Core::Asset<Pitaya::Asset::Material>::AssetEntry* entry = nullptr;
	materials.FindOperateKV(guid,
		[path,&log,&entry](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::Material>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "material asset entry is empty" + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "material asset marked as unload, abort CPU load: " + path;
				_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "material asset entry not found" + _guid.ToString();
		});
	if (!log.empty()){ Pitaya::Log::Error(log); }
	if (!entry) { return; }

	auto* material = entry->Data.load(std::memory_order_acquire);
	if (!material)
	{
		Pitaya::Log::Error(guid.ToString() + " asset entry data is empty,path:" + path);
		return;
	}

	if (!material->DeserializeFromFile(file))
	{
		Pitaya::Log::Error("material deserialize fail, path:" + file.string());
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return;
	}

	//Material无GPU资源 直接标记为GPULoaded即可
	entry->State.SetBits(Pitaya::Core::AssetState::GPULoaded);
	entry->State.ModifyBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::CPULoading);
	Pitaya::Log::Info("material load success, path: " + path + " , GUID: " + guid.ToString());
}

void Pitaya::Asset::AssetHub::LoadRenderTarget(Pitaya::Core::GUID guid, const std::filesystem::path& file)
{
	if (!CheckIsValidRenderTargetFile(file))
	{
		Pitaya::Log::Error("rendertarget invalid, path:" + file.string());
		return;
	}

	std::string log;
	std::string path = file.string();
	Pitaya::Core::Asset<Pitaya::Asset::RenderTarget>::AssetEntry* entry = nullptr;
	rendertargets.FindOperateKV(guid,
		[path, &log, &entry](Pitaya::Core::GUID _guid, Pitaya::Core::Asset<Pitaya::Asset::RenderTarget>::AssetEntry* _entry)
		{
			if (!_entry)
			{
				log = "rendertarget asset entry is empty" + _guid.ToString();
				return;
			}

			if (_entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				log = "rendertarget asset marked as unload, abort CPU load: " + path;
				_entry->State.SetBits(Pitaya::Core::AssetState::CPUFailed);
				return;
			}

			_entry->State.SetBits(Pitaya::Core::AssetState::CPULoading);
			entry = _entry;
		},
		[&log](Pitaya::Core::GUID _guid)
		{
			log = "rendertarget asset entry not found" + _guid.ToString();
		});
	if (!log.empty()) { Pitaya::Log::Error(log); }
	if (!entry) { return; }

	auto* rendertarget = entry->Data.load(std::memory_order_acquire);
	if (!rendertarget)
	{
		Pitaya::Log::Error(guid.ToString() + "rendertarget asset entry data is empty,path:" + path);
		return;
	}

	if (!rendertarget->DeserializeFromFile(file))
	{
		Pitaya::Log::Error("rendertarget deserialize fail, path:" + file.string());
		entry->State.ModifyBits(Pitaya::Core::AssetState::CPUFailed, Pitaya::Core::AssetState::CPULoading);
		return;
	}

	Pitaya::Log::Info("rendertarget CPU laod success, GUID:" + guid.ToString());
	Pitaya::Import::RenderTargetImportResult cpuOpResult_Inner;
	cpuOpResult_Inner.GUID = guid;
	cpuOpResult_Inner.SceneFrameBufferSpecification = rendertarget->SceneFrameBufferSpecification;
	cpuOpResult_Inner.PingPongFrameBufferSpecification = rendertarget->PingPongFrameBufferSpecification;
	cpuOpResult_Inner.FinalFrameBufferSpecification = rendertarget->FinalFrameBufferSpecification;
	Pitaya::Asset::AssetOperate cpuOpResult_Out;
	cpuOpResult_Out.Data = cpuOpResult_Inner;
	assetOperateQueue.Push(cpuOpResult_Out);

	entry->State.ModifyBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::CPULoading);
}

void Pitaya::Asset::AssetHub::AssetRegistry::Serialize(Pitaya::Serialize::SerializeContext&) const
{

}
void Pitaya::Asset::AssetHub::AssetRegistry::Deserialize(const Pitaya::Serialize::DeserializeContext&)
{

}
