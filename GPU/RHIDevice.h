#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Container/SlotMap.h>
#include<Core/Utils/Check.h>
#include<GPU/Common/GPUObjectType.h>
#include<Render/Common/FuncTable.h>
#include<GPU/Frontend/Texture/Texture2D.h>
#include<GPU/Frontend/Texture/TextureCubemap.h>
#include<GPU/Frontend/Texture/Texture2DArray.h>
#include<GPU/Frontend/Shader/Shader.h>
#include<GPU/Frontend/Buffer/FrameBuffer.h>
#include<GPU/Frontend/Buffer/ShaderStorageBuffer.h>
#include<GPU/Frontend/Buffer/UniformBuffer.h>
#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<GPU/Frontend/Buffer/IndexBuffer.h>
#include<GPU/Frontend/Buffer/VertexArray.h>
#include<Log/Common/FuncTable.h>

#include<tuple>
#include<type_traits>
#include<utility>

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
            template<GPUObjectType T, typename... Args>
            inline auto Create(Args&&... args) 
            {
                try 
                {
                    auto& map = std::get<Pitaya::Core::SlotMap<T>>(maps);
                    return map.Emplace(T::Factory::Create(std::forward<Args>(args)...));
                }
                catch (const std::exception& e) 
                {
                    Pitaya::Log::Error(std::string("RHI: Create [") + typeid(T).name() + "] failed! " + e.what());
                    return Pitaya::Core::SlotMap<T>::Handle::Invalid;
                }
                catch (...)
                {
                    Pitaya::Log::Error(std::string("RHI: Create [") + typeid(T).name() + "] failed! unknown error!");
                    return Pitaya::Core::SlotMap<T>::Handle::Invalid;
                }
            }
            template<GPUObjectType T>
            inline bool Destroy(auto handle)
            {
                auto& map = std::get<Pitaya::Core::SlotMap<T>>(maps);
                if (auto* item = map.Get(handle))
                {
                    T::Factory::Destroy(*item);
                    return map.Remove(handle);
                }
                return false;
            }
            template<GPUObjectType T>
            inline bool Get(auto handle, T& outItem) const
            {
                return std::get<Pitaya::Core::SlotMap<T>>(maps).Get(handle, outItem);
            }
            inline void DestroyAll() 
            {
                std::apply([](auto&... map) 
                    {
                        ([&](auto& m) 
                        {
                            for (auto [handle, item] : m.Each()) 
                            {
                                using T = std::remove_const_t<std::remove_reference_t<decltype(item)>>;
                                T::Factory::Destroy(item);
                            }
                            m.Clear();
                        }(map), ...);
                    }, maps);
            }

        private:
            std::tuple<
                Pitaya::Core::SlotMap<VertexArray>,
                Pitaya::Core::SlotMap<VertexBuffer>,
                Pitaya::Core::SlotMap<IndexBuffer>,
                Pitaya::Core::SlotMap<Shader>,
                Pitaya::Core::SlotMap<Texture2D>,
                Pitaya::Core::SlotMap<TextureCubemap>,
                Pitaya::Core::SlotMap<Texture2DArray>,
                Pitaya::Core::SlotMap<UniformBuffer>,
                Pitaya::Core::SlotMap<FrameBuffer>,
                Pitaya::Core::SlotMap<ShaderStorageBuffer>> maps;
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
        template<GPUObjectType T, typename... Args>
        inline auto Create(Args&&... args)
        {
            PITAYA_CHECK(Pitaya::Render::IsInRenderThread());
            return registry.Create<T>(std::forward<Args>(args)...);
        }
        template<GPUObjectType T>
        inline bool Destroy(typename Pitaya::Core::SlotMap<T>::Handle handle)
        {
            PITAYA_CHECK(Pitaya::Render::IsInRenderThread());
            return registry.Destroy<T>(handle);
        }
        template<GPUObjectType T>
        inline bool Get(typename Pitaya::Core::SlotMap<T>::Handle handle, T& out) const
        {
            PITAYA_CHECK(Pitaya::Render::IsInRenderThread());
            return registry.Get<T>(handle, out);
        }
        inline void DestroyAllGPUResource()
        {
            PITAYA_CHECK(Pitaya::Render::IsInRenderThread());
            registry.DestroyAll();
        }
        inline bool LinkVertexArray(Pitaya::Core::SlotMap<VertexArray>::Handle vaoHandle,
            Pitaya::Core::SlotMap<VertexBuffer>::Handle vboHandle, Pitaya::Core::SlotMap<IndexBuffer>::Handle eboHandle)
        {
            PITAYA_CHECK(Pitaya::Render::IsInRenderThread());
            Pitaya::GPU::VertexArray vao;
            Pitaya::GPU::VertexBuffer vbo;
            Pitaya::GPU::IndexBuffer ebo;
            if (registry.Get<Pitaya::GPU::VertexArray>(vaoHandle, vao) &&
                registry.Get<Pitaya::GPU::VertexBuffer>(vboHandle, vbo) &&
                registry.Get<Pitaya::GPU::IndexBuffer>(eboHandle, ebo))
            {
                Pitaya::GPU::VertexArray::Operate::Link(vao, vbo, ebo);
                return true;
            }

            Pitaya::Log::Error("RHIDevice::LinkVertexArray Failed! Invalid Handle.");
            return false;
        }

    private:
        RHIRegistry registry;
    };
}
