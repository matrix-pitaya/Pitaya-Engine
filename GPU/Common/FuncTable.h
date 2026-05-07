#pragma once

#include<Core/PassKey/PassKey.h>
#include<Core/Container/SlotMap.h>
#include<Context/Context.h>
#include<GPU/Layout/BufferLayout.h>
#include<GPU/Common/GPUObjectType.h>
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
		inline bool Verify() const
		{
			if (!OnDestroyAllGPUResource) { throw std::runtime_error("FuncTable miss [GPU::DestroyAllGPUResource] Function!"); }
			if (!OnLinkVertexArray) { throw std::runtime_error("FuncTable miss [GPU::LinkVertexArray] Function!"); }

			if (!OnCreateVertexArray) { throw std::runtime_error("FuncTable miss [GPU::CreateVertexArray] Function!"); }
			if (!OnCreateVertexBuffer) { throw std::runtime_error("FuncTable miss [GPU::CreateVertexBuffer] Function!"); }
			if (!OnCreateIndexBuffer) { throw std::runtime_error("FuncTable miss [GPU::CreateIndexBuffer] Function!"); }
			if (!OnCreateShaderVF) { throw std::runtime_error("FuncTable miss [GPU::CreateShaderVF] Function!"); }
			if (!OnCreateShaderVFG) { throw std::runtime_error("FuncTable miss [GPU::CreateShaderVFG] Function!"); }
			if (!OnCreateTexture2D) { throw std::runtime_error("FuncTable miss [GPU::CreateTexture2D] Function!"); }
			if (!OnCreateTexture2DArray) { throw std::runtime_error("FuncTable miss [GPU::CreateTexture2DArray] Function!"); }
			if (!OnCreateTextureCubemap) { throw std::runtime_error("FuncTable miss [GPU::CreateTextureCubemap] Function!"); }
			if (!OnCreateUniformBuffer) { throw std::runtime_error("FuncTable miss [GPU::CreateUniformBuffer] Function!"); }
			if (!OnCreateFrameBuffer) { throw std::runtime_error("FuncTable miss [GPU::CreateFrameBuffer] Function!"); }
			if (!OnCreateEmptyFrameBuffer) { throw std::runtime_error("FuncTable miss [GPU::CreateEmptyFrameBuffer] Function!"); }
			if (!OnCreateShaderStorageBuffer) { throw std::runtime_error("FuncTable miss [GPU::CreateShaderStorageBuffer] Function!"); }

			if (!OnDestroyVertexArray) { throw std::runtime_error("FuncTable miss [GPU::DestroyVertexArray] Function!"); }
			if (!OnDestroyVertexBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyVertexBuffer] Function!"); }
			if (!OnDestroyIndexBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyIndexBuffer] Function!"); }
			if (!OnDestroyShader) { throw std::runtime_error("FuncTable miss [GPU::DestroyShader] Function!"); }
			if (!OnDestroyTexture2D) { throw std::runtime_error("FuncTable miss [GPU::DestroyTexture2D] Function!"); }
			if (!OnDestroyTextureCubemap) { throw std::runtime_error("FuncTable miss [GPU::DestroyTextureCubemap] Function!"); }
			if (!OnDestroyTexture2DArray) { throw std::runtime_error("FuncTable miss [GPU::DestroyTexture2DArray] Function!"); }
			if (!OnDestroyUniformBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyUniformBuffer] Function!"); }
			if (!OnDestroyFrameBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyFrameBuffer] Function!"); }
			if (!OnDestroyShaderStorageBuffer) { throw std::runtime_error("FuncTable miss [GPU::DestroyShaderStorageBuffer] Function!"); }

			if (!OnGetVertexArray) { throw std::runtime_error("FuncTable miss [GPU::GetVertexArray] Function!"); }
			if (!OnGetVertexBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetVertexBuffer] Function!"); }
			if (!OnGetIndexBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetIndexBuffer] Function!"); }
			if (!OnGetShader) { throw std::runtime_error("FuncTable miss [GPU::GetShader] Function!"); }
			if (!OnGetTexture2D) { throw std::runtime_error("FuncTable miss [GPU::GetTexture2D] Function!"); }
			if (!OnGetTextureCubemap) { throw std::runtime_error("FuncTable miss [GPU::GetTextureCubemap] Function!"); }
			if (!OnGetTexture2DArray) { throw std::runtime_error("FuncTable miss [GPU::GetTexture2DArray] Function!"); }
			if (!OnGetUniformBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetUniformBuffer] Function!"); }
			if (!OnGetFrameBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetFrameBuffer] Function!"); }
			if (!OnGetShaderStorageBuffer) { throw std::runtime_error("FuncTable miss [GPU::GetShaderStorageBuffer] Function!"); }

			return true;
		}
		inline void Nullify() noexcept
		{
			OnDestroyAllGPUResource = nullptr;
			OnLinkVertexArray = nullptr;

			OnCreateVertexArray = nullptr;
			OnCreateVertexBuffer = nullptr;
			OnCreateIndexBuffer = nullptr;
			OnCreateShaderVF = nullptr;
			OnCreateShaderVFG = nullptr;
			OnCreateTexture2D = nullptr;
			OnCreateTextureCubemap = nullptr;
			OnCreateTexture2DArray = nullptr;
			OnCreateUniformBuffer = nullptr;
			OnCreateFrameBuffer = nullptr;
			OnCreateEmptyFrameBuffer = nullptr;
			OnCreateShaderStorageBuffer = nullptr;

			OnDestroyVertexArray = nullptr;
			OnDestroyVertexBuffer= nullptr;
			OnDestroyIndexBuffer = nullptr;
			OnDestroyShader = nullptr;
			OnDestroyTexture2D = nullptr;
			OnDestroyTextureCubemap = nullptr;
			OnDestroyTexture2DArray = nullptr;
			OnDestroyUniformBuffer = nullptr;
			OnDestroyFrameBuffer = nullptr;
			OnDestroyShaderStorageBuffer = nullptr;

			OnGetVertexArray= nullptr;
			OnGetVertexBuffer = nullptr;
			OnGetIndexBuffer= nullptr;
			OnGetShader = nullptr;
			OnGetTexture2D = nullptr;
			OnGetTextureCubemap = nullptr;
			OnGetTexture2DArray = nullptr;
			OnGetUniformBuffer = nullptr;
			OnGetFrameBuffer = nullptr;
			OnGetShaderStorageBuffer = nullptr;
		}

	public:
		inline void InvokeOnDestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
		{
			OnDestroyAllGPUResource(passkey);
		}
		inline bool InvokeOnLinkVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle vaoHandle,
			Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle vboHandle, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle eboHandle) const
		{
			return OnLinkVertexArray(passkey, vaoHandle, vboHandle, eboHandle);
		}

		inline auto InvokeOnCreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey) const
		{
			return OnCreateVertexArray(passkey);
		}
		inline auto InvokeOnCreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, float* vertices, uint32_t size, Pitaya::GPU::BufferLayout layout) const
		{
			return OnCreateVertexBuffer(passkey, vertices, size, layout);
		}
		inline auto InvokeOnCreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t* indices, uint32_t count) const
		{
			return OnCreateIndexBuffer(passkey, indices, count);
		}

		inline auto InvokeOnCreateShaderVF(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource) const
		{
			return OnCreateShaderVF(passkey, vertexSource, fragmentSource);
		}
		inline auto InvokeOnCreateShaderVFG(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource, const char* geometrySource) const
		{
			return OnCreateShaderVFG(passkey, vertexSource, fragmentSource, geometrySource);
		}
		inline auto InvokeOnCreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest) const
		{
			return OnCreateTexture2D(passkey, data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
		}
		inline auto InvokeOnCreateTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest) const
		{
			return OnCreateTextureCubemap(passkey, datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
		}
		inline auto InvokeOnCreateTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, int width, int height, int layers, bool isDepth) const
		{
			return OnCreateTexture2DArray(passkey, width, height, layers, isDepth);
		}
		inline auto InvokeOnCreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint) const
		{
			return OnCreateUniformBuffer(passkey, size, bindingPoint);
		}
		inline auto InvokeOnCreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const Pitaya::GPU::FrameBufferSpecification& spec) const
		{
			return OnCreateFrameBuffer(passkey, spec);
		}
		inline auto InvokeOnCreateEmptyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey) const
		{
			return OnCreateEmptyFrameBuffer(passkey);
		}
		inline auto InvokeOnCreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint) const
		{
			return OnCreateShaderStorageBuffer(passkey, size, bindingPoint);
		}

		inline bool InvokeOnDestroyVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle) const
		{
			return OnDestroyVertexArray(passkey, handle);
		}
		inline bool InvokeOnDestroyVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle) const
		{
			return OnDestroyVertexBuffer(passkey, handle);
		}
		inline bool InvokeOnDestroyIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle) const
		{
			return OnDestroyIndexBuffer(passkey, handle);
		}
		inline bool InvokeOnDestroyShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle) const
		{
			return OnDestroyShader(passkey, handle);
		}
		inline bool InvokeOnDestroyTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle) const
		{
			return OnDestroyTexture2D(passkey, handle);
		}
		inline bool InvokeOnDestroyTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle) const
		{
			return OnDestroyTextureCubemap(passkey, handle);
		}
		inline bool InvokeOnDestroyTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle) const
		{
			return OnDestroyTexture2DArray(passkey, handle);
		}
		inline bool InvokeOnDestroyUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle) const
		{
			return OnDestroyUniformBuffer(passkey, handle);
		}
		inline bool InvokeOnDestroyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle) const
		{
			return OnDestroyFrameBuffer(passkey, handle);
		}
		inline bool InvokeOnDestroyShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle) const
		{
			return OnDestroyShaderStorageBuffer(passkey, handle);
		}

		inline bool InvokeOnGetVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle, Pitaya::GPU::VertexArray& outItem) const
		{
			return OnGetVertexArray(passkey, handle, outItem);
		}
		inline bool InvokeOnGetVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle, Pitaya::GPU::VertexBuffer& outItem) const
		{
			return OnGetVertexBuffer(passkey, handle, outItem);
		}
		inline bool InvokeOnGetIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle, Pitaya::GPU::IndexBuffer& outItem) const
		{
			return OnGetIndexBuffer(passkey, handle, outItem);
		}
		inline bool InvokeOnGetShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle, Pitaya::GPU::Shader& outItem) const
		{
			return OnGetShader(passkey, handle, outItem);
		}
		inline bool InvokeOnGetTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle, Pitaya::GPU::Texture2D& outItem) const
		{
			return OnGetTexture2D(passkey, handle, outItem);
		}
		inline bool InvokeOnGetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle, Pitaya::GPU::TextureCubemap& outItem) const
		{
			return OnGetTextureCubemap(passkey, handle, outItem);
		}
		inline bool InvokeOnGetTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle, Pitaya::GPU::Texture2DArray& outItem) const
		{
			return OnGetTexture2DArray(passkey, handle, outItem);
		}
		inline bool InvokeOnGetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle, Pitaya::GPU::UniformBuffer& outItem) const
		{
			return OnGetUniformBuffer(passkey, handle, outItem);
		}
		inline bool InvokeOnGetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle, Pitaya::GPU::FrameBuffer& outItem) const
		{
			return OnGetFrameBuffer(passkey, handle, outItem);
		}
		inline bool InvokeOnGetShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle, Pitaya::GPU::ShaderStorageBuffer& outItem) const
		{
			return OnGetShaderStorageBuffer(passkey, handle, outItem);
		}

	private:
		void (ENGINE_CALL *OnDestroyAllGPUResource)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;
		bool (ENGINE_CALL* OnLinkVertexArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle,
			Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle) = nullptr;

		Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle (ENGINE_CALL *OnCreateVertexArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle (ENGINE_CALL *OnCreateVertexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, float*, uint32_t, Pitaya::GPU::BufferLayout) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle (ENGINE_CALL *OnCreateIndexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t*, uint32_t) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle (ENGINE_CALL *OnCreateShaderVF)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const char*, const char*) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle  (ENGINE_CALL *OnCreateShaderVFG)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const char*, const char*, const char*) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle (ENGINE_CALL *OnCreateTexture2D)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, unsigned char*, int, int, int, bool, bool, bool) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle (ENGINE_CALL *OnCreateTextureCubemap)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, unsigned char**, int*, int*, int*, bool, bool, bool) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle (ENGINE_CALL *OnCreateTexture2DArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, int, int, int, bool) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle (ENGINE_CALL *OnCreateUniformBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t, uint32_t) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle (ENGINE_CALL *OnCreateFrameBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, const Pitaya::GPU::FrameBufferSpecification&) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle(ENGINE_CALL* OnCreateEmptyFrameBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle (ENGINE_CALL *OnCreateShaderStorageBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, uint32_t, uint32_t) = nullptr;

		bool (ENGINE_CALL *OnDestroyVertexArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyVertexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyIndexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyShader)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyTexture2D)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyTextureCubemap)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyTexture2DArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyUniformBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyFrameBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyShaderStorageBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle) = nullptr;

		bool (ENGINE_CALL *OnGetVertexArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle, Pitaya::GPU::VertexArray&) = nullptr;
		bool (ENGINE_CALL *OnGetVertexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle, Pitaya::GPU::VertexBuffer&) = nullptr;
		bool (ENGINE_CALL *OnGetIndexBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle, Pitaya::GPU::IndexBuffer&) = nullptr;
		bool (ENGINE_CALL *OnGetShader)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle, Pitaya::GPU::Shader&) = nullptr;
		bool (ENGINE_CALL *OnGetTexture2D)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle, Pitaya::GPU::Texture2D&) = nullptr;
		bool (ENGINE_CALL *OnGetTextureCubemap)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle, Pitaya::GPU::TextureCubemap&) = nullptr;
		bool (ENGINE_CALL *OnGetTexture2DArray)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle, Pitaya::GPU::Texture2DArray&) = nullptr;
		bool (ENGINE_CALL *OnGetUniformBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle, Pitaya::GPU::UniformBuffer&) = nullptr;
		bool (ENGINE_CALL *OnGetFrameBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle, Pitaya::GPU::FrameBuffer&) = nullptr;
		bool (ENGINE_CALL *OnGetShaderStorageBuffer)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle, Pitaya::GPU::ShaderStorageBuffer&) = nullptr;
	};
}

namespace Pitaya::GPU
{
	inline void DestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyAllGPUResource(passkey);
	}
	inline bool LinkVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle vaoHandle,
		Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle vboHandle, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle eboHandle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnLinkVertexArray(passkey, vaoHandle, vboHandle, eboHandle);
	}

	inline auto CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateVertexArray(passkey);
	}
	inline auto CreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, float* vertices, uint32_t size, Pitaya::GPU::BufferLayout layout)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateVertexBuffer(passkey, vertices, size, layout);
	}
	inline auto CreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t* indices, uint32_t count)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateIndexBuffer(passkey, indices, count);
	}
	inline auto CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderVF(passkey, vertexSource, fragmentSource);
	}
	inline auto CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderVFG(passkey, vertexSource, fragmentSource, geometrySource);
	}
	inline auto CreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char* data, int width, int height, int channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTexture2D(passkey, data, width, height, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	inline auto CreateTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, unsigned char** datas, int* widths, int* heights, int* channels, bool isGenerateMipmap, bool isSRGB, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTextureCubemap(passkey, datas, widths, heights, channels, isGenerateMipmap, isSRGB, isNearest);
	}
	inline auto CreateTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, int width, int height, int layers, bool isDepth)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTexture2DArray(passkey, width, height, layers, isDepth);
	}
	inline auto CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateUniformBuffer(passkey, size, bindingPoint);
	}
	inline auto CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, const Pitaya::GPU::FrameBufferSpecification& spec)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateFrameBuffer(passkey, spec);
	}
	inline auto CreateEmptyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateEmptyFrameBuffer(passkey);
	}
	inline auto CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderStorageBuffer(passkey, size, bindingPoint);
	}

	inline bool DestroyVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyVertexArray(passkey, handle);
	}
	inline bool DestroyVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyVertexBuffer(passkey, handle);
	}
	inline bool DestroyIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyIndexBuffer(passkey, handle);
	}
	inline bool DestroyShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyShader(passkey, handle);
	}
	inline bool DestroyTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTexture2D(passkey, handle);
	}
	inline bool DestroyTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTextureCubemap(passkey, handle);
	}
	inline bool DestroyTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTexture2DArray(passkey, handle);
	}
	inline bool DestroyUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyUniformBuffer(passkey, handle);
	}
	inline bool DestroyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyFrameBuffer(passkey, handle);
	}
	inline bool DestroyShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyShaderStorageBuffer(passkey, handle);
	}

	inline bool GetVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle, Pitaya::GPU::VertexArray& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetVertexArray(passkey, handle, outItem);
	}
	inline bool GetVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle, Pitaya::GPU::VertexBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetVertexBuffer(passkey, handle, outItem);
	}
	inline bool GetIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle, Pitaya::GPU::IndexBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetIndexBuffer(passkey, handle, outItem);
	}
	inline bool GetShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle, Pitaya::GPU::Shader& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetShader(passkey, handle, outItem);
	}
	inline bool GetTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle, Pitaya::GPU::Texture2D& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTexture2D(passkey, handle, outItem);
	}
	inline bool GetTextureCubemap(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle, Pitaya::GPU::TextureCubemap& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTextureCubemap(passkey, handle, outItem);
	}
	inline bool GetTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle, Pitaya::GPU::Texture2DArray& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTexture2DArray(passkey, handle, outItem);
	}
	inline bool GetUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle, Pitaya::GPU::UniformBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetUniformBuffer(passkey, handle, outItem);
	}
	inline bool GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle, Pitaya::GPU::FrameBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetFrameBuffer(passkey, handle, outItem);
	}
	inline bool GetShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer> passkey, Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle, Pitaya::GPU::ShaderStorageBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetShaderStorageBuffer(passkey, handle, outItem);
	}
}
