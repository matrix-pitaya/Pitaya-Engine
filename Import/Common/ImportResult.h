#pragma once

#include<Core/Math/AABB.h>
#include<Core/Identifier/GUID.h>
#include<Core/StateFlags/StateFlags.h>

#include<GPU/Common/Identifier.h>
#include<GPU/Common/MeshType.h>
#include<GPU/Layout/BufferLayout.h>
#include<GPU/Layout/BufferElement.h>
#include<GPU/Frontend/Buffer/FrameBuffer.h>
#include<Asset/Common/Mesh.h>
#include<Asset/Common/TextureUsage.h>

#include<glm.hpp>
#include<variant>

namespace Pitaya::Import
{
	struct Texture2DImportResult
	{
		int Width = 0;
		int Height = 0;
		int Channels = 0;
		bool IsGenerateMipmap = true;
		bool isNearest = false;
		Pitaya::Asset::TextureUsage Usage = Pitaya::Asset::TextureUsage::Color;
		std::vector<uint8_t> Data; //STB加载的像素数据
		Pitaya::Core::GUID GUID;
	};
	struct TextureCubemapImportResult
	{
		int Width[6] = {};
		int Height[6] = {};
		int Channels[6] = {};
		bool IsGenerateMipmap = true;
		bool isNearest = false;
		Pitaya::Asset::TextureUsage Usage = Pitaya::Asset::TextureUsage::Color;
		std::vector<uint8_t> Data[6] = {}; //STB加载的像素数据
		Pitaya::Core::GUID GUID;
	};

	struct ShaderImportResult
	{
		std::string VertexSource;
		std::string FragmentSource;
		std::string GeometrySource;
		Pitaya::Core::StateFlags<Pitaya::GPU::ShaderType> Type = GPU::ShaderType::Empty;
		Pitaya::Core::GUID GUID;
	};

	struct MeshPreloadResult
	{
		struct MeshNodeInfo
		{
			uint32_t MeshIndex;
			glm::mat4 GlobalTransform;
		};
		struct SubMeshPreloadResult
		{
			uint32_t VertexCount = 0;
			uint32_t IndexCount = 0;
			uint32_t MaterialIndex = 0;

			bool HasNormals = false;
			bool HasUVs = false;
			bool HasTangents = false;
			bool HasBones = false;

			Pitaya::Core::AABB BoundingBox;
		};
		
		bool IsValid = false;
		bool HasBones = false; 

		Pitaya::GPU::MeshType Type = Pitaya::GPU::MeshType::Unknwon;

		uint32_t SubMeshCount = 0;
		std::vector<SubMeshPreloadResult> SubMeshPreloadResults;
		std::vector<Pitaya::Core::GUID> MaterialGUIDs;
		std::vector<MeshNodeInfo> MeshNodeInfos;

		uint32_t TotalVertexCount = 0;
		uint32_t TotalIndexCount = 0;
	};
	struct StaticMeshImportResult
	{
		Pitaya::Core::GUID GUID;
		std::vector<float> Vertices;							//顶点数据 VertexBuffer
		std::vector<uint32_t> Indices;							//索引数据 IndexBuffer
		Pitaya::GPU::BufferLayout VertexLayout;					//顶点布局 VertexBuffer::SetLayout
		std::vector<Pitaya::Asset::Mesh::SubMesh> SubMeshs;
		std::vector<Pitaya::Core::GUID> MaterialGUIDs;
		Pitaya::Core::AABB BoundingBox;
		bool IsValid = false;
	};
	struct SkinnedMeshImportResult
	{
		Pitaya::Core::GUID GUID;
		std::vector<float> Vertices;
		std::vector<uint32_t> Indices;
		Pitaya::GPU::BufferLayout VertexLayout;
		std::vector<Pitaya::Asset::Mesh::SubMesh> SubMeshs;
		std::vector<Pitaya::Core::GUID> MaterialGUIDs;
		Pitaya::Core::AABB BoundingBox;
		uint32_t BoneCount = 0;						
		std::vector<glm::mat4> BoneInverseMatrices;	
		bool IsValid = false;
	};

	struct RenderTargetImportResult
	{
		Pitaya::Core::GUID GUID;
	};
}
