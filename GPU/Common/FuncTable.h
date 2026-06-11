#pragma once


#include<Core/Container/SlotMap.h>
#include<Context/Context.h>
#include<GPU/Layout/BufferLayout.h>
#include<GPU/Common/GPUObjectType.h>
#include<GPU/Common/FrameBufferSpecification.h>
#include<stdexcept>
#include<cstddef>

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
			if (!OnCreateEmptyTextureCubemap) { throw std::runtime_error("FuncTable miss [GPU::CreateEmptyTextureCubemap] Function!"); }
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
			OnCreateEmptyTextureCubemap = nullptr;
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
		inline void InvokeOnDestroyAllGPUResource()
		{
			OnDestroyAllGPUResource();
		}
		inline bool InvokeOnLinkVertexArray(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle vaoHandle,
			Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle vboHandle, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle eboHandle) const
		{
			return OnLinkVertexArray(vaoHandle, vboHandle, eboHandle);
		}

		inline auto InvokeOnCreateVertexArray() const
		{
			return OnCreateVertexArray();
		}
		inline auto InvokeOnCreateVertexBuffer(const float* vertices, uint32_t size, Pitaya::GPU::BufferLayout layout) const
		{
			return OnCreateVertexBuffer(vertices, size, layout);
		}
		inline auto InvokeOnCreateIndexBuffer(const uint32_t* indices, uint32_t count) const
		{
			return OnCreateIndexBuffer(indices, count);
		}

		inline auto InvokeOnCreateShaderVF(const char* vertexSource, size_t vertexSize, const char* fragmentSource, size_t fragmentSize) const
		{
			return OnCreateShaderVF(vertexSource, vertexSize, fragmentSource, fragmentSize);
		}
		inline auto InvokeOnCreateShaderVFG(const char* vertexSource, size_t vertexSize, const char* fragmentSource, size_t fragmentSize, const char* geometrySource, size_t geometrySize) const
		{
			return OnCreateShaderVFG(vertexSource, vertexSize, fragmentSource, fragmentSize, geometrySource, geometrySize);
		}
		inline auto InvokeOnCreateTexture2D(const void* data, int width, int height, Pitaya::GPU::PixelFormat format, bool isGenerateMipmap, bool isNearest) const
		{
			return OnCreateTexture2D(data, width, height, format, isGenerateMipmap, isNearest);
		}
		inline auto InvokeOnCreateTextureCubemap(const void** datas, const int* widths, const int* heights, Pitaya::GPU::PixelFormat format, bool isGenerateMipmap, bool isNearest) const
		{
			return OnCreateTextureCubemap(datas, widths, heights, format, isGenerateMipmap, isNearest);
		}
		inline auto InvokeOnCreateEmptyTextureCubemap(int size, int mipLevels, Pitaya::GPU::PixelFormat format) const
		{
			return OnCreateEmptyTextureCubemap(size, mipLevels, format);
		}
		inline auto InvokeOnCreateTexture2DArray(int width, int height, int layers, Pitaya::GPU::PixelFormat format) const
		{
			return OnCreateTexture2DArray(width, height, layers, format);
		}
		inline auto InvokeOnCreateUniformBuffer(uint32_t size, uint32_t bindingPoint) const
		{
			return OnCreateUniformBuffer(size, bindingPoint);
		}
		inline auto InvokeOnCreateFrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec) const
		{
			return OnCreateFrameBuffer(spec);
		}
		inline auto InvokeOnCreateEmptyFrameBuffer() const
		{
			return OnCreateEmptyFrameBuffer();
		}
		inline auto InvokeOnCreateShaderStorageBuffer(uint32_t size, uint32_t bindingPoint) const
		{
			return OnCreateShaderStorageBuffer(size, bindingPoint);
		}

		inline bool InvokeOnDestroyVertexArray(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle) const
		{
			return OnDestroyVertexArray(handle);
		}
		inline bool InvokeOnDestroyVertexBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle) const
		{
			return OnDestroyVertexBuffer(handle);
		}
		inline bool InvokeOnDestroyIndexBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle) const
		{
			return OnDestroyIndexBuffer(handle);
		}
		inline bool InvokeOnDestroyShader(Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle) const
		{
			return OnDestroyShader(handle);
		}
		inline bool InvokeOnDestroyTexture2D(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle) const
		{
			return OnDestroyTexture2D(handle);
		}
		inline bool InvokeOnDestroyTextureCubemap(Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle) const
		{
			return OnDestroyTextureCubemap(handle);
		}
		inline bool InvokeOnDestroyTexture2DArray(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle) const
		{
			return OnDestroyTexture2DArray(handle);
		}
		inline bool InvokeOnDestroyUniformBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle) const
		{
			return OnDestroyUniformBuffer(handle);
		}
		inline bool InvokeOnDestroyFrameBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle) const
		{
			return OnDestroyFrameBuffer(handle);
		}
		inline bool InvokeOnDestroyShaderStorageBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle) const
		{
			return OnDestroyShaderStorageBuffer(handle);
		}

		inline bool InvokeOnGetVertexArray(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle, Pitaya::GPU::VertexArray& outItem) const
		{
			return OnGetVertexArray(handle, outItem);
		}
		inline bool InvokeOnGetVertexBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle, Pitaya::GPU::VertexBuffer& outItem) const
		{
			return OnGetVertexBuffer(handle, outItem);
		}
		inline bool InvokeOnGetIndexBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle, Pitaya::GPU::IndexBuffer& outItem) const
		{
			return OnGetIndexBuffer(handle, outItem);
		}
		inline bool InvokeOnGetShader(Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle, Pitaya::GPU::Shader& outItem) const
		{
			return OnGetShader(handle, outItem);
		}
		inline bool InvokeOnGetTexture2D(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle, Pitaya::GPU::Texture2D& outItem) const
		{
			return OnGetTexture2D(handle, outItem);
		}
		inline bool InvokeOnGetTextureCubemap(Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle, Pitaya::GPU::TextureCubemap& outItem) const
		{
			return OnGetTextureCubemap(handle, outItem);
		}
		inline bool InvokeOnGetTexture2DArray(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle, Pitaya::GPU::Texture2DArray& outItem) const
		{
			return OnGetTexture2DArray(handle, outItem);
		}
		inline bool InvokeOnGetUniformBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle, Pitaya::GPU::UniformBuffer& outItem) const
		{
			return OnGetUniformBuffer(handle, outItem);
		}
		inline bool InvokeOnGetFrameBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle, Pitaya::GPU::FrameBuffer& outItem) const
		{
			return OnGetFrameBuffer(handle, outItem);
		}
		inline bool InvokeOnGetShaderStorageBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle, Pitaya::GPU::ShaderStorageBuffer& outItem) const
		{
			return OnGetShaderStorageBuffer(handle, outItem);
		}

	private:
		void (ENGINE_CALL *OnDestroyAllGPUResource)() = nullptr;
		bool (ENGINE_CALL* OnLinkVertexArray)(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle,
			Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle) = nullptr;

		Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle (ENGINE_CALL *OnCreateVertexArray)() = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle (ENGINE_CALL *OnCreateVertexBuffer)(const float*, uint32_t, Pitaya::GPU::BufferLayout) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle (ENGINE_CALL *OnCreateIndexBuffer)(const uint32_t*, uint32_t) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle (ENGINE_CALL *OnCreateShaderVF)(const char*, size_t, const char*, size_t) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle  (ENGINE_CALL *OnCreateShaderVFG)(const char*, size_t, const char*, size_t, const char*, size_t) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle (ENGINE_CALL *OnCreateTexture2D)(const void*, int, int, Pitaya::GPU::PixelFormat, bool, bool) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle (ENGINE_CALL *OnCreateTextureCubemap)(const void**, const int*, const int*, Pitaya::GPU::PixelFormat, bool, bool) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle (ENGINE_CALL *OnCreateEmptyTextureCubemap)(int, int, Pitaya::GPU::PixelFormat) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle (ENGINE_CALL *OnCreateTexture2DArray)(int, int, int, Pitaya::GPU::PixelFormat) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle (ENGINE_CALL *OnCreateUniformBuffer)(uint32_t, uint32_t) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle (ENGINE_CALL *OnCreateFrameBuffer)(const Pitaya::GPU::FrameBufferSpecification&) = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle(ENGINE_CALL* OnCreateEmptyFrameBuffer)() = nullptr;
		Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle (ENGINE_CALL *OnCreateShaderStorageBuffer)(uint32_t, uint32_t) = nullptr;

		bool (ENGINE_CALL *OnDestroyVertexArray)(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyVertexBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyIndexBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyShader)(Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyTexture2D)(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyTextureCubemap)(Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyTexture2DArray)(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyUniformBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyFrameBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle) = nullptr;
		bool (ENGINE_CALL *OnDestroyShaderStorageBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle) = nullptr;

		bool (ENGINE_CALL *OnGetVertexArray)(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle, Pitaya::GPU::VertexArray&) = nullptr;
		bool (ENGINE_CALL *OnGetVertexBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle, Pitaya::GPU::VertexBuffer&) = nullptr;
		bool (ENGINE_CALL *OnGetIndexBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle, Pitaya::GPU::IndexBuffer&) = nullptr;
		bool (ENGINE_CALL *OnGetShader)(Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle, Pitaya::GPU::Shader&) = nullptr;
		bool (ENGINE_CALL *OnGetTexture2D)(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle, Pitaya::GPU::Texture2D&) = nullptr;
		bool (ENGINE_CALL *OnGetTextureCubemap)(Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle, Pitaya::GPU::TextureCubemap&) = nullptr;
		bool (ENGINE_CALL *OnGetTexture2DArray)(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle, Pitaya::GPU::Texture2DArray&) = nullptr;
		bool (ENGINE_CALL *OnGetUniformBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle, Pitaya::GPU::UniformBuffer&) = nullptr;
		bool (ENGINE_CALL *OnGetFrameBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle, Pitaya::GPU::FrameBuffer&) = nullptr;
		bool (ENGINE_CALL *OnGetShaderStorageBuffer)(Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle, Pitaya::GPU::ShaderStorageBuffer&) = nullptr;
	};
}

namespace Pitaya::GPU
{
	inline void DestroyAllGPUResource()
	{
		Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyAllGPUResource();
	}
	inline bool LinkVertexArray(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle vaoHandle,
		Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle vboHandle, Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle eboHandle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnLinkVertexArray(vaoHandle, vboHandle, eboHandle);
	}

	inline auto CreateVertexArray()
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateVertexArray();
	}
	inline auto CreateVertexBuffer(const float* vertices, uint32_t size, Pitaya::GPU::BufferLayout layout)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateVertexBuffer(vertices, size, layout);
	}
	inline auto CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateIndexBuffer(indices, count);
	}
	inline auto CreateShader(const char* vertexSource, size_t vertexSize, const char* fragmentSource, size_t fragmentSize)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderVF(vertexSource, vertexSize, fragmentSource, fragmentSize);
	}
	inline auto CreateShader(const char* vertexSource, size_t vertexSize, const char* fragmentSource, size_t fragmentSize, const char* geometrySource, size_t geometrySize)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderVFG(vertexSource, vertexSize, fragmentSource, fragmentSize, geometrySource, geometrySize);
	}
	inline auto CreateTexture2D(const void* data, int width, int height, Pitaya::GPU::PixelFormat format, bool isGenerateMipmap, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTexture2D(data, width, height, format, isGenerateMipmap, isNearest);
	}
	inline auto CreateTextureCubemap(const void** datas, const int* widths, const int* heights, Pitaya::GPU::PixelFormat format, bool isGenerateMipmap, bool isNearest)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTextureCubemap(datas, widths, heights, format, isGenerateMipmap, isNearest);
	}
	inline auto CreateEmptyTextureCubemap(int size, int mipLevels, Pitaya::GPU::PixelFormat format)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateEmptyTextureCubemap(size, mipLevels, format);
	}
	inline auto CreateTexture2DArray(int width, int height, int layers, Pitaya::GPU::PixelFormat format)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateTexture2DArray(width, height, layers, format);
	}
	inline auto CreateUniformBuffer(uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateUniformBuffer(size, bindingPoint);
	}
	inline auto CreateFrameBuffer(const Pitaya::GPU::FrameBufferSpecification& spec)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateFrameBuffer(spec);
	}
	inline auto CreateEmptyFrameBuffer()
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateEmptyFrameBuffer();
	}
	inline auto CreateShaderStorageBuffer(uint32_t size, uint32_t bindingPoint)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnCreateShaderStorageBuffer(size, bindingPoint);
	}

	inline bool DestroyVertexArray(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyVertexArray(handle);
	}
	inline bool DestroyVertexBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyVertexBuffer(handle);
	}
	inline bool DestroyIndexBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyIndexBuffer(handle);
	}
	inline bool DestroyShader(Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyShader(handle);
	}
	inline bool DestroyTexture2D(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTexture2D(handle);
	}
	inline bool DestroyTextureCubemap(Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTextureCubemap(handle);
	}
	inline bool DestroyTexture2DArray(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyTexture2DArray(handle);
	}
	inline bool DestroyUniformBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyUniformBuffer(handle);
	}
	inline bool DestroyFrameBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyFrameBuffer(handle);
	}
	inline bool DestroyShaderStorageBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnDestroyShaderStorageBuffer(handle);
	}

	inline bool GetVertexArray(Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle handle, Pitaya::GPU::VertexArray& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetVertexArray(handle, outItem);
	}
	inline bool GetVertexBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::VertexBuffer>::Handle handle, Pitaya::GPU::VertexBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetVertexBuffer(handle, outItem);
	}
	inline bool GetIndexBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::IndexBuffer>::Handle handle, Pitaya::GPU::IndexBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetIndexBuffer(handle, outItem);
	}
	inline bool GetShader(Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle handle, Pitaya::GPU::Shader& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetShader(handle, outItem);
	}
	inline bool GetTexture2D(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle handle, Pitaya::GPU::Texture2D& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTexture2D(handle, outItem);
	}
	inline bool GetTextureCubemap(Pitaya::Core::SlotMap<Pitaya::GPU::TextureCubemap>::Handle handle, Pitaya::GPU::TextureCubemap& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTextureCubemap(handle, outItem);
	}
	inline bool GetTexture2DArray(Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle handle, Pitaya::GPU::Texture2DArray& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetTexture2DArray(handle, outItem);
	}
	inline bool GetUniformBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle handle, Pitaya::GPU::UniformBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetUniformBuffer(handle, outItem);
	}
	inline bool GetFrameBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle handle, Pitaya::GPU::FrameBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetFrameBuffer(handle, outItem);
	}
	inline bool GetShaderStorageBuffer(Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle handle, Pitaya::GPU::ShaderStorageBuffer& outItem)
	{
		return Pitaya::Engine::Context::Instance().GetFuncTable<Pitaya::GPU::RHIDevice>().InvokeOnGetShaderStorageBuffer(handle, outItem);
	}
}
