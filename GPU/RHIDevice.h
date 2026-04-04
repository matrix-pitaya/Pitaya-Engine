#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/PassKey/PassKey.h>
#include<Core/Camera/CameraSnapshot.h>
#include<GPU/Common/FuncTable.h>
#include<GPU/Common/BindPoint.h>
#include<GPU/Frontend/Texture/Texture2D.h>
#include<GPU/Frontend/Texture/TextureCubemap.h>
#include<GPU/Frontend/Shader/Shader.h>
#include<GPU/Frontend/Buffer/FrameBuffer.h>
#include<GPU/Frontend/Buffer/IndexBuffer.h>
#include<GPU/Frontend/Buffer/ShaderStorageBuffer.h>
#include<GPU/Frontend/Buffer/UniformBuffer.h>
#include<GPU/Frontend/Buffer/VertexArray.h>
#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Render/Common/PostProcessStep.h>

#include<unordered_map>

namespace Pitaya::GPU
{
	class RHIDevice
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<RHIDevice>;
		private:
			static RHIDevice* Create()
			{
				return PITAYA_NEW(RHIDevice);
			}
			static void Destroy(RHIDevice* rhiDevice)
			{
				PITAYA_DELETE(rhiDevice);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<RHIDevice>;
		private:
			static bool Initialize(RHIDevice* rhiDevice)
			{
				return rhiDevice->Initialize();
			}
			static void Release(RHIDevice* rhiDevice)
			{
				rhiDevice->Release();
			}
		};

	private:
		struct RHIRegistry
		{
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray>, Pitaya::GPU::VertexArray*> VertexArrays;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer>, Pitaya::GPU::VertexBuffer*> VertexBuffers;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer>, Pitaya::GPU::IndexBuffer*> IndexBuffers;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::Shader>, Pitaya::GPU::Shader*> Shaders;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D>, Pitaya::GPU::Texture2D*> Texture2Ds;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap>, Pitaya::GPU::TextureCubemap*> TextureCubemaps;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer>, Pitaya::GPU::UniformBuffer*> UniformBuffers;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer>, Pitaya::GPU::FrameBuffer*> FrameBuffers;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer>, Pitaya::GPU::ShaderStorageBuffer*> ShaderStorageBuffers;
		};

	private:
		RHIDevice() = default;
		~RHIDevice() = default;

	public:
		RHIDevice(const RHIDevice&) = delete;
		RHIDevice& operator=(const RHIDevice&) = delete;
		RHIDevice(RHIDevice&&) = delete;
		RHIDevice& operator=(RHIDevice&&) = delete;

	private:
		bool Initialize();
		void Release();
		
	public:
		inline void DestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer>)
		{
			for (auto& [key, value] : registry.VertexArrays)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			for (auto& [key, value] : registry.VertexBuffers)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			for (auto& [key, value] : registry.IndexBuffers)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			for (auto& [key, value] : registry.Shaders)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			for (auto& [key, value] : registry.Texture2Ds)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			for (auto& [key, value] : registry.TextureCubemaps)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			for (auto& [key, value] : registry.UniformBuffers)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			for (auto& [key, value] : registry.FrameBuffers)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			for (auto& [key, value] : registry.ShaderStorageBuffers)
			{
				if (!value) { continue; }
				delete value;
				value = nullptr;
			}
			registry.VertexArrays.clear();
			registry.VertexBuffers.clear();
			registry.IndexBuffers.clear();
			registry.Shaders.clear();
			registry.Texture2Ds.clear();
			registry.TextureCubemaps.clear();
			registry.UniformBuffers.clear();
			registry.FrameBuffers.clear();
			registry.ShaderStorageBuffers.clear();
		}
		 
	public:				
		inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> CreateVertexArray()
		{
			Pitaya::GPU::VertexArray* vertexArray = Pitaya::GPU::VertexArray::Create();
			Pitaya::GPU::Identifier gpuIdentifier = vertexArray->GetGPUIdentifier();
			registry.VertexArrays.emplace(gpuIdentifier, vertexArray);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> CreateVertexBuffer(float* vertices, uint32_t size)
		{
			Pitaya::GPU::VertexBuffer* vertexBuffer = Pitaya::GPU::VertexBuffer::Create(vertices, size);
			Pitaya::GPU::Identifier gpuIdentifier = vertexBuffer->GetGPUIdentifier();
			registry.VertexBuffers.emplace(gpuIdentifier, vertexBuffer);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> CreateIndexBuffer(uint32_t* indices, uint32_t count)
		{
			Pitaya::GPU::IndexBuffer* indexBuffer = Pitaya::GPU::IndexBuffer::Create(indices, count);
			Pitaya::GPU::Identifier gpuIdentifier = indexBuffer->GetGPUIdentifier();
			registry.IndexBuffers.emplace(gpuIdentifier, indexBuffer);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> CreateShader(const char* vertexSource, const char* fragmentSource)
		{
			try
			{
				Pitaya::GPU::Shader* shader = Pitaya::GPU::Shader::Create(vertexSource, fragmentSource);
				Pitaya::GPU::Identifier gpuIdentifier = shader->GetGPUIdentifier();
				registry.Shaders.emplace(gpuIdentifier, shader);
				return gpuIdentifier;
			}
			catch (const std::exception& e)
			{
				Pitaya::Log::Error("create shader error:" + std::string(e.what()));
				return Pitaya::GPU::Identifier<Pitaya::GPU::Shader>::Invalid;
			}
			catch (...)
			{
				Pitaya::Log::Error("create shader unknown error");
				return Pitaya::GPU::Identifier<Pitaya::GPU::Shader>::Invalid;
			}
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> CreateShader(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
		{
			try
			{
				Pitaya::GPU::Shader* shader = Pitaya::GPU::Shader::Create(vertexSource, fragmentSource, geometrySource);
				Pitaya::GPU::Identifier gpuIdentifier = shader->GetGPUIdentifier();
				registry.Shaders.emplace(gpuIdentifier, shader);
				return gpuIdentifier;
			}
			catch (const std::exception& e)
			{
				Pitaya::Log::Error("create shader error:" + std::string(e.what()));
				return Pitaya::GPU::Identifier<Pitaya::GPU::Shader>::Invalid;
			}
			catch (...)
			{
				Pitaya::Log::Error("create shader unknown error");
				return Pitaya::GPU::Identifier<Pitaya::GPU::Shader>::Invalid;
			}
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> CreateTexture2D(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
		{
			Pitaya::GPU::Texture2D* texture2D = Pitaya::GPU::Texture2D::Create(data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
			Pitaya::GPU::Identifier gpuIdentifier = texture2D->GetGPUIdentifier();
			registry.Texture2Ds.emplace(gpuIdentifier, texture2D);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> CreateTextureCubemap(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
		{
			Pitaya::GPU::TextureCubemap* textureCubemap = Pitaya::GPU::TextureCubemap::Create(datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
			Pitaya::GPU::Identifier gpuIdentifier = textureCubemap->GetGPUIdentifier();
			registry.TextureCubemaps.emplace(gpuIdentifier, textureCubemap);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> CreateUniformBuffer(uint32_t size, uint32_t bindingPoint)
		{
			Pitaya::GPU::UniformBuffer* uniformBuffer = Pitaya::GPU::UniformBuffer::Create(size, bindingPoint);
			Pitaya::GPU::Identifier gpuIdentifier = uniformBuffer->GetGPUIdentifier();
			registry.UniformBuffers.emplace(gpuIdentifier, uniformBuffer);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> CreateFrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec)
		{
			try
			{
				Pitaya::GPU::FrameBuffer* frameBuffer = Pitaya::GPU::FrameBuffer::Create(spec);
				Pitaya::GPU::Identifier gpuIdentifier = frameBuffer->GetGPUIdentifier();
				registry.FrameBuffers.emplace(gpuIdentifier, frameBuffer);
				return gpuIdentifier;
			}
			catch (const std::exception& e)
			{
				Pitaya::Log::Error("create frame buffer fail, error:" + std::string(e.what()));
				return Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer>::Invalid;
			}
			catch (...)
			{
				Pitaya::Log::Error("create frame buffer fail, unknwon error");
				return Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer>::Invalid;
			}
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> CreateShaderStorageBuffer(uint32_t size, uint32_t bindingPoint)
		{
			Pitaya::GPU::ShaderStorageBuffer* shaderStorageBuffer = Pitaya::GPU::ShaderStorageBuffer::Create(size, bindingPoint);
			Pitaya::GPU::Identifier gpuIdentifier = shaderStorageBuffer->GetGPUIdentifier();
			registry.ShaderStorageBuffers.emplace(gpuIdentifier, shaderStorageBuffer);
			return gpuIdentifier;
		}

		inline bool DestroyVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
		{
			auto iterator = registry.VertexArrays.find(id);
			if (iterator == registry.VertexArrays.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU VertexArray Empty Destroy!");
				registry.VertexArrays.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.VertexArrays.erase(iterator);
			return true;
		}
		inline bool DestroyVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
		{
			auto iterator = registry.VertexBuffers.find(id);
			if (iterator == registry.VertexBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU VertexBuffer Empty Destroy!");
				registry.VertexBuffers.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.VertexBuffers.erase(iterator);
			return true;
		}
		inline bool DestroyIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
		{
			auto iterator = registry.IndexBuffers.find(id);
			if (iterator == registry.IndexBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU IndexBuffer Empty Destroy!");
				registry.IndexBuffers.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.IndexBuffers.erase(iterator);
			return true;
		}
		inline bool DestroyShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
		{
			auto iterator = registry.Shaders.find(id);
			if (iterator == registry.Shaders.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU Shader Empty Destroy!");
				registry.Shaders.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.Shaders.erase(iterator);
			return true;
		}
		inline bool DestroyTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
		{
			auto iterator = registry.Texture2Ds.find(id);
			if (iterator == registry.Texture2Ds.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU Texture2D Empty Destroy!");
				registry.Texture2Ds.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.Texture2Ds.erase(iterator);
			return true;
		}
		inline bool DestroyTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
		{
			auto iterator = registry.TextureCubemaps.find(id);
			if (iterator == registry.TextureCubemaps.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU TextureCubemap Empty Destroy!");
				registry.TextureCubemaps.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.TextureCubemaps.erase(iterator);
			return true;
		}
		inline bool DestroyUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
		{
			auto iterator = registry.UniformBuffers.find(id);
			if (iterator == registry.UniformBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU UniformBuffer Empty Destroy!");
				registry.UniformBuffers.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.UniformBuffers.erase(iterator);
			return true;
		}
		inline bool DestroyFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
		{
			auto iterator = registry.FrameBuffers.find(id);
			if (iterator == registry.FrameBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU Shader Empty Destroy!");
				registry.FrameBuffers.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.FrameBuffers.erase(iterator);
			return true;
		}
		inline bool DestroyShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
		{
			auto iterator = registry.ShaderStorageBuffers.find(id);
			if (iterator == registry.ShaderStorageBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU Shader Empty Destroy!");
				registry.ShaderStorageBuffers.erase(iterator);
				return false;
			}

			delete iterator->second;
			iterator->second = nullptr;
			registry.ShaderStorageBuffers.erase(iterator);
			return true;
		}

		inline Pitaya::GPU::VertexArray* GetVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
		{
			auto iterator = registry.VertexArrays.find(id);
			return iterator != registry.VertexArrays.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::VertexBuffer* GetVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
		{
			auto iterator = registry.VertexBuffers.find(id);
			return iterator != registry.VertexBuffers.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::IndexBuffer* GetIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
		{
			auto iterator = registry.IndexBuffers.find(id);
			return iterator != registry.IndexBuffers.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::Shader* GetShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
		{
			auto iterator = registry.Shaders.find(id);
			return iterator != registry.Shaders.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::Texture2D* GetTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
		{
			auto iterator = registry.Texture2Ds.find(id);
			return iterator != registry.Texture2Ds.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::TextureCubemap* GetTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
		{
			auto iterator = registry.TextureCubemaps.find(id);
			return iterator != registry.TextureCubemaps.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::UniformBuffer* GetUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
		{
			auto iterator = registry.UniformBuffers.find(id);
			return iterator != registry.UniformBuffers.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::FrameBuffer* GetFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
		{
			auto iterator = registry.FrameBuffers.find(id);
			return iterator != registry.FrameBuffers.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::ShaderStorageBuffer* GetShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
		{
			auto iterator = registry.ShaderStorageBuffers.find(id);
			return iterator != registry.ShaderStorageBuffers.end() ? iterator->second : nullptr;
		}

	private:
		RHIRegistry registry;
	};
}
