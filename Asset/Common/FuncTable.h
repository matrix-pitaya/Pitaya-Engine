#pragma once

#include<Core/PassKey/PassKey.h>
#include<Core/Asset/Asset.h>
#include<Core/Identifier/GUID.h>
#include<Context/Context.h>
#include<stdexcept>
#include<string>
#include<filesystem>

namespace Pitaya::Asset
{
	struct Texture;
	struct Material;
	struct Mesh;
	struct Shader;
	struct RenderTarget;
}

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::Asset::AssetHub>
	{
		friend class Pitaya::Engine::Engine;

	private:
		FuncTable() = default;
		~FuncTable() = default;

	public:
		FuncTable(const FuncTable&) = delete;
		FuncTable& operator=(const FuncTable&) = delete;
		FuncTable(FuncTable&&) = delete;
		FuncTable& operator=(FuncTable&&) = delete;

	public:
		inline bool Check() const
		{
			if (!OnLoadTexture) { throw std::runtime_error("FuncTable miss [Asset::OnLoadTexture] Function!"); }
			if (!OnLoadShader) { throw std::runtime_error("FuncTable miss [Asset::OnLoadShader] Function!"); }
			if (!OnLoadMesh) { throw std::runtime_error("FuncTable miss [Asset::OnLoadMesh] Function!"); }
			if (!OnLoadMaterial) { throw std::runtime_error("FuncTable miss [Asset::OnLoadMaterial] Function!"); }
			if (!OnLoadRenderTarget) { throw std::runtime_error("FuncTable miss [Asset::OnLoadRenderTarget] Function!"); }
			if (!OnGetAssetPathByGUID) { throw std::runtime_error("FuncTable miss [Asset::GetAssetPathByGUID] Function!"); }
			if (!OnGetAssetGUIDByPath) { throw std::runtime_error("FuncTable miss [Asset::GetAssetGUIDByPath] Function!"); }
			if (!OnTransformToVirtualPath) { throw std::runtime_error("FuncTable miss [Asset::TransformToVirtualPath] Function!"); }
			if (!OnRegisterExternalFile) { throw std::runtime_error("FuncTable miss [Asset::RegisterExternalFile] Function!"); }
			if (!OnSyncAssetToGPU) { throw std::runtime_error("FuncTable miss [Asset::SyncAssetToGPU] Function!"); }
			if (!OnIsUploadedToGPU) { throw std::runtime_error("FuncTable miss [Asset::IsUploadedToGPU] Function!"); }
			return true;
		}
		inline void UnRegister() noexcept
		{
			OnLoadTexture = nullptr;
			OnLoadShader = nullptr;
			OnLoadMesh = nullptr;
			OnLoadMaterial = nullptr;
			OnLoadRenderTarget = nullptr;
			OnGetAssetPathByGUID = nullptr;
			OnGetAssetGUIDByPath = nullptr;
			OnTransformToVirtualPath = nullptr;
			OnRegisterExternalFile = nullptr;
			OnSyncAssetToGPU = nullptr;
			OnIsUploadedToGPU = nullptr;
		}

	public:
		inline Pitaya::Core::Asset<Pitaya::Asset::Texture> InvokeOnLoadTexture(Pitaya::Core::GUID guid)
		{
			return OnLoadTexture(guid);
		}
		inline Pitaya::Core::Asset<Pitaya::Asset::Shader> InvokeOnLoadShader(Pitaya::Core::GUID guid)
		{
			return OnLoadShader(guid);
		}
		inline Pitaya::Core::Asset<Pitaya::Asset::Mesh> InvokeOnLoadMesh(Pitaya::Core::GUID guid)
		{
			return OnLoadMesh(guid);
		}
		inline Pitaya::Core::Asset<Pitaya::Asset::Material> InvokeOnLoadMaterial(Pitaya::Core::GUID guid)
		{
			return OnLoadMaterial(guid);
		}
		inline Pitaya::Core::Asset<Pitaya::Asset::RenderTarget> InvokeOnLoadRenderTarget(Pitaya::Core::GUID guid)
		{
			return OnLoadRenderTarget(guid);
		}
		inline bool InvokeOnGetAssetPathByGUID(Pitaya::Core::GUID guid, std::filesystem::path& out_path)
		{
			return OnGetAssetPathByGUID(guid, out_path);
		}
		inline bool InvokeOnGetAssetGUIDByPath(const std::filesystem::path& path, Pitaya::Core::GUID& out_guid)
		{
			return OnGetAssetGUIDByPath(path, out_guid);
		}
		inline bool InvokeOnTransformToVirtualPath(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath)
		{
			return OnTransformToVirtualPath(inputPath, basePath, out_virtualpath);
		}
		inline bool InvokeOnRegisterExternalFile(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath, Pitaya::Core::GUID& out_guid)
		{
			return OnRegisterExternalFile(inputPath, basePath, out_virtualpath, out_guid);
		}
		inline void InvokeOnSyncAssetToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
		{
			OnSyncAssetToGPU(passkey);
		}
		inline bool InvokeOnIsUploadedToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
		{
			return OnIsUploadedToGPU(passkey);
		}

	private:
		Pitaya::Core::Asset<Pitaya::Asset::Texture>(ENGINE_CALL *OnLoadTexture)(Pitaya::Core::GUID) = nullptr;
		Pitaya::Core::Asset<Pitaya::Asset::Shader>(ENGINE_CALL *OnLoadShader)(Pitaya::Core::GUID) = nullptr;
		Pitaya::Core::Asset<Pitaya::Asset::Mesh>(ENGINE_CALL *OnLoadMesh)(Pitaya::Core::GUID) = nullptr;
		Pitaya::Core::Asset<Pitaya::Asset::Material>(ENGINE_CALL *OnLoadMaterial)(Pitaya::Core::GUID) = nullptr;
		Pitaya::Core::Asset<Pitaya::Asset::RenderTarget>(ENGINE_CALL *OnLoadRenderTarget)(Pitaya::Core::GUID) = nullptr;
		bool (ENGINE_CALL *OnGetAssetPathByGUID)(Pitaya::Core::GUID, std::filesystem::path&) = nullptr;
		bool (ENGINE_CALL *OnGetAssetGUIDByPath)(const std::filesystem::path&, Pitaya::Core::GUID&) = nullptr;
		bool (ENGINE_CALL *OnTransformToVirtualPath)(const std::filesystem::path&, const std::filesystem::path&, std::filesystem::path&) = nullptr;
		bool (ENGINE_CALL *OnRegisterExternalFile)(const std::filesystem::path&, const std::filesystem::path&, std::filesystem::path&, Pitaya::Core::GUID&) = nullptr;
		void (ENGINE_CALL *OnSyncAssetToGPU)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;
		bool (ENGINE_CALL *OnIsUploadedToGPU)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;
	};
}

namespace Pitaya::Asset
{
	template<typename T>
	inline Pitaya::Core::Asset<T> LoadAsset(Pitaya::Core::GUID guid)
	{
		static_assert(std::is_same_v<T, Pitaya::Asset::Texture> ||
			std::is_same_v<T, Pitaya::Asset::Shader> ||
			std::is_same_v<T, Pitaya::Asset::Mesh> ||
			std::is_same_v<T, Pitaya::Asset::RenderTarget> ||
			std::is_same_v<T, Pitaya::Asset::Material>,
			"Unknow Asset!");

		if constexpr (std::is_same_v<T, Pitaya::Asset::Texture>) { return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnLoadTexture(guid); }
		if constexpr (std::is_same_v<T, Pitaya::Asset::Shader>) { return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnLoadShader(guid); }
		if constexpr (std::is_same_v<T, Pitaya::Asset::Mesh>) { return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnLoadMesh(guid); }
		if constexpr (std::is_same_v<T, Pitaya::Asset::Material>) { return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnLoadMaterial(guid); }
		if constexpr (std::is_same_v<T, Pitaya::Asset::RenderTarget>) { return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnLoadRenderTarget(guid); }
	}
	inline bool GetAssetPathByGUID(Pitaya::Core::GUID guid, std::filesystem::path& out_path)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnGetAssetPathByGUID(guid, out_path);
	}
	inline bool GetAssetGUIDByPath(const std::filesystem::path& path, Pitaya::Core::GUID& out_guid)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnGetAssetGUIDByPath(path, out_guid);
	}
	inline bool TransformToVirtualPath(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnTransformToVirtualPath(inputPath, basePath, out_virtualpath);
	}
	inline bool RegisterExternalFile(const std::filesystem::path& inputPath, const std::filesystem::path& basePath, std::filesystem::path& out_virtualpath, Pitaya::Core::GUID& out_guid)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnRegisterExternalFile(inputPath, basePath, out_virtualpath, out_guid);
	}
	inline void SyncAssetToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnSyncAssetToGPU(passkey);
	}
	inline bool IsUploadedToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::Asset::AssetHub>().InvokeOnIsUploadedToGPU(passkey);
	}
}