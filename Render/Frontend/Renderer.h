#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Storage/Storage.h>
#include<Core/Camera/CameraSnapshot.h>
#include<Core/Asset/Asset.h>
#include<Core/Utils/Console.h>
#include<Core/Utils/System.h>
#include<Hook/def.h>
#include<Thread/Common/FuncTable.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Window/Common/FuncTable.h>

#include<Render/Common/API.h>
#include<Render/Common/RenderCommandType.h>
#include<Render/Common/RenderQueue.h>
#include<Render/Common/InstanceInfo.h>
#include<Render/Common/LightInfo.h>
#include<Render/Common/ShadowInfo.h>
#include<Render/Command/BeginPassCommand.h>
#include<Render/Command/DrawCommand.h>
#include<Render/Command/InstancedDrawCommand.h>
#include<Render/Command/BlitToScreenCommand.h>
#include<Render/Command/PostProcessCommand.h>
#include<Render/Command/BeginShadowPassCommand.h>
#include<Render/Specific/RenderPass.h>
#include<Render/Specific/RenderItem.h>

#include<GPU/Common/FuncTable.h>
#include<GPU/Common/TextureType.h>
#include<GPU/Common/BindPoint.h>
#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<GPU/Frontend/Buffer/VertexArray.h>
#include<GPU/Frontend/Buffer/FrameBuffer.h>

#include<Asset/Common/FuncTable.h>
#include<Asset/Common/Shader.h>
#include<Asset/Common/Texture.h>
#include<Asset/Common/Material.h>
#include<Asset/Common/RenderTarget.h>

#include<Application/Built-in.h>

#include<algorithm>
#include<atomic>
#include<condition_variable>
#include<mutex>
#include<vector>
#include<functional>
#include<numeric>
#include<optional>
#include<memory>
#include<cstddef>
#include<future>
#include<filesystem>
#include<fstream>

namespace Pitaya::Render
{
    class Renderer
    {
    public:
        struct Factory
        {
            friend class Pitaya::Engine::Module<Renderer>;
        private:
            static Renderer* Create()
            {
                return PITAYA_NEW(Renderer);
            }
            static void Destroy(Renderer* renderer)
            {
                PITAYA_DELETE(renderer);
            }
        };
        struct Lifecycle
        {
            friend class Pitaya::Engine::Module<Renderer>;
        private:
            static bool Initialize(Renderer* renderer, void* nativeWindow)
            {
                return renderer->Initialize(nativeWindow);
            }
            static void Release(Renderer* renderer)
            {
                renderer->Release();
            }
        };

    public:
        struct GlobalRHI
        {
            //Specific
            struct Specific
            {
                Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle EmptyVAOHandle;
                Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle ShadowFBO;
                Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle DepthOnlyStaticShaderHandle;
                Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle DepthOnlySkinnedShaderHandle;
            } Specific;
            //Fallback
            struct Fallback
            {
                Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle VAOHandle;
                Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle ShaderHandle;
                Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle TextureHandle;
            } Fallback;
            //PostProcess Shader
            struct PostProcessShader
            {
                Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle BlitShaderHandle;
                Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle GammaCorrectionShaderHandle;
            } PostProcessShader;
            //Uniform Buffer
            struct UBO
            {
                Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle Handle;
            } CameraSnapshotUBO, PostProcessParamsUBO;
            //ShaderStorageBuffer
            struct SSBO
            {
                Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle Handle;
                size_t Capacity = 0;	//记录当前显存缓冲区的大小
            } InstanceModelTransformSSBO, BoneInverseMatriceSSBO, SceneLightsSSBO, ShadowSSBO, MaterialSSBO;
            //MainDisplayRT
            struct RenderTarget
            {
                Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle SceneFrameBufferHandle;
                Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle PingPongFrameBufferHandles[2] = { };
                Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle FinalFrameBufferHandle;
            } MainDisplayRenderTarget;
            //Shadow
            struct ShadowAtlas
            {
                Pitaya::Core::SlotMap<Pitaya::GPU::Texture2DArray>::Handle TextureHandle;
                uint32_t LayerCapacity = 0;

                inline static constexpr const uint32_t CSMResolution = 2048;
                inline static constexpr const uint32_t SpotResolution = 1024;
                inline static constexpr const uint32_t PointResolution = 512;
            } CSMAtlas, SpotShadowAtlas, PointShadowAtlas;

            inline void Create(Pitaya::Core::PassKey<Pitaya::Render::Renderer>)
            {
                Specific.EmptyVAOHandle = Pitaya::GPU::CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
                CameraSnapshotUBO.Handle = Pitaya::GPU::CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    sizeof(Pitaya::Core::CameraSnapshot), static_cast<uint32_t>(Pitaya::GPU::UBOBindPoint::CameraSnapshot));
                PostProcessParamsUBO.Handle = Pitaya::GPU::CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    Pitaya::Render::PostProcessStep::UniformBufferBytes, static_cast<uint32_t>(Pitaya::GPU::UBOBindPoint::PostProcessUBO));

                // 初始分配1024个位置
                InstanceModelTransformSSBO.Capacity = 1024 * sizeof(InstanceInfo);
                InstanceModelTransformSSBO.Handle = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    InstanceModelTransformSSBO.Capacity, static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform));

                // 初始分配一段骨骼容量
                BoneInverseMatriceSSBO.Capacity = 4096 * sizeof(glm::mat4);
                BoneInverseMatriceSSBO.Handle = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    BoneInverseMatriceSSBO.Capacity, static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice));

                // 预分配材质容量
                MaterialSSBO.Capacity = 4096 * sizeof(float);
                MaterialSSBO.Handle = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    static_cast<uint32_t>(MaterialSSBO.Capacity), static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::Material));

                // 初始分配10个光源位置
                SceneLightsSSBO.Capacity = 10 * sizeof(Pitaya::Render::LightInfo);
                SceneLightsSSBO.Handle = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    SceneLightsSSBO.Capacity, static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::SceneLights));

                {
                    auto vs = Pitaya::Core::LoadBuiltInRC(IDR_BLIT_VERTEX_SHADER);
                    auto fs = Pitaya::Core::LoadBuiltInRC(IDR_BLIT_FRAGMENT_SHADER);
                    PostProcessShader.BlitShaderHandle = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                        static_cast<const char*>(vs.data), vs.size, static_cast<const char*>(fs.data), fs.size);
                }
                {
                    auto vs = Pitaya::Core::LoadBuiltInRC(IDR_GAMMA_CORRECTION_VERTEX_SHADER);
                    auto fs = Pitaya::Core::LoadBuiltInRC(IDR_GAMMA_CORRECTION_FRAGMENT_SHADER);
                    PostProcessShader.GammaCorrectionShaderHandle = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                        static_cast<const char*>(vs.data), vs.size, static_cast<const char*>(fs.data), fs.size);
                }

                Fallback.VAOHandle = Pitaya::GPU::CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
                auto fallbackVboData = Pitaya::Core::LoadBuiltInRC(IDR_ERROR_VERTICES);
                auto fallbackIboData = Pitaya::Core::LoadBuiltInRC(IDR_ERROR_INDICES);
                auto fallbackVBOHandle = Pitaya::GPU::CreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    static_cast<float*>(const_cast<void*>(fallbackVboData.data)), fallbackVboData.size, {
                        { Pitaya::GPU::ShaderVariableType::Float3, 0 },     // Position
                        { Pitaya::GPU::ShaderVariableType::Float3, 1 },     // Normal
                        { Pitaya::GPU::ShaderVariableType::Float2, 2 },     // UV
                        { Pitaya::GPU::ShaderVariableType::Float4, 3 } });  // Tangent
                auto fallbackIBOHandle = Pitaya::GPU::CreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    static_cast<uint32_t*>(const_cast<void*>(fallbackIboData.data)), 36);
                if (!Pitaya::GPU::LinkVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    Fallback.VAOHandle, fallbackVBOHandle, fallbackIBOHandle))
                {
                    Pitaya::Core::PopMessageBox("Error", "Create Global RHI Failed! Check Log for Details.");
                    Pitaya::Core::Terminate(-1);
                }

                auto errTex = Pitaya::Core::LoadBuiltInRC(IDR_ERROR_TEXTURE);
                Fallback.TextureHandle = Pitaya::GPU::CreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    errTex.data, 32, 32, Pitaya::GPU::PixelFormat::RGBA8, false, true);
                {
                    auto vs = Pitaya::Core::LoadBuiltInRC(IDR_ERROR_VERTEX_SHADER);
                    auto fs = Pitaya::Core::LoadBuiltInRC(IDR_ERROR_FRAGMENT_SHADER);
                    Fallback.ShaderHandle = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                        static_cast<const char*>(vs.data), vs.size, static_cast<const char*>(fs.data), fs.size);
                }

                //MainDisplayRenderTarget
                Pitaya::GPU::FrameBufferSpecification mainSceneSpec = Pitaya::Config::GetMainSceneSpec();
                Pitaya::GPU::FrameBufferSpecification mainPingPongSpec = Pitaya::Config::GetMainPingPongSpec();
                Pitaya::GPU::FrameBufferSpecification mainFinalSpec = Pitaya::Config::GetMainFinalSpec();
                MainDisplayRenderTarget.SceneFrameBufferHandle = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainSceneSpec);
                MainDisplayRenderTarget.PingPongFrameBufferHandles[0] = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainPingPongSpec);
                MainDisplayRenderTarget.PingPongFrameBufferHandles[1] = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainPingPongSpec);
                MainDisplayRenderTarget.FinalFrameBufferHandle = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainFinalSpec);

                // Shadow
                Specific.ShadowFBO = Pitaya::GPU::CreateEmptyFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
                {
                    auto vs = Pitaya::Core::LoadBuiltInRC(IDR_SHADOW_STATIC_VERTEX_SHADER);
                    auto fs = Pitaya::Core::LoadBuiltInRC(IDR_SHADOW_STATIC_FRAGMENT_SHADER);
                    Specific.DepthOnlyStaticShaderHandle = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                        static_cast<const char*>(vs.data), vs.size, static_cast<const char*>(fs.data), fs.size);
                }
                {
                    auto vs = Pitaya::Core::LoadBuiltInRC(IDR_SHADOW_SKINNED_VERTEX_SHADER);
                    auto fs = Pitaya::Core::LoadBuiltInRC(IDR_SHADOW_SKINNED_FRAGMENT_SHADER);
                    Specific.DepthOnlySkinnedShaderHandle = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                        static_cast<const char*>(vs.data), vs.size, static_cast<const char*>(fs.data), fs.size);
                }
                ShadowSSBO.Capacity = 4096;
                ShadowSSBO.Handle = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    static_cast<uint32_t>(ShadowSSBO.Capacity), static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::Shadow));

                // 预分配 Shadow Atlas
                CSMAtlas.LayerCapacity = 4;
                CSMAtlas.TextureHandle = Pitaya::GPU::CreateTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    CSMAtlas.CSMResolution, CSMAtlas.CSMResolution, CSMAtlas.LayerCapacity, Pitaya::GPU::PixelFormat::Depth32F);
                SpotShadowAtlas.LayerCapacity = 4;
                SpotShadowAtlas.TextureHandle = Pitaya::GPU::CreateTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    SpotShadowAtlas.SpotResolution, SpotShadowAtlas.SpotResolution, SpotShadowAtlas.LayerCapacity, Pitaya::GPU::PixelFormat::Depth32F);
                PointShadowAtlas.LayerCapacity = 6;
                PointShadowAtlas.TextureHandle = Pitaya::GPU::CreateTexture2DArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
                    PointShadowAtlas.PointResolution, PointShadowAtlas.PointResolution, PointShadowAtlas.LayerCapacity, Pitaya::GPU::PixelFormat::Depth32F);
            }
        };
        class RenderPacket
        {
            friend class Renderer;
        public:
            struct Buffer
            {
                std::vector<std::byte> CommandBuffer;
                std::vector<InstanceInfo> InstanceInfo;
                std::vector<glm::mat4> BoneMatrices;
                std::vector<std::byte> MaterialParams;
                std::vector<uint32_t> MaterialTexturePatches;
                std::vector<LightInfo> Lights;
                std::vector<std::byte> ShadowSSBOData;
                uint32_t RequiredCSMLayers = 0;
                uint32_t RequiredSpotLayers = 0;
                uint32_t RequiredPointLayers = 0;

                inline void Clear() noexcept
                {
                    CommandBuffer.clear();
                    InstanceInfo.clear();
                    BoneMatrices.clear();
                    MaterialParams.clear();
                    MaterialTexturePatches.clear();
                    Lights.clear();
                    ShadowSSBOData.clear();
                    RequiredCSMLayers = 0;
                    RequiredSpotLayers = 0;
                    RequiredPointLayers = 0;
                }
            };

        private:
            struct CommandHeader
            {
                RenderCommandType type = RenderCommandType::Invalid;
                uint32_t size = 0;
            };

        private:
            RenderPacket() = default;
            ~RenderPacket() = default;

        public:
            RenderPacket(const RenderPacket&) = delete;
            RenderPacket& operator=(const RenderPacket&) = delete;
            RenderPacket(RenderPacket&&) = delete;
            RenderPacket& operator=(RenderPacket&&) = delete;

        public:
            inline void ParseCommand(const Pitaya::Render::Renderer* renderer)
            {
                size_t offset = 0;
                const size_t bufferSize = back.CommandBuffer.size();
                std::byte* rawData = const_cast<std::byte*>(back.CommandBuffer.data());
                while (offset < bufferSize)
                {
                    void* ptr = rawData + offset;
                    size_t space = bufferSize - offset;

                    if (!std::align(alignof(CommandHeader), sizeof(CommandHeader), ptr, space)) { break; }

                    offset = static_cast<std::byte*>(ptr) - rawData;

                    CommandHeader header;
                    std::memcpy(&header, rawData + offset, sizeof(CommandHeader));
                    offset += sizeof(CommandHeader);

                    switch (header.type)
                    {
                        case Pitaya::Render::RenderCommandType::BeginPass:
                            renderer->ExecuteCommand(FetchCommand<Pitaya::Render::BeginPassCommand>(offset));
                            break;

                        case Pitaya::Render::RenderCommandType::InstanceDraw:
                            renderer->ExecuteCommand(FetchCommand<Pitaya::Render::InstancedDrawCommand>(offset));
                            break;

                        case Pitaya::Render::RenderCommandType::PostProcess:
                            renderer->ExecuteCommand(FetchCommand<Pitaya::Render::PostProcessCommand>(offset));
                            break;

                        case Pitaya::Render::RenderCommandType::BlitToScreen:
                            renderer->ExecuteCommand(FetchCommand<Pitaya::Render::BlitToScreenCommand>(offset));
                            break;

                        case Pitaya::Render::RenderCommandType::BeginShadowPass:
                            renderer->ExecuteCommand(FetchCommand<Pitaya::Render::BeginShadowPassCommand>(offset));
                            break;

                        case Pitaya::Render::RenderCommandType::Invalid:
                        default:
                            offset += header.size;
                            break;
                    }
                }
                back.Clear();	//清空缓冲区
                INVOKE_POSTRENDERERPARSECOMMAND_HOOK
            }

        public:
            template <typename T>
            inline void PushCommand(const T& command)
            {
                static_assert(std::is_trivially_copyable<T>::value,
                    "Command must be trivially copyable (POD-like) for raw memory copy!");

                constexpr size_t alignRequirement = alignof(CommandHeader);
                constexpr size_t dataSize = sizeof(CommandHeader) + sizeof(T);

                size_t currentSize = front.CommandBuffer.size();
                size_t maxSpace = currentSize + dataSize + alignRequirement;

                front.CommandBuffer.resize(maxSpace);

                void* ptr = front.CommandBuffer.data() + currentSize;
                size_t space = dataSize + alignRequirement;

                void* alignedPtr = std::align(alignRequirement, dataSize, ptr, space);

                size_t alignedOffset = static_cast<std::byte*>(alignedPtr) - front.CommandBuffer.data();
                front.CommandBuffer.resize(alignedOffset + dataSize);

                std::byte* writePtr = front.CommandBuffer.data() + alignedOffset;

                CommandHeader header = { T::Type, static_cast<uint32_t>(sizeof(T)) };
                std::memcpy(writePtr, &header, sizeof(CommandHeader));
                std::memcpy(writePtr + sizeof(CommandHeader), &command, sizeof(T));
            }

        private:
            template <typename T>
            inline const T* FetchCommand(size_t& offset)
            {
                if (offset + sizeof(T) > back.CommandBuffer.size()) { return nullptr; }
                const T* ptr = reinterpret_cast<const T*>(&back.CommandBuffer[offset]);
                offset += sizeof(T);
                return ptr;
            }

        public:
            inline bool IsRemain() const noexcept
            {
                return !back.CommandBuffer.empty();
            }
            inline void PushDrawCommandToPass(DrawCommand&& cmd)
            {
                if (!cmd.BoneInverseMatrices || cmd.BoneInverseMatrices->empty())
                {
                    staticPass.emplace_back(std::move(cmd));
                }
                else
                {
                    skinnedPass.emplace_back(std::move(cmd));
                }
            }
            inline void CompilePass(Pitaya::Render::Renderer* renderer)
            {
                uint32_t beforeBatch = skinnedPass.size() + staticPass.size();
                if (beforeBatch == 0) { return; }

                uint32_t currentMaterialOffset = 0;
                auto ProcessQueue =
                    [&](std::vector<DrawCommand>& currentPass) -> uint32_t
                    {
                        if (currentPass.empty()) { return 0; }

                        static std::vector<uint32_t> sortedIndices;
                        uint32_t drawCommandCount = currentPass.size();
                        sortedIndices.resize(drawCommandCount);
                        std::iota(sortedIndices.begin(), sortedIndices.end(), 0);

                        std::sort(sortedIndices.begin(), sortedIndices.end(),
                            [&](uint32_t a, uint32_t b) { return currentPass[a].SortKey < currentPass[b].SortKey; });

                        bool isBatching = false;
                        uint32_t drawtimes = 0;
                        InstancedDrawCommand currentBatch;
                        for (uint32_t idx : sortedIndices)
                        {
                            const auto& cmd = currentPass[idx];
                            bool canBatch = isBatching &&
                                cmd.VertexArrayHandle == currentBatch.VertexArrayHandle &&
                                cmd.BaseIndex == currentBatch.BaseIndex &&
                                cmd.BaseVertex == currentBatch.BaseVertex &&
                                cmd.IndexCount == currentBatch.IndexCount &&
                                cmd.MaterialId == currentBatch.MaterialId &&
                                cmd.DepthTest == currentBatch.DepthTest &&
                                cmd.Blend == currentBatch.Blend &&
                                cmd.CullFace == currentBatch.CullFace;

                            if (!canBatch)
                            {
                                if (isBatching) { PushCommand(currentBatch); drawtimes++; }
                                isBatching = true;

                                currentBatch.VertexArrayHandle = cmd.VertexArrayHandle;
                                currentBatch.MaterialId = cmd.MaterialId;
                                currentBatch.IndexCount = cmd.IndexCount;
                                currentBatch.BaseIndex = cmd.BaseIndex;
                                currentBatch.BaseVertex = cmd.BaseVertex;
                                currentBatch.DepthTest = cmd.DepthTest;
                                currentBatch.Blend = cmd.Blend;
                                currentBatch.CullFace = cmd.CullFace;
                                currentBatch.ShaderHandle = cmd.ShaderHandle;
                                currentBatch.InstanceCount = 0;
                                currentBatch.BaseInstance = static_cast<uint32_t>(front.InstanceInfo.size());
                            }

                            uint32_t matByteOffset = 0;
                            if (cmd.MaterialPtr)	// fallback 材质为 null, fallback shader 不使用材质参数
                            {
                                const auto& layout = cmd.MaterialPtr->Shader->ParamLayout;
                                matByteOffset = currentMaterialOffset;
                                currentMaterialOffset += layout.TotalBytes;

                                auto baseSize = front.MaterialParams.size();
                                front.MaterialParams.resize(baseSize + layout.TotalBytes);
                                auto* block = front.MaterialParams.data() + baseSize;

                                Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle tex2Dhandle;  //用于将Index转化为Handle
                                for (const auto& slot : layout.Slots)
                                {
                                    switch (slot.Type)
                                    {
                                        case Pitaya::Asset::ParamType::Vector:
                                            if (slot.Index < cmd.MaterialPtr->VectorParams.size())
                                                { std::memcpy(block + slot.Offset, &cmd.MaterialPtr->VectorParams[slot.Index], slot.Size); }
                                            break;

                                        case Pitaya::Asset::ParamType::Float:
                                            if (slot.Index < cmd.MaterialPtr->FloatParams.size())
                                                { std::memcpy(block + slot.Offset, &cmd.MaterialPtr->FloatParams[slot.Index], slot.Size); }
                                            break;

                                        case Pitaya::Asset::ParamType::Texture:
                                            tex2Dhandle = (slot.Index < cmd.MaterialPtr->Textures.size() && cmd.MaterialPtr->Textures[slot.Index].IsReady() && cmd.MaterialPtr->Textures[slot.Index]->Type == Pitaya::GPU::TextureType::Texture2D) ?
                                                cmd.MaterialPtr->Textures[slot.Index]->Texture2DHandle : renderer->globalRHI.Fallback.TextureHandle;
                                            std::memcpy(block + slot.Offset, &tex2Dhandle, sizeof(tex2Dhandle));
                                            front.MaterialTexturePatches.push_back(matByteOffset + slot.Offset);
                                            break;

                                        default: break;
                                    }
                                }
                            }

                            front.InstanceInfo.push_back({ cmd.ModelMatrix, glm::transpose(glm::inverse(cmd.ModelMatrix)), {cmd.ReceiveShadow, matByteOffset, 0, 0} });

                            if (cmd.BoneInverseMatrices && !cmd.BoneInverseMatrices->empty())
                            {
                                constexpr const size_t MaxBonesPerInstance = 100;
                                size_t bonesToCopy = std::min(cmd.BoneInverseMatrices->size(), MaxBonesPerInstance);
                                size_t paddingBones = MaxBonesPerInstance - bonesToCopy;
                                if (bonesToCopy > 0)
                                {
                                    front.BoneMatrices.insert(
                                        front.BoneMatrices.end(),
                                        cmd.BoneInverseMatrices->begin(),
                                        cmd.BoneInverseMatrices->begin() + bonesToCopy);
                                }
                                if (paddingBones > 0)
                                {
                                    front.BoneMatrices.insert(
                                        front.BoneMatrices.end(),
                                        paddingBones,
                                        glm::mat4(1.0f));
                                }
                            }
                            currentBatch.InstanceCount++;
                        }

                        if (isBatching) { PushCommand(currentBatch); drawtimes++; }
                        currentPass.clear();
                        return drawtimes;
                    };

                // 严格控制调用顺序 先骨骼网格 后静态网格
                uint32_t afterBath = 0;
                afterBath += ProcessQueue(skinnedPass);	 // 先处理骨骼队列
                afterBath += ProcessQueue(staticPass);	 // 后处理无骨骼的静态物体队列

                Core::Print(Core::Color::Red, "[Batch] Before:%d to After:%d", beforeBatch, afterBath);
            }
            inline void SwapBuffer()
            {
                std::swap(front, back);
                INVOKE_POSTRENDERERSWAPBUFFER_HOOK
            }

        private:
            Buffer front;								// 主线程写入渲染命令、实例化Models、骨骼动画
            Buffer back;								// 渲染线程执行渲染命令
            std::vector<DrawCommand> skinnedPass;		// 用于对DrawCommand进行排序
            std::vector<DrawCommand> staticPass;		// 用于对DrawCommand进行排序
        };

    private:
        Renderer() = default;
        ~Renderer() = default;

    public:
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

    private:
        inline bool Initialize(void* nativeWindow)
        {
            // front buffer reserve
            renderPacket.front.CommandBuffer.reserve(64 * 1024);
            renderPacket.front.InstanceInfo.reserve(1024);
            renderPacket.front.BoneMatrices.reserve(1024);
            renderPacket.front.MaterialParams.reserve(4096 * sizeof(float));
            renderPacket.front.MaterialTexturePatches.reserve(512);
            renderPacket.front.Lights.reserve(10);
            renderPacket.front.ShadowSSBOData.reserve(64 * 1024);

            // front buffer reserve
            renderPacket.back.CommandBuffer.reserve(64 * 1024);
            renderPacket.back.InstanceInfo.reserve(1024);
            renderPacket.back.BoneMatrices.reserve(1024);
            renderPacket.back.MaterialParams.reserve(4096 * sizeof(float));
            renderPacket.back.MaterialTexturePatches.reserve(512);
            renderPacket.back.Lights.reserve(10);
            renderPacket.back.ShadowSSBOData.reserve(64 * 1024);

            // skinned comand reserve
            renderPacket.skinnedPass.reserve(1024);

            // static comand reserve
            renderPacket.staticPass.reserve(1024);

            // start render thread
            isRunning.store(true, std::memory_order_release);
            renderThread = Pitaya::Thread::RegisterThread("Render", &Pitaya::Render::Renderer::BootstrapRenderThread, this, nativeWindow);
            if (renderThread == Pitaya::Core::Thread::Identifier::Invalid) { throw std::runtime_error("Render Thread Register Fail!"); }

            // invoke hook func
            INVOKE_POSTRENDERERINTIALIZE_HOOK(nativeWindow)
            return true;
        }
        inline void Release()
        {
            // terminate render thread
            isRunning.store(false, std::memory_order_release);
            cond.notify_one();
            Pitaya::Thread::UnregisterThread(renderThread);

            // clear render packet
            renderPacket.front.Clear();
            renderPacket.back.Clear();
            renderPacket.skinnedPass.clear();
            renderPacket.staticPass.clear();

            //invoke hook func
            INVOKE_POSTRENDERERRELEASE_HOOK
        }

    private:
        bool InitializeRenderContext(void* nativeWindow);
        void ReleaseRenderContext();

    private:
        inline void RenderThread(void* nativeWindow)
        {
            InitializeRenderContext(nativeWindow);
            globalRHI.Create(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
            INVOKE_POSTRENDERCONTEXTINITIALIZED_HOOK(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), globalRHI.MainDisplayRenderTarget.FinalFrameBufferHandle)

            while (true)
            {
                std::unique_lock<std::mutex> lock(mutex);
                cond.wait(lock, [this] { return renderPacket.IsRemain() || INVOKE_SHOULDWAKEUPRENDERTHREAD_HOOK ||
                    Pitaya::Asset::IsUploadedToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer>()) || !isRunning.load(std::memory_order_acquire); });
                if (!isRunning.load(std::memory_order_acquire)) { break; }

                Pitaya::Asset::SyncAssetToGPU(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
                if (renderPacket.IsRemain() || INVOKE_SHOULDWAKEUPRENDERTHREAD_HOOK)
                {
                    NewRenderFrame();
                    renderPacket.ParseCommand(this);
                    SwapBuffer();
                }
            }

            INVOKE_PRERENDERCONTEXTINRELEASED_HOOK
            Pitaya::GPU::DestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
            ReleaseRenderContext();
        }

    private:
        void NewRenderFrame();
        void SwapBuffer() const;

    private:
        void ExecuteCommand(const Pitaya::Render::BeginPassCommand* command) const;
        void ExecuteCommand(const Pitaya::Render::InstancedDrawCommand* command) const;
        void ExecuteCommand(const Pitaya::Render::PostProcessCommand* command) const;
        void ExecuteCommand(const Pitaya::Render::BlitToScreenCommand* command) const;
        void ExecuteCommand(const Pitaya::Render::BeginShadowPassCommand* command) const;

    public:
        inline void BeginRenderFrame(Pitaya::Core::PassKey<RenderPipeline>)
        {
            renderPacket.front.Clear();
            INVOKE_POSTRENDERERBEGINRENDERFRAME_HOOK
        }
        inline void SubmitLightShadow(Pitaya::Core::PassKey<RenderPipeline>, const std::vector<Pitaya::Render::ShadowCasterSlice>& slices, const std::vector<glm::mat4>& matrices, const std::vector<Pitaya::Render::CascadeSplitInfo>& cascadeSplits, uint32_t dirCount, uint32_t spotCount, uint32_t pointCount)
        {
            // 计算各 atlas 所需的最大 layer 数
            uint32_t maxCSMLayer = 0, maxSpotLayer = 0, maxPointLayer = 0;
            for (const auto& s : slices)
            {
                uint32_t endLayer = s.LayerOffset + s.MatrixCount;
                switch (s.LightType)
                {
                    case 0: maxCSMLayer = std::max(maxCSMLayer, endLayer);   break;
                    case 2: maxSpotLayer = std::max(maxSpotLayer, endLayer);  break;
                    case 1: maxPointLayer = std::max(maxPointLayer, endLayer); break;
                }
            }
            renderPacket.front.RequiredCSMLayers = maxCSMLayer;
            renderPacket.front.RequiredSpotLayers = maxSpotLayer;
            renderPacket.front.RequiredPointLayers = maxPointLayer;

            // 序列化 SSBO 数据
            // 布局: Header | CascadeSplit[] | SliceGPU[] | mat4[]
            size_t headerSize = sizeof(Pitaya::Render::ShadowSSBOHeader);
            size_t splitsSize = cascadeSplits.size() * sizeof(Pitaya::Render::CascadeSplitInfo);
            size_t slicesSize = slices.size() * sizeof(Pitaya::Render::ShadowSliceGPU);
            size_t matricesSize = matrices.size() * sizeof(glm::mat4);
            size_t totalSize = headerSize + splitsSize + slicesSize + matricesSize;

            if (totalSize > 0)
            {
                auto& shadowData = renderPacket.front.ShadowSSBOData;
                shadowData.resize(totalSize);

                Pitaya::Render::ShadowSSBOHeader header;
                header.DirectionalLightCount = dirCount;
                header.SpotLightCount = spotCount;
                header.PointLightCount = pointCount;
                header.TotalMatrixCount = static_cast<uint32_t>(matrices.size());
                header.CascadeSplitCount = static_cast<uint32_t>(cascadeSplits.size());
                header.ShadowSliceCount = static_cast<uint32_t>(slices.size());
                std::memcpy(shadowData.data(), &header, headerSize);

                if (splitsSize > 0)
                {
                    std::memcpy(shadowData.data() + headerSize, cascadeSplits.data(), splitsSize);
                }

                if (slicesSize > 0)
                {
                    std::vector<Pitaya::Render::ShadowSliceGPU> gpuSlices(slices.size());
                    for (size_t i = 0; i < slices.size(); ++i)
                    {
                        gpuSlices[i].MatrixOffset = slices[i].MatrixOffset;
                        gpuSlices[i].LayerOffset = slices[i].LayerOffset;
                        gpuSlices[i].LightType = slices[i].LightType;
                        gpuSlices[i]._pad0 = 0;
                    }
                    std::memcpy(shadowData.data() + headerSize + splitsSize, gpuSlices.data(), slicesSize);
                }

                if (matricesSize > 0)
                {
                    std::memcpy(shadowData.data() + headerSize + splitsSize + slicesSize, matrices.data(), matricesSize);
                }
            }
        }
        inline void BeginShadowPass(Pitaya::Core::PassKey<RenderPipeline>, uint32_t lightType, uint32_t layer, const glm::mat4& shadowVP)
        {
            Pitaya::Render::BeginShadowPassCommand cmd;
            cmd.LightType = lightType;
            cmd.Layer = layer;
            cmd.ShadowViewProjection = shadowVP;

            switch (lightType)
            {
                case 0: cmd.Resolution = GlobalRHI::ShadowAtlas::CSMResolution;   break;
                case 2: cmd.Resolution = GlobalRHI::ShadowAtlas::SpotResolution;  break;
                case 1: cmd.Resolution = GlobalRHI::ShadowAtlas::PointResolution; break;
            }

            renderPacket.PushCommand(cmd);
        }
        inline void SubmitCastShadowItem(Pitaya::Core::PassKey<RenderPipeline>, const RenderItem& item)
        {
            if (!item.Mesh || item.SubMeshIndex >= item.Mesh->SubMeshs.size()) { return; }

            auto& subMesh = item.Mesh->SubMeshs[item.SubMeshIndex];

            Pitaya::Render::DrawCommand cmd;
            cmd.ModelMatrix = item.Model;
            cmd.VertexArrayHandle = item.Mesh->VertexArrayHandle;
            cmd.IndexCount = subMesh.IndexCount;
            cmd.BaseIndex = subMesh.BaseIndex;
            cmd.BaseVertex = subMesh.BaseVertex;
            cmd.BoneInverseMatrices = &item.Mesh->BoneInverseMatrices;

            bool hasBones = cmd.BoneInverseMatrices && !cmd.BoneInverseMatrices->empty();
            cmd.ShaderHandle = hasBones
                ? globalRHI.Specific.DepthOnlySkinnedShaderHandle
                : globalRHI.Specific.DepthOnlyStaticShaderHandle;

            cmd.MaterialId = 0;
            cmd.DepthTest = true;
            cmd.Blend = false;
            cmd.CullFace = true;
            cmd.SortKey = 0;

            renderPacket.PushDrawCommandToPass(std::move(cmd));
        }
        inline void EndShadowPass(Pitaya::Core::PassKey<RenderPipeline>)
        {
            renderPacket.CompilePass(this);
        }
        inline void BeginPass(Pitaya::Core::PassKey<RenderPipeline>, const RenderPass& pass)
        {
            Pitaya::Render::BeginPassCommand beginPassCommand;
            beginPassCommand.CameraSnapshot = pass.CameraSnapshot;
            if (pass.RenderTarget)
            {
                beginPassCommand.SceneFrameBufferHandle = pass.RenderTarget->SceneFrameBufferHandle;
                beginPassCommand.ClearColor = pass.RenderTarget->ClearColor;
                beginPassCommand.Rect = { {0.0f, 0.0f}, { pass.RenderTarget->SceneFrameBufferSpecification.Width, pass.RenderTarget->SceneFrameBufferSpecification.Height } };
                beginPassCommand.ClearDepth = pass.RenderTarget->ClearDepth;
                beginPassCommand.ClearStencil = pass.RenderTarget->ClearStencil;
            }
            else
            {
                Pitaya::GPU::FrameBufferSpecification mainSceneSpec = Pitaya::Config::GetMainSceneSpec();
                beginPassCommand.SceneFrameBufferHandle = globalRHI.MainDisplayRenderTarget.SceneFrameBufferHandle;
                beginPassCommand.ClearColor = Pitaya::Core::Color::SkyBlue;
                beginPassCommand.Rect = { {0.0f, 0.0f}, { mainSceneSpec.Width, mainSceneSpec.Height } };
                beginPassCommand.ClearDepth = true;
                beginPassCommand.ClearStencil = true;
            }
            size_t currentTotal = renderPacket.front.Lights.size();
            beginPassCommand.LightCount = pass.LightCount;
            beginPassCommand.LightDataOffset = (currentTotal >= pass.LightCount)
                ? static_cast<uint32_t>(currentTotal - pass.LightCount) : 0;
            renderPacket.PushCommand(beginPassCommand);
        }
        inline void Submit(Pitaya::Core::PassKey<RenderPipeline>, const RenderItem& item)
        {
            auto* mesh = item.Mesh;
            auto* material = item.Material;
            uint32_t submeshIndex = item.SubMeshIndex;

            Pitaya::Render::DrawCommand cmd;
            cmd.ReceiveShadow = item.ReceiveShadow;
            cmd.ModelMatrix = item.Model;
            if (mesh && mesh->VertexArrayHandle != Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle::Invalid &&
                submeshIndex < mesh->SubMeshs.size())
            {
                cmd.VertexArrayHandle = mesh->VertexArrayHandle;
                cmd.IndexCount = mesh->SubMeshs[submeshIndex].IndexCount;
                cmd.BaseIndex = mesh->SubMeshs[submeshIndex].BaseIndex;
                cmd.BaseVertex = mesh->SubMeshs[submeshIndex].BaseVertex;
                cmd.BoneInverseMatrices = &mesh->BoneInverseMatrices;
            }
            else
            {
                //Mesh 异常 → fallback 立方体
                cmd.VertexArrayHandle = globalRHI.Fallback.VAOHandle;
                cmd.IndexCount = 36;
                cmd.BaseIndex = 0;
                cmd.BaseVertex = 0;
                cmd.BoneInverseMatrices = nullptr;	//异常情况无需骨骼数据
            }

            auto GenerateSortKey =	//[Queue][Order][Shader][Material][Mesh][Depth]
                [](RenderQueue renderQueue, int32_t drawOrder, uint32_t shaderID, uint32_t materialID, uint32_t meshID, float depth) ->uint64_t
                {
                    constexpr const uint64_t MaxSortKey = 0xFFFFFFFFFFFFFFFF;
                    constexpr const uint64_t MinSortKey = 0;

                    constexpr const uint64_t RENDER_QUEUE_SHIFT = 60;
                    constexpr const uint64_t DRAW_ORDER_SHIFT = 52;
                    constexpr const uint64_t SHADER_ID_SHIFT = 42;
                    constexpr const uint64_t MATERIAL_ID_SHIFT = 30;
                    constexpr const uint64_t MESH_ID_SHIFT = 16;
                    constexpr const uint64_t DEPTH_SHIFT = 0;

                    uint64_t key = 0;

                    // Queue (Mask 0xF, 4 bits) 
                    key |= (static_cast<uint64_t>(renderQueue) & 0xF) << RENDER_QUEUE_SHIFT;

                    // DrawOrder (Mask 0xFF, 8 bits) 
                    uint32_t orderVal = static_cast<uint32_t>(drawOrder + 128);
                    key |= (static_cast<uint64_t>(orderVal) & 0xFF) << DRAW_ORDER_SHIFT;

                    // Shader (Mask 0x3FF -> 1023) 
                    key |= (static_cast<uint64_t>(shaderID) & 0x3FF) << SHADER_ID_SHIFT;     // 限制：同屏使用的不同 Shader 类型不能超过 1024 个

                    // Material (Mask 0xFFF -> 4095) 
                    key |= (static_cast<uint64_t>(materialID) & 0xFFF) << MATERIAL_ID_SHIFT;     // 限制：同屏使用的不同材质数量不能超过 4096 个

                    // Mesh (Mask 0x3FFF -> 16383) 
                    key |= (static_cast<uint64_t>(meshID) & 0x3FFF) << MESH_ID_SHIFT;   // 限制：同屏使用的不同网格数量不能超过 16384 个

                    // Depth (Mask 0xFFFF, 16 bits) 
                    float normalizedDepth = std::clamp((depth + 1.0f) * 0.5f, 0.0f, 1.0f);

                    uint32_t depthInt = (renderQueue <= RenderQueue::Geometry) ?
                        static_cast<uint32_t>(normalizedDepth * 0xFFFF) :           // 不透明物体：从前往后画 (Front-to-Back)，利用 Early-Z 剔除
                        static_cast<uint32_t>((1.0f - normalizedDepth) * 0xFFFF);   // 半透明物体：从后往前画 (Back-to-Front)，保证混合正确

                    key |= (static_cast<uint64_t>(depthInt) & 0xFFFF) << DEPTH_SHIFT;

                    return key;
                };

            if (material && material->Shader.IsReady() &&
                material->Shader->ShaderHandle != Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle::Invalid)
            {
                cmd.ShaderHandle = material->Shader->ShaderHandle;
                cmd.MaterialId = material->SortId;
                cmd.MaterialPtr = material;
                cmd.DepthTest = material->DepthTest;
                cmd.Blend = material->Blend;
                cmd.CullFace = material->CullFace;
                cmd.SortKey = GenerateSortKey(
                    material->RenderQueue,
                    material->DrawOrder,
                    cmd.ShaderHandle.Index(),
                    cmd.MaterialId,
                    (cmd.VertexArrayHandle.Index() << 8) + submeshIndex,
                    0);
            }
            else
            {
                cmd.ShaderHandle = globalRHI.Fallback.ShaderHandle;
                cmd.MaterialId = 0;
                cmd.MaterialPtr = nullptr;
                cmd.DepthTest = true;
                cmd.Blend = false;
                cmd.CullFace = true;
                cmd.SortKey = GenerateSortKey(
                    Pitaya::Render::RenderQueue::Geometry,
                    0,
                    cmd.ShaderHandle.Index(),
                    0,
                    (cmd.VertexArrayHandle.Index() << 8) + submeshIndex,
                    0);
            }

            renderPacket.PushDrawCommandToPass(std::move(cmd));
        }
        inline void EndPass(Pitaya::Core::PassKey<RenderPipeline>)
        {
            renderPacket.CompilePass(this);
        }
        inline void SubmitPostProcess(Pitaya::Core::PassKey<RenderPipeline>, const RenderPass& pass)
        {
            bool firstPass = true;
            uint32_t pingpongIndex = 0;

            auto mainSceneSpec = Pitaya::Config::GetMainSceneSpec();
            auto sceneFboHandle = pass.RenderTarget ? pass.RenderTarget->SceneFrameBufferHandle : globalRHI.MainDisplayRenderTarget.SceneFrameBufferHandle;
            auto finalFboHandle = pass.RenderTarget ? pass.RenderTarget->FinalFrameBufferHandle : globalRHI.MainDisplayRenderTarget.FinalFrameBufferHandle;
            auto isMultisample = pass.RenderTarget ? pass.RenderTarget->SceneFrameBufferSpecification.Samples > 1 : mainSceneSpec.Samples > 1;
            auto size = pass.RenderTarget ? glm::uvec2(pass.RenderTarget->SceneFrameBufferSpecification.Width, pass.RenderTarget->SceneFrameBufferSpecification.Height) :
                glm::uvec2(mainSceneSpec.Width, mainSceneSpec.Height);
            auto currentReadFBOHandle = sceneFboHandle;

            // 迭代处理后处理步骤 Ping-Pong 
            for (uint32_t i = 0; i < pass.PostProcessSetting.StepCount; i++)
            {
                auto& currentStep = pass.PostProcessSetting.Steps[i];
                auto targetPingPongFboHandle = pass.RenderTarget ? pass.RenderTarget->PingPongFrameBufferHandles[pingpongIndex] : globalRHI.MainDisplayRenderTarget.PingPongFrameBufferHandles[pingpongIndex];

                PostProcessCommand cmd;
                cmd.PostProcessStep = currentStep;
                switch (currentStep.Type)
                {
                    case Pitaya::Render::PostProcessType::Bilt:
                        cmd.ProcessShaderHandle = globalRHI.PostProcessShader.BlitShaderHandle;
                        break;

                    case Pitaya::Render::PostProcessType::GammaCorrection:
                        cmd.ProcessShaderHandle = globalRHI.PostProcessShader.GammaCorrectionShaderHandle;
                        break;

                    case Pitaya::Render::PostProcessType::Invalid:
                    default:
                        cmd.ProcessShaderHandle = globalRHI.PostProcessShader.BlitShaderHandle;
                        break;
                }

                // 处理 MSAA 解析逻辑
                if (firstPass && isMultisample)
                {
                    cmd.ResolveMSAA = true;
                    cmd.ResolveSize = size;
                    cmd.ResolveFrameBufferHandle = currentReadFBOHandle;
                }

                cmd.ReadFrameBufferHandle = currentReadFBOHandle;
                cmd.WriteFrameBufferHandle = (i == pass.PostProcessSetting.StepCount - 1) ? finalFboHandle : targetPingPongFboHandle;
                renderPacket.PushCommand(cmd);

                // 更新状态流
                currentReadFBOHandle = cmd.WriteFrameBufferHandle;
                pingpongIndex = 1 - pingpongIndex;
                firstPass = false;

                Pitaya::Core::Print(Pitaya::Core::Color::Purple, "Post Process Submitted: %s", Pitaya::Render::ToString(currentStep.Type).data());
            }

            // Bypass 若无后处理 则直接将源 FBO Blit 到最终 FBO
            if (firstPass)
            {
                PostProcessCommand cmd;
                cmd.ProcessShaderHandle = globalRHI.PostProcessShader.BlitShaderHandle;
                cmd.ReadFrameBufferHandle = currentReadFBOHandle;
                cmd.WriteFrameBufferHandle = finalFboHandle;

                if (isMultisample)
                {
                    cmd.ResolveMSAA = true;
                    cmd.ResolveSize = size;
                    cmd.ResolveFrameBufferHandle = currentReadFBOHandle;
                }

                renderPacket.PushCommand(cmd);
                Pitaya::Core::Print(Pitaya::Core::Color::Purple, "Post Process: Bypass - Direct Blit to Final (Resolve: %s)", isMultisample ? "True" : "False");
            }
        }
        inline void SubmitLight(Pitaya::Core::PassKey<RenderPipeline>, const LightInfo& light)
        {
            renderPacket.front.Lights.emplace_back(light);
        }
        inline void SubmitBlitToScreen(Pitaya::Core::PassKey<RenderPipeline>)
        {
            Pitaya::Render::BlitToScreenCommand blitToScreenCommand{ Pitaya::Window::GetWindowSize() };
            renderPacket.PushCommand(blitToScreenCommand);
        }
        inline void EndRenderFrame(Pitaya::Core::PassKey<RenderPipeline>)
        {
            INVOKE_PRERENDERERENDRENDERFRAME_HOOK

            //交换渲染缓冲区
            {
                std::lock_guard<std::mutex> lock(mutex);
                renderPacket.SwapBuffer();
            }

            //唤醒渲染线程工作
            cond.notify_one();
        }

    private:
        inline static void BootstrapRenderThread(void* renderer, void* nativeWindow)
        {
            static_cast<Pitaya::Render::Renderer*>(renderer)->RenderThread(nativeWindow);
        }

    private:
        GlobalRHI globalRHI;
        RenderPacket renderPacket;

        std::mutex mutex;
        std::condition_variable cond;
        std::atomic<bool> isRunning = false;
        Pitaya::Core::Thread::Identifier renderThread;
        Pitaya::Core::Storage<8> backendStorage;
    };
}


/*
                方法1
                float CalculateSortDepth_Planar(glm::vec3 camPos, glm::vec3 camForward,
                                        glm::vec3 objPos, float farClipPlane)
                {
                    glm::vec3 toObject = objPos - camPos;
                    float viewDepth = glm::dot(toObject, camForward);
                    float normalizedDepth = viewDepth / farClipPlane;
                    return std::clamp(normalizedDepth, 0.0f, 1.0f);
                }
                */