#pragma once

#include<Core/PassKey/PassKey.h>
#include<Context/Context.h>
#include<GPU/Common/Identifier.h>
#include<GPU/Common/FrameBufferSpecification.h>
#include<stdexcept>

namespace Pitaya::Engine
{
	template<>
	struct FuncTable<Pitaya::GPU::RHIDevice>
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
			if (!OnDestroyAllGPUResource) { throw std::runtime_error("FuncTable miss [GPU::DestroyAllGPUResource] Function!"); }

			if (!OnCreateVertexArray) { throw std::runtime_error("FuncTable miss [GPU::CreateVertexArray] Function!"); }
			if (!OnCreateVertexBuffer) { throw std::runtime_error("FuncTable miss [GPU::CreateVertexBuffer] Function!"); }
			if (!OnCreateIndexBuffer) { throw std::runtime_error("FuncTable miss [GPU::CreateIndexBuffer] Function!"); }
			if (!OnCreateShaderVF) { throw std::runtime_error("FuncTable miss [GPU::OnCreateShaderVF] Function!"); }
			if (!OnCreateShaderVFG) { throw std::runtime_error("FuncTable miss [GPU::OnCreateShaderVFG] Function!"); }
			if (!OnCreateTexture2D) { throw std::runtime_error("FuncTable miss [GPU::OnCreateTexture2D] Function!"); }
			if (!OnCreateTextureCubemap) { throw std::runtime_error("FuncTable miss [GPU::OnCreateTextureCubemap] Function!"); }
			if (!OnCreateUniformBuffer) { throw std::runtime_error("FuncTable miss [GPU::OnCreateUniformBuffer] Function!"); }
			if (!OnCreateFrameBuffer) { throw std::runtime_error("FuncTable miss [GPU::OnCreateFrameBuffer] Function!"); }
			if (!OnCreateShaderStorageBuffer) { throw std::runtime_error("FuncTable miss [GPU::OnCreateShaderStorageBuffer] Function!"); }

			if (!OnDestroyVertexArray) { throw std::runtime_error("FuncTable miss [GPU::DestroyVertexArray] Function!"); }
			if (!OnDestroyVertexBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyVertexBuffer] Function!"); }
			if (!OnDestroyIndexBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyIndexBuffer] Function!"); }
			if (!OnDestroyShader) { throw std::runtime_error("FuncTable miss [GPU::DestroyShader] Function!"); }
			if (!OnDestroyTexture2D) { throw std::runtime_error("FuncTable miss [GPU::DestroyTexture2D] Function!"); }
			if (!OnDestroyTextureCubemap) { throw std::runtime_error("FuncTable miss [GPU::DestroyTextureCubemap] Function!"); }
			if (!OnDestroyUniformBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyUniformBuffer] Function!"); }
			if (!OnDestroyFrameBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyFrameBuffer] Function!"); }
			if (!OnDestroyShaderStorageBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyShaderStorageBuffer] Function!"); }

			if (!OnGetVertexArray) { throw std::runtime_error("FuncTable miss [GPU::GetVertexArray] Function!"); }
			if (!OnGetVertexBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetVertexBuffer] Function!"); }
			if (!OnGetIndexBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetIndexBuffer] Function!"); }
			if (!OnGetShader) { throw std::runtime_error("FuncTable miss [GPU::GetShader] Function!"); }
			if (!OnGetTexture2D) { throw std::runtime_error("FuncTable miss [GPU::GetTexture2D] Function!"); }
			if (!OnGetTextureCubemap) { throw std::runtime_error("FuncTable miss [GPU::GetTextureCubemap] Function!"); }
			if (!OnGetUniformBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetUniformBuffer] Function!"); }
			if (!OnGetFrameBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetFrameBuffer] Function!"); }
			if (!OnGetShaderStorageBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetShaderStorageBuffer] Function!"); }

			return true;
		}
		inline void UnRegister() noexcept
		{
			OnDestroyAllGPUResource = nullptr;

			OnCreateVertexArray = nullptr;
			OnCreateVertexBuffer = nullptr;
			OnCreateIndexBuffer = nullptr;
			OnCreateShaderVF = nullptr;
			OnCreateShaderVFG = nullptr;
			OnCreateTexture2D = nullptr;
			OnCreateTextureCubemap = nullptr;
			OnCreateUniformBuffer = nullptr;
			OnCreateFrameBuffer = nullptr;
			OnCreateShaderStorageBuffer = nullptr;

			OnDestroyVertexArray = nullptr;
			OnDestroyVertexBuffer= nullptr;
			OnDestroyIndexBuffer = nullptr;
			OnDestroyShader = nullptr;
			OnDestroyTexture2D = nullptr;
			OnDestroyTextureCubemap = nullptr;
			OnDestroyUniformBuffer = nullptr;
			OnDestroyFrameBuffer = nullptr;
			OnDestroyShaderStorageBuffer = nullptr;

			OnGetVertexArray= nullptr;
			OnGetVertexBuffer = nullptr;
			OnGetIndexBuffer= nullptr;
			OnGetShader = nullptr;
			OnGetTexture2D = nullptr;
			OnGetTextureCubemap = nullptr;
			OnGetUniformBuffer = nullptr;
			OnGetFrameBuffer = nullptr;
			OnGetShaderStorageBuffer = nullptr;
		}

	public:
		inline void InvokeOnDestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
		{
			OnDestroyAllGPUResource(passkey);
		}

		inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> InvokeOnCreateVertexArray() const
		{
			return OnCreateVertexArray();
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> InvokeOnCreateVertexBuffer(float* vertices, uint32_t size) const
		{
			return OnCreateVertexBuffer(vertices, size);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> InvokeOnCreateIndexBuffer(uint32_t* indices, uint32_t count) const
		{
			return OnCreateIndexBuffer(indices, count);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> InvokeOnCreateShaderVF(const char* vertexSource, const char* fragmentSource) const
		{
			return OnCreateShaderVF(vertexSource, fragmentSource);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> InvokeOnCreateShaderVFG(const char* vertexSource, const char* fragmentSource, const char* geometrySource) const
		{
			return OnCreateShaderVFG(vertexSource, fragmentSource, geometrySource);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> InvokeOnCreateTexture2D(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest) const
		{
			return OnCreateTexture2D(data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> InvokeOnCreateTextureCubemap(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest) const
		{
			return OnCreateTextureCubemap(datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> InvokeOnCreateUniformBuffer(uint32_t size, uint32_t bindingPoint) const
		{
			return OnCreateUniformBuffer(size, bindingPoint);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> InvokeOnCreateFrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec) const
		{
			return OnCreateFrameBuffer(spec);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> InvokeOnCreateShaderStorageBuffer(uint32_t size, uint32_t bindingPoint) const
		{
			return OnCreateShaderStorageBuffer(size, bindingPoint);
		}

		inline bool InvokeOnDestroyVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id) const
		{
			return OnDestroyVertexArray(id);
		}
		inline bool InvokeOnDestroyVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id) const
		{
			return OnDestroyVertexBuffer(id);
		}
		inline bool InvokeOnDestroyIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id) const
		{
			return OnDestroyIndexBuffer(id);
		}
		inline bool InvokeOnDestroyShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id) const
		{
			return OnDestroyShader(id);
		}
		inline bool InvokeOnDestroyTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id) const
		{
			return OnDestroyTexture2D(id);
		}
		inline bool InvokeOnDestroyTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id) const
		{
			return OnDestroyTextureCubemap(id);
		}
		inline bool InvokeOnDestroyUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id) const
		{
			return OnDestroyUniformBuffer(id);
		}
		inline bool InvokeOnDestroyFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id) const
		{
			return OnDestroyFrameBuffer(id);
		}
		inline bool InvokeOnDestroyShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id) const
		{
			return OnDestroyShaderStorageBuffer(id);
		}

		inline Pitaya::GPU::VertexArray* InvokeOnGetVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id) const
		{
			return OnGetVertexArray(id);
		}
		inline Pitaya::GPU::VertexBuffer* InvokeOnGetVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id) const
		{
			return OnGetVertexBuffer(id);
		}
		inline Pitaya::GPU::IndexBuffer* InvokeOnGetIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id) const
		{
			return OnGetIndexBuffer(id);
		}
		inline Pitaya::GPU::Shader* InvokeOnGetShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id) const
		{
			return OnGetShader(id);
		}
		inline Pitaya::GPU::Texture2D* InvokeOnGetTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id) const
		{
			return OnGetTexture2D(id);
		}
		inline Pitaya::GPU::TextureCubemap* InvokeOnGetTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id) const
		{
			return OnGetTextureCubemap(id);
		}
		inline Pitaya::GPU::UniformBuffer* InvokeOnGetUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id) const
		{
			return OnGetUniformBuffer(id);
		}
		inline Pitaya::GPU::FrameBuffer* InvokeOnGetFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id) const
		{
			return OnGetFrameBuffer(id);
		}
		inline Pitaya::GPU::ShaderStorageBuffer* InvokeOnGetShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id) const
		{
			return OnGetShaderStorageBuffer(id);
		}

	private:
		void (ENGINE_CALL *OnDestroyAllGPUResource)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;

		Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> (ENGINE_CALL *OnCreateVertexArray)() = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> (ENGINE_CALL *OnCreateVertexBuffer)(float*, uint32_t) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> (ENGINE_CALL *OnCreateIndexBuffer)(uint32_t*, uint32_t) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::Shader> (ENGINE_CALL *OnCreateShaderVF)(const char*, const char*) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::Shader> (ENGINE_CALL *OnCreateShaderVFG)(const char*, const char*, const char*) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> (ENGINE_CALL *OnCreateTexture2D)(unsigned char*, int, int, int, bool, bool, bool) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> (ENGINE_CALL *OnCreateTextureCubemap)(unsigned char**, int*, int*, int*, bool, bool, bool) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> (ENGINE_CALL *OnCreateUniformBuffer)(uint32_t, uint32_t) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> (ENGINE_CALL *OnCreateFrameBuffer)(const Pitaya::GPU::FrameBufferSpecification&) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> (ENGINE_CALL *OnCreateShaderStorageBuffer)(uint32_t, uint32_t) = nullptr;

		bool (ENGINE_CALL *OnDestroyVertexArray)(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray>) = nullptr;
		bool (ENGINE_CALL *OnDestroyVertexBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer>) = nullptr;
		bool (ENGINE_CALL *OnDestroyIndexBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer>) = nullptr;
		bool (ENGINE_CALL *OnDestroyShader)(Pitaya::GPU::Identifier<Pitaya::GPU::Shader>) = nullptr;
		bool (ENGINE_CALL *OnDestroyTexture2D)(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D>) = nullptr;
		bool (ENGINE_CALL *OnDestroyTextureCubemap)(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap>) = nullptr;
		bool (ENGINE_CALL *OnDestroyUniformBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer>) = nullptr;
		bool (ENGINE_CALL *OnDestroyFrameBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer>) = nullptr;
		bool (ENGINE_CALL *OnDestroyShaderStorageBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer>) = nullptr;

		Pitaya::GPU::VertexArray* (ENGINE_CALL *OnGetVertexArray)(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray>) = nullptr;
		Pitaya::GPU::VertexBuffer* (ENGINE_CALL *OnGetVertexBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer>) = nullptr;
		Pitaya::GPU::IndexBuffer* (ENGINE_CALL *OnGetIndexBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer>) = nullptr;
		Pitaya::GPU::Shader* (ENGINE_CALL *OnGetShader)(Pitaya::GPU::Identifier<Pitaya::GPU::Shader>) = nullptr;
		Pitaya::GPU::Texture2D* (ENGINE_CALL *OnGetTexture2D)(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D>) = nullptr;
		Pitaya::GPU::TextureCubemap* (ENGINE_CALL *OnGetTextureCubemap)(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap>) = nullptr;
		Pitaya::GPU::UniformBuffer* (ENGINE_CALL *OnGetUniformBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer>) = nullptr;
		Pitaya::GPU::FrameBuffer* (ENGINE_CALL *OnGetFrameBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer>) = nullptr;
		Pitaya::GPU::ShaderStorageBuffer* (ENGINE_CALL *OnGetShaderStorageBuffer)(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer>) = nullptr;
	};
}

namespace Pitaya::GPU
{
	inline void DestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyAllGPUResource(passkey);
	}

	inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> CreateVertexArray()
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateVertexArray();
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> CreateVertexBuffer(float* vertices, uint32_t size)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateVertexBuffer(vertices, size);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> CreateIndexBuffer(uint32_t* indices, uint32_t count)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateIndexBuffer(indices, count);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> CreateShader(const char* vertexSource, const char* fragmentSource)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderVF(vertexSource, fragmentSource);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> CreateShader(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderVFG(vertexSource, fragmentSource, geometrySource);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> CreateTexture2D(unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTexture2D(data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> CreateTextureCubemap(unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTextureCubemap(datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> CreateUniformBuffer(uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateUniformBuffer(size, bindingPoint);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> CreateFrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateFrameBuffer(spec);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> CreateShaderStorageBuffer(uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderStorageBuffer(size, bindingPoint);
	}

	inline bool DestroyVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyVertexArray(id);
	}
	inline bool DestroyVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyVertexBuffer(id);
	}
	inline bool DestroyIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyIndexBuffer(id);
	}
	inline bool DestroyShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyShader(id);
	}
	inline bool DestroyTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTexture2D(id);
	}
	inline bool DestroyTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTextureCubemap(id);
	}
	inline bool DestroyUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyUniformBuffer(id);
	}
	inline bool DestroyFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyFrameBuffer(id);
	}
	inline bool DestroyShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyShaderStorageBuffer(id);
	}

	inline Pitaya::GPU::VertexArray* GetVertexArray(Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetVertexArray(id);
	}
	inline Pitaya::GPU::VertexBuffer* GetVertexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetVertexBuffer(id);
	}
	inline Pitaya::GPU::IndexBuffer* GetIndexBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetIndexBuffer(id);
	}
	inline Pitaya::GPU::Shader* GetShader(Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetShader(id);
	}
	inline Pitaya::GPU::Texture2D* GetTexture2D(Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTexture2D(id);
	}
	inline Pitaya::GPU::TextureCubemap* GetTextureCubemap(Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTextureCubemap(id);
	}
	inline Pitaya::GPU::UniformBuffer* GetUniformBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetUniformBuffer(id);
	}
	inline Pitaya::GPU::FrameBuffer* GetFrameBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetFrameBuffer(id);
	}
	inline Pitaya::GPU::ShaderStorageBuffer* GetShaderStorageBuffer(Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetShaderStorageBuffer(id);
	}
}