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

		inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> InvokeOnCreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey) const
		{
			return OnCreateVertexArray(passkey);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> InvokeOnCreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, float* vertices, uint32_t size) const
		{
			return OnCreateVertexBuffer(passkey, vertices, size);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> InvokeOnCreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t* indices, uint32_t count) const
		{
			return OnCreateIndexBuffer(passkey, indices, count);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> InvokeOnCreateShaderVF(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource) const
		{
			return OnCreateShaderVF(passkey, vertexSource, fragmentSource);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> InvokeOnCreateShaderVFG(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource, const char* geometrySource) const
		{
			return OnCreateShaderVFG(passkey, vertexSource, fragmentSource, geometrySource);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> InvokeOnCreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest) const
		{
			return OnCreateTexture2D(passkey, data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> InvokeOnCreateTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest) const
		{
			return OnCreateTextureCubemap(passkey, datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> InvokeOnCreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint) const
		{
			return OnCreateUniformBuffer(passkey, size, bindingPoint);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> InvokeOnCreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const Pitaya::GPU::FrameBufferSpecification& spec) const
		{
			return OnCreateFrameBuffer(passkey, spec);
		}
		inline Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> InvokeOnCreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint) const
		{
			return OnCreateShaderStorageBuffer(passkey, size, bindingPoint);
		}

		inline bool InvokeOnDestroyVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id) const
		{
			return OnDestroyVertexArray(passkey, id);
		}
		inline bool InvokeOnDestroyVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id) const
		{
			return OnDestroyVertexBuffer(passkey, id);
		}
		inline bool InvokeOnDestroyIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id) const
		{
			return OnDestroyIndexBuffer(passkey, id);
		}
		inline bool InvokeOnDestroyShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id) const
		{
			return OnDestroyShader(passkey, id);
		}
		inline bool InvokeOnDestroyTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id) const
		{
			return OnDestroyTexture2D(passkey, id);
		}
		inline bool InvokeOnDestroyTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id) const
		{
			return OnDestroyTextureCubemap(passkey, id);
		}
		inline bool InvokeOnDestroyUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id) const
		{
			return OnDestroyUniformBuffer(passkey, id);
		}
		inline bool InvokeOnDestroyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id) const
		{
			return OnDestroyFrameBuffer(passkey, id);
		}
		inline bool InvokeOnDestroyShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id) const
		{
			return OnDestroyShaderStorageBuffer(passkey, id);
		}

		inline Pitaya::GPU::VertexArray* InvokeOnGetVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id) const
		{
			return OnGetVertexArray(passkey, id);
		}
		inline Pitaya::GPU::VertexBuffer* InvokeOnGetVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id) const
		{
			return OnGetVertexBuffer(passkey, id);
		}
		inline Pitaya::GPU::IndexBuffer* InvokeOnGetIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id) const
		{
			return OnGetIndexBuffer(passkey, id);
		}
		inline Pitaya::GPU::Shader* InvokeOnGetShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id) const
		{
			return OnGetShader(passkey, id);
		}
		inline Pitaya::GPU::Texture2D* InvokeOnGetTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id) const
		{
			return OnGetTexture2D(passkey, id);
		}
		inline Pitaya::GPU::TextureCubemap* InvokeOnGetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id) const
		{
			return OnGetTextureCubemap(passkey, id);
		}
		inline Pitaya::GPU::UniformBuffer* InvokeOnGetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id) const
		{
			return OnGetUniformBuffer(passkey, id);
		}
		inline Pitaya::GPU::FrameBuffer* InvokeOnGetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id) const
		{
			return OnGetFrameBuffer(passkey, id);
		}
		inline Pitaya::GPU::ShaderStorageBuffer* InvokeOnGetShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id) const
		{
			return OnGetShaderStorageBuffer(passkey, id);
		}

	private:
		void (ENGINE_CALL *OnDestroyAllGPUResource)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;

		Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> (ENGINE_CALL *OnCreateVertexArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> (ENGINE_CALL *OnCreateVertexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, float*, uint32_t) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> (ENGINE_CALL *OnCreateIndexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t*, uint32_t) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::Shader> (ENGINE_CALL *OnCreateShaderVF)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const char*, const char*) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::Shader> (ENGINE_CALL *OnCreateShaderVFG)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const char*, const char*, const char*) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> (ENGINE_CALL *OnCreateTexture2D)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, unsigned char*, int, int, int, bool, bool, bool) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> (ENGINE_CALL *OnCreateTextureCubemap)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, unsigned char**, int*, int*, int*, bool, bool, bool) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> (ENGINE_CALL *OnCreateUniformBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t, uint32_t) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> (ENGINE_CALL *OnCreateFrameBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const Pitaya::GPU::FrameBufferSpecification&) = nullptr;
		Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> (ENGINE_CALL *OnCreateShaderStorageBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t, uint32_t) = nullptr;

		bool (ENGINE_CALL *OnDestroyVertexArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray>) = nullptr;
		bool (ENGINE_CALL *OnDestroyVertexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer>) = nullptr;
		bool (ENGINE_CALL *OnDestroyIndexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer>) = nullptr;
		bool (ENGINE_CALL *OnDestroyShader)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Shader>) = nullptr;
		bool (ENGINE_CALL *OnDestroyTexture2D)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D>) = nullptr;
		bool (ENGINE_CALL *OnDestroyTextureCubemap)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap>) = nullptr;
		bool (ENGINE_CALL *OnDestroyUniformBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer>) = nullptr;
		bool (ENGINE_CALL *OnDestroyFrameBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer>) = nullptr;
		bool (ENGINE_CALL *OnDestroyShaderStorageBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer>) = nullptr;

		Pitaya::GPU::VertexArray* (ENGINE_CALL *OnGetVertexArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray>) = nullptr;
		Pitaya::GPU::VertexBuffer* (ENGINE_CALL *OnGetVertexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer>) = nullptr;
		Pitaya::GPU::IndexBuffer* (ENGINE_CALL *OnGetIndexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer>) = nullptr;
		Pitaya::GPU::Shader* (ENGINE_CALL *OnGetShader)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Shader>) = nullptr;
		Pitaya::GPU::Texture2D* (ENGINE_CALL *OnGetTexture2D)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D>) = nullptr;
		Pitaya::GPU::TextureCubemap* (ENGINE_CALL *OnGetTextureCubemap)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap>) = nullptr;
		Pitaya::GPU::UniformBuffer* (ENGINE_CALL *OnGetUniformBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer>) = nullptr;
		Pitaya::GPU::FrameBuffer* (ENGINE_CALL *OnGetFrameBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer>) = nullptr;
		Pitaya::GPU::ShaderStorageBuffer* (ENGINE_CALL *OnGetShaderStorageBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer>) = nullptr;
	};
}

namespace Pitaya::GPU
{
	inline void DestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyAllGPUResource(passkey);
	}

	inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateVertexArray(passkey);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> CreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, float* vertices, uint32_t size)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateVertexBuffer(passkey, vertices, size);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> CreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t* indices, uint32_t count)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateIndexBuffer(passkey, indices, count);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderVF(passkey, vertexSource, fragmentSource);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Shader> CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderVFG(passkey, vertexSource, fragmentSource, geometrySource);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> CreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTexture2D(passkey, data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> CreateTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTextureCubemap(passkey, datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateUniformBuffer(passkey, size, bindingPoint);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const Pitaya::GPU::FrameBufferSpecification& spec)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateFrameBuffer(passkey, spec);
	}
	inline Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderStorageBuffer(passkey, size, bindingPoint);
	}

	inline bool DestroyVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyVertexArray(passkey, id);
	}
	inline bool DestroyVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyVertexBuffer(passkey, id);
	}
	inline bool DestroyIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyIndexBuffer(passkey, id);
	}
	inline bool DestroyShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyShader(passkey, id);
	}
	inline bool DestroyTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTexture2D(passkey, id);
	}
	inline bool DestroyTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTextureCubemap(passkey, id);
	}
	inline bool DestroyUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyUniformBuffer(passkey, id);
	}
	inline bool DestroyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyFrameBuffer(passkey, id);
	}
	inline bool DestroyShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyShaderStorageBuffer(passkey, id);
	}

	inline Pitaya::GPU::VertexArray* GetVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetVertexArray(passkey, id);
	}
	inline Pitaya::GPU::VertexBuffer* GetVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::VertexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetVertexBuffer(passkey, id);
	}
	inline Pitaya::GPU::IndexBuffer* GetIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::IndexBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetIndexBuffer(passkey, id);
	}
	inline Pitaya::GPU::Shader* GetShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::Shader> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetShader(passkey, id);
	}
	inline Pitaya::GPU::Texture2D* GetTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTexture2D(passkey, id);
	}
	inline Pitaya::GPU::TextureCubemap* GetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::TextureCubemap> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTextureCubemap(passkey, id);
	}
	inline Pitaya::GPU::UniformBuffer* GetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetUniformBuffer(passkey, id);
	}
	inline Pitaya::GPU::FrameBuffer* GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetFrameBuffer(passkey, id);
	}
	inline Pitaya::GPU::ShaderStorageBuffer* GetShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> id)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetShaderStorageBuffer(passkey, id);
	}
}