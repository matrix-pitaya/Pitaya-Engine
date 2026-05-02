#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/PassKey/PassKey.h>
#include<Core/Camera/CameraSnapshot.h>
#include<GPU/Common/FuncTable.h>
#include<GPU/Common/BindPoint.h>
#include<GPU/Frontend/Texture/Texture2D.h>
#include<GPU/Frontend/Texture/TextureCubemap.h>
#include<GPU/Frontend/Texture/Texture2DArray.h>
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

namespace Pitaya::Render { class Renderer; }

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
		public:
			inline void DestroyAll()
			{
				for (auto& [key, value] : VertexArrays)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : VertexBuffers)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : IndexBuffers)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : Shaders)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : Texture2Ds)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : TextureCubemaps)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : Texture2DArrays)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : UniformBuffers)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : FrameBuffers)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				for (auto& [key, value] : ShaderStorageBuffers)
				{
					if (value)
					{
						PITAYA_DELETE(value);
						value = nullptr;
					}
				}
				VertexArrays.clear();
				VertexBuffers.clear();
				IndexBuffers.clear();
				Shaders.clear();
				Texture2Ds.clear();
				TextureCubemaps.clear();
				Texture2DArrays.clear();
				UniformBuffers.clear();
				FrameBuffers.clear();
				ShaderStorageBuffers.clear();
			}
			inline auto Create(int width, int height, int layers, bool isDepth)
			{
				Pitaya::GPU::Texture2DArray* texture2DArray = Pitaya::GPU::Texture2DArray::Create(width, height, layers, isDepth);
				Pitaya::GPU::Identifier gpuIdentifier = texture2DArray->GetGPUIdentifier();
				Texture2DArrays.emplace(gpuIdentifier, texture2DArray);
				return gpuIdentifier;
			}
			inline bool Destroy(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2DArray> id)
			{
				auto iterator = Texture2DArrays.find(id);
				if (iterator == Texture2DArrays.end()) { return false; }

				if (!iterator->second)
				{
					Pitaya::Log::Warning("GPU TextureCubemap Empty Destroy!");
					Texture2DArrays.erase(iterator);
					return false;
				}

				PITAYA_DELETE(iterator->second);
				iterator->second = nullptr;
				Texture2DArrays.erase(iterator);
				return true;
			}
			inline auto Get(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2DArray> id)
			{
				auto iterator = Texture2DArrays.find(id);
				return iterator != Texture2DArrays.end() ? iterator->second : nullptr;
			}

		public:
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray>, Pitaya::GPU::VertexArray*> VertexArrays;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer>, Pitaya::GPU::VertexBuffer*> VertexBuffers;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer>, Pitaya::GPU::IndexBuffer*> IndexBuffers;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::Shader>, Pitaya::GPU::Shader*> Shaders;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D>, Pitaya::GPU::Texture2D*> Texture2Ds;
			std::unordered_map<Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap>, Pitaya::GPU::TextureCubemap*> TextureCubemaps;
			std::unordered_map< Pitaya::GPU::Identifier<Pitaya::GPU::Texture2DArray>, Pitaya::GPU::Texture2DArray*> Texture2DArrays;
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
			registry.DestroyAll();
		}
		 
	public:				
		inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
		{
			Pitaya::GPU::VertexArray* vertexArray = Pitaya::GPU::VertexArray::Create(passkey);
			Pitaya::GPU::Identifier gpuIdentifier = vertexArray->GetGPUIdentifier();
			registry.VertexArrays.emplace(gpuIdentifier, vertexArray);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> CreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, float* vertices, uint32_t size)
		{
			Pitaya::GPU::VertexBuffer* vertexBuffer = Pitaya::GPU::VertexBuffer::Create(vertices, size);
			Pitaya::GPU::Identifier gpuIdentifier = vertexBuffer->GetGPUIdentifier();
			registry.VertexBuffers.emplace(gpuIdentifier, vertexBuffer);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> CreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t* indices, uint32_t count)
		{
			Pitaya::GPU::IndexBuffer* indexBuffer = Pitaya::GPU::IndexBuffer::Create(indices, count);
			Pitaya::GPU::Identifier gpuIdentifier = indexBuffer->GetGPUIdentifier();
			registry.IndexBuffers.emplace(gpuIdentifier, indexBuffer);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const char* vertexSource, const char* fragmentSource)
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
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
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
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> CreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
		{
			Pitaya::GPU::Texture2D* texture2D = Pitaya::GPU::Texture2D::Create(data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
			Pitaya::GPU::Identifier gpuIdentifier = texture2D->GetGPUIdentifier();
			registry.Texture2Ds.emplace(gpuIdentifier, texture2D);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> CreateTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
		{
			Pitaya::GPU::TextureCubemap* textureCubemap = Pitaya::GPU::TextureCubemap::Create(datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
			Pitaya::GPU::Identifier gpuIdentifier = textureCubemap->GetGPUIdentifier();
			registry.TextureCubemaps.emplace(gpuIdentifier, textureCubemap);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Texture2DArray> CreateTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, int width, int height, int layers, bool isDepth)
		{
			return registry.Create(width, height, layers, isDepth);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t size, uint32_t bindingPoint)
		{
			Pitaya::GPU::UniformBuffer* uniformBuffer = Pitaya::GPU::UniformBuffer::Create(size, bindingPoint);
			Pitaya::GPU::Identifier gpuIdentifier = uniformBuffer->GetGPUIdentifier();
			registry.UniformBuffers.emplace(gpuIdentifier, uniformBuffer);
			return gpuIdentifier;
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const Pitaya::GPU::FrameBufferSpecification& spec)
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
		inline Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t size, uint32_t bindingPoint)
		{
			Pitaya::GPU::ShaderStorageBuffer* shaderStorageBuffer = Pitaya::GPU::ShaderStorageBuffer::Create(size, bindingPoint);
			Pitaya::GPU::Identifier gpuIdentifier = shaderStorageBuffer->GetGPUIdentifier();
			registry.ShaderStorageBuffers.emplace(gpuIdentifier, shaderStorageBuffer);
			return gpuIdentifier;
		}

		inline bool DestroyVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
		{
			auto iterator = registry.VertexArrays.find(id);
			if (iterator == registry.VertexArrays.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU VertexArray Empty Destroy!");
				registry.VertexArrays.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.VertexArrays.erase(iterator);
			return true;
		}
		inline bool DestroyVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
		{
			auto iterator = registry.VertexBuffers.find(id);
			if (iterator == registry.VertexBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU VertexBuffer Empty Destroy!");
				registry.VertexBuffers.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.VertexBuffers.erase(iterator);
			return true;
		}
		inline bool DestroyIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
		{
			auto iterator = registry.IndexBuffers.find(id);
			if (iterator == registry.IndexBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU IndexBuffer Empty Destroy!");
				registry.IndexBuffers.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.IndexBuffers.erase(iterator);
			return true;
		}
		inline bool DestroyShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
		{
			auto iterator = registry.Shaders.find(id);
			if (iterator == registry.Shaders.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU Shader Empty Destroy!");
				registry.Shaders.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.Shaders.erase(iterator);
			return true;
		}
		inline bool DestroyTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
		{
			auto iterator = registry.Texture2Ds.find(id);
			if (iterator == registry.Texture2Ds.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU Texture2D Empty Destroy!");
				registry.Texture2Ds.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.Texture2Ds.erase(iterator);
			return true;
		}
		inline bool DestroyTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
		{
			auto iterator = registry.TextureCubemaps.find(id);
			if (iterator == registry.TextureCubemaps.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU TextureCubemap Empty Destroy!");
				registry.TextureCubemaps.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.TextureCubemaps.erase(iterator);
			return true;
		}
		inline bool DestroyTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2DArray> id)
		{
			return registry.Destroy(id);
		}
		inline bool DestroyUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
		{
			auto iterator = registry.UniformBuffers.find(id);
			if (iterator == registry.UniformBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU UniformBuffer Empty Destroy!");
				registry.UniformBuffers.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.UniformBuffers.erase(iterator);
			return true;
		}
		inline bool DestroyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
		{
			auto iterator = registry.FrameBuffers.find(id);
			if (iterator == registry.FrameBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU Shader Empty Destroy!");
				registry.FrameBuffers.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.FrameBuffers.erase(iterator);
			return true;
		}
		inline bool DestroyShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
		{
			auto iterator = registry.ShaderStorageBuffers.find(id);
			if (iterator == registry.ShaderStorageBuffers.end()) { return false; }

			if (!iterator->second)
			{
				Pitaya::Log::Warning("GPU Shader Empty Destroy!");
				registry.ShaderStorageBuffers.erase(iterator);
				return false;
			}

			PITAYA_DELETE(iterator->second);
			iterator->second = nullptr;
			registry.ShaderStorageBuffers.erase(iterator);
			return true;
		}

		inline Pitaya::GPU::VertexArray* GetVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
		{
			auto iterator = registry.VertexArrays.find(id);
			return iterator != registry.VertexArrays.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::VertexBuffer* GetVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
		{
			auto iterator = registry.VertexBuffers.find(id);
			return iterator != registry.VertexBuffers.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::IndexBuffer* GetIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
		{
			auto iterator = registry.IndexBuffers.find(id);
			return iterator != registry.IndexBuffers.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::Shader* GetShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
		{
			auto iterator = registry.Shaders.find(id);
			return iterator != registry.Shaders.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::Texture2D* GetTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
		{
			auto iterator = registry.Texture2Ds.find(id);
			return iterator != registry.Texture2Ds.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::TextureCubemap* GetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
		{
			auto iterator = registry.TextureCubemaps.find(id);
			return iterator != registry.TextureCubemaps.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::Texture2DArray* GetTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2DArray> id)
		{
			return registry.Get(id);
		}
		inline Pitaya::GPU::UniformBuffer* GetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
		{
			auto iterator = registry.UniformBuffers.find(id);
			return iterator != registry.UniformBuffers.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::FrameBuffer* GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
		{
			auto iterator = registry.FrameBuffers.find(id);
			return iterator != registry.FrameBuffers.end() ? iterator->second : nullptr;
		}
		inline Pitaya::GPU::ShaderStorageBuffer* GetShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
		{
			auto iterator = registry.ShaderStorageBuffers.find(id);
			return iterator != registry.ShaderStorageBuffers.end() ? iterator->second : nullptr;
		}

	private:
		RHIRegistry registry;
	};
}
