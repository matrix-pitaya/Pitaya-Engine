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
#include<Render/Common/LightInfo.h>
#include<Render/Common/InstanceTransformInfo.h>
#include<Render/Command/BeginPassCommand.h>
#include<Render/Command/DrawCommand.h>
#include<Render/Command/InstancedDrawCommand.h>
#include<Render/Command/BlitToScreenCommand.h>
#include<Render/Command/PostProcessCommand.h>
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

	private:
		struct GlobalRHI
		{
			//Specific
			Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle EmptyVAOHandle;

			//Fallback
			Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle FallbackVAOHandle;
			Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle FallbackShaderHandle;
			Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle FallbackTextureHandle;

			//PostProcess Shader
			Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle BlitShaderHandle;
			Pitaya::Core::SlotMap<Pitaya::GPU::Shader>::Handle GammaCorrectionShaderHandle;

			//Uniform Buffer
			Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle CameraSnapshotUBOHandle;
			Pitaya::Core::SlotMap<Pitaya::GPU::UniformBuffer>::Handle PostProcessUBOHandle;

			//ShaderStorageBuffer
			Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle InstanceModelTransformSSBOHandle;
			Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle BoneInverseMatriceSSBOHandle;
			Pitaya::Core::SlotMap<Pitaya::GPU::ShaderStorageBuffer>::Handle SceneLightsSSBOHandle;
			size_t TransformSSBOCapacity = 0;	//记录当前显存缓冲区的大小
			size_t BoneSSBOCapacity = 0;
			size_t LightSSBOCapacity = 0;

			//MainDisplayRT
			Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle MainSceneFrameBufferHandle;
			Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle MainPingPongFrameBufferHandles[2] = { };
			Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle MainFinalFrameBufferHandle;

			inline void Create()
			{
				EmptyVAOHandle = Pitaya::GPU::CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
				CameraSnapshotUBOHandle = Pitaya::GPU::CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					sizeof(Pitaya::Core::CameraSnapshot), static_cast<uint32_t>(Pitaya::GPU::UBOBindPoint::CameraSnapshot));
				PostProcessUBOHandle = Pitaya::GPU::CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					Pitaya::Render::PostProcessStep::UniformBufferBytes, static_cast<uint32_t>(Pitaya::GPU::UBOBindPoint::PostProcessUBO));
				
				// 初始分配1024个位置
				TransformSSBOCapacity = 1024 * sizeof(InstanceTransformInfo);
				InstanceModelTransformSSBOHandle = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					TransformSSBOCapacity, static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform));
				
				// 初始分配一段骨骼容量
				BoneSSBOCapacity = 4096 * sizeof(glm::mat4);	
				BoneInverseMatriceSSBOHandle = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					BoneSSBOCapacity, static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice));
				
				// 初始分配10个光源位置
				LightSSBOCapacity = 10 * sizeof(Pitaya::Render::LightInfo);
				SceneLightsSSBOHandle = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					LightSSBOCapacity, static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::SceneLights));

				BlitShaderHandle = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					Pitaya::Core::LoadBuiltInRC(IDR_BLIT_VERTEX_SHADER).c_str(),
					Pitaya::Core::LoadBuiltInRC(IDR_BLIT_FRAGMENT_SHADER).c_str());
				GammaCorrectionShaderHandle = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					Pitaya::Core::LoadBuiltInRC(IDR_GAMMA_CORRECTION_VERTEX_SHADER).c_str(),
					Pitaya::Core::LoadBuiltInRC(IDR_GAMMA_CORRECTION_FRAGMENT_SHADER).c_str());

				FallbackVAOHandle = Pitaya::GPU::CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
				std::string fallbackVboData = Pitaya::Core::LoadBuiltInRC(IDR_ERROR_VERTICES);
				std::string fallbackIboData = Pitaya::Core::LoadBuiltInRC(IDR_ERROR_INDICES);
				auto fallbackVBOHandle = Pitaya::GPU::CreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					reinterpret_cast<float*>(fallbackVboData.data()), fallbackVboData.size(), { { Pitaya::GPU::ShaderVariableType::Float3, 0 } });
				auto fallbackIBOHandle = Pitaya::GPU::CreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					reinterpret_cast<uint32_t*>(fallbackIboData.data()), 36);
				if (!Pitaya::GPU::LinkVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), 
					FallbackVAOHandle, fallbackVBOHandle, fallbackIBOHandle))
				{
					Pitaya::Core::PopMessageBox("Error", "Create Global RHI Failed! Check Log for Details.");
					Pitaya::Core::Terminate(-1);
				}
				
				FallbackTextureHandle = Pitaya::GPU::CreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					reinterpret_cast<unsigned char*>(Pitaya::Core::LoadBuiltInRC(IDR_ERROR_TEXTURE).data()),
					32, 32, 4, false, false, true);
				FallbackShaderHandle = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					Pitaya::Core::LoadBuiltInRC(IDR_ERROR_VERTEX_SHADER).c_str(),
					Pitaya::Core::LoadBuiltInRC(IDR_ERROR_FRAGMENT_SHADER).c_str());

				//MainRT
				Pitaya::GPU::FrameBufferSpecification mainSceneSpec = Pitaya::Config::GetMainSceneSpec();
				Pitaya::GPU::FrameBufferSpecification mainPingPongSpec = Pitaya::Config::GetMainPingPongSpec();
				Pitaya::GPU::FrameBufferSpecification mainFinalSpec = Pitaya::Config::GetMainFinalSpec();
				MainSceneFrameBufferHandle = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainSceneSpec);
				MainPingPongFrameBufferHandles[0] = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainPingPongSpec);
				MainPingPongFrameBufferHandles[1] = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainPingPongSpec);
				MainFinalFrameBufferHandle = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainFinalSpec);
			}
		};
		class RenderPacket
		{
			friend class Renderer;
		public:
			struct Buffer
			{
				std::vector<std::byte> CommandBuffer;
				std::vector<InstanceTransformInfo> InstanceModelTransforms;
				std::vector<glm::mat4> BoneMatrices;
				std::vector<LightInfo> Lights;

				inline void Clear() noexcept
				{
					CommandBuffer.clear();
					CommandBuffer.reserve(64 * 1024);

					InstanceModelTransforms.clear();
					InstanceModelTransforms.reserve(1024);

					BoneMatrices.clear();
					BoneMatrices.reserve(1024);

					Lights.clear();
					Lights.reserve(10);
				}
			};

		private:
			struct CommandHeader
			{
				RenderCommandType type = RenderCommandType::Unknown;
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

						case Pitaya::Render::RenderCommandType::Unknown:
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
				if (cmd.BoneInverseMatrices.empty())
				{
					staticPass.emplace_back(std::move(cmd));
				}
				else
				{
					skinnedPass.emplace_back(std::move(cmd));
				}
			}
			inline void CompilePass()
			{
				uint32_t beforeBatch = skinnedPass.size() + staticPass.size();
				if (beforeBatch == 0) { return; }

				// 处理命令队列
				auto ProcessQueue = 
					[this](std::vector<DrawCommand>& currentPass, bool isSkinnedBatch) -> uint32_t
					{
						if (currentPass.empty()) { return 0; }

						// 通过索引间接排序DrawcallCommand
						static std::vector<uint32_t> sortedIndices;
						uint32_t drawCommandCount = currentPass.size();
						sortedIndices.resize(drawCommandCount);
						std::iota(sortedIndices.begin(), sortedIndices.end(), 0);

						// 通过SortKey对DrawComamnd进行排序
						std::sort(sortedIndices.begin(), sortedIndices.end(),
							[&](uint32_t a, uint32_t b) { return currentPass[a].SortKey < currentPass[b].SortKey; });

						// 合批处理
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

								// 复制状态
								currentBatch.VertexArrayHandle = cmd.VertexArrayHandle;
								currentBatch.MaterialId = cmd.MaterialId;
								currentBatch.IndexCount = cmd.IndexCount;
								currentBatch.BaseIndex = cmd.BaseIndex;
								currentBatch.BaseVertex = cmd.BaseVertex;
								currentBatch.DepthTest = cmd.DepthTest;
								currentBatch.Blend = cmd.Blend;
								currentBatch.CullFace = cmd.CullFace;
								currentBatch.ShaderHandle = cmd.ShaderHandle;
								for (uint32_t i = 0; i < static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Unknown); i++)
								{
									currentBatch.TextureHandles[i] = cmd.TextureHandles[i];
								}

								currentBatch.InstanceCount = 0;
								currentBatch.BaseInstance = static_cast<uint32_t>(front.InstanceModelTransforms.size()); // 不论是哪个队列 BaseInstance 永远递增
							}

							// Transform SSBO
							front.InstanceModelTransforms.push_back({ cmd.ModelMatrix ,glm::transpose(glm::inverse(cmd.ModelMatrix)) });

							// 只有蒙皮渲染队列才去处理骨骼
							if (isSkinnedBatch)
							{
								constexpr const size_t MaxBonesPerInstance = 100;
								size_t bonesToCopy = std::min(cmd.BoneInverseMatrices.size(), MaxBonesPerInstance);
								size_t paddingBones = MaxBonesPerInstance - bonesToCopy;
								if (bonesToCopy > 0)
								{
									front.BoneMatrices.insert(
										front.BoneMatrices.end(),
										cmd.BoneInverseMatrices.begin(),
										cmd.BoneInverseMatrices.begin() + bonesToCopy);
								}
								if (paddingBones > 0)	// 不足补1单元矩阵
								{
									front.BoneMatrices.insert(
										front.BoneMatrices.end(),
										paddingBones,
										glm::mat4(1.0f));
								}
							}
							currentBatch.InstanceCount++;
						}

						// 提交当前队列最后一个批次
						if (isBatching) { PushCommand(currentBatch); drawtimes++; }
						currentPass.clear();
						return drawtimes;
					};

				// 严格控制调用顺序 先骨骼网格 后静态网格
				uint32_t afterBath = 0;
				afterBath += ProcessQueue(skinnedPass, true);   // 先处理骨骼队列
				afterBath += ProcessQueue(staticPass, false);   // 后处理无骨骼的静态物体队列

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
			isRunning.store(true, std::memory_order_release);
			renderThread = Pitaya::Thread::RegisterThread("Render", &Pitaya::Render::Renderer::BootstrapRenderThread, this, nativeWindow);
			if (renderThread == Pitaya::Core::Thread::Identifier::Invalid) { throw std::runtime_error("Render Thread Register Fail!"); }
			INVOKE_POSTRENDERERINTIALIZE_HOOK(nativeWindow)
			return true;
		}
		inline void Release()
		{
			isRunning.store(false, std::memory_order_release);
			cond.notify_one();
			Pitaya::Thread::UnregisterThread(renderThread);
			INVOKE_POSTRENDERERRELEASE_HOOK
		}

	private:
		bool InitializeRenderContext(void* nativeWindow);
		void ReleaseRenderContext();

	private:
		inline void RenderThread(void* nativeWindow)
		{
			InitializeRenderContext(nativeWindow);
			globalRHI.Create();
			INVOKE_POSTRENDERCONTEXTINITIALIZED_HOOK(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), globalRHI.MainFinalFrameBufferHandle)

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

	public:
		inline void BeginRenderFrame(Pitaya::Core::PassKey<RenderPipeline>)
		{
			renderPacket.front.Clear(); 
			INVOKE_POSTRENDERERBEGINRENDERFRAME_HOOK
		}
		inline void BeginPass(Pitaya::Core::PassKey<RenderPipeline>, RenderPass& pass)
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
				beginPassCommand.SceneFrameBufferHandle = globalRHI.MainSceneFrameBufferHandle;
				beginPassCommand.ClearColor = Pitaya::Core::Color::SkyBlue;
				beginPassCommand.Rect = { {0.0f, 0.0f}, { mainSceneSpec.Width, mainSceneSpec.Height } };
				beginPassCommand.ClearDepth = true;
				beginPassCommand.ClearStencil = true;
			}
			renderPacket.PushCommand(beginPassCommand);
		}
		inline void Submit(Pitaya::Core::PassKey<RenderPipeline>, RenderItem& item)
		{
			auto* mesh = item.Mesh;
			auto* material = item.Material;
			uint32_t submeshIndex = item.SubMeshIndex;

			Pitaya::Render::DrawCommand cmd;
			cmd.ModelMatrix = item.Model;
			if (mesh && mesh->VertexArrayHandle != Pitaya::Core::SlotMap<Pitaya::GPU::VertexArray>::Handle::Invalid &&
				submeshIndex < mesh->SubMeshs.size())
			{
				cmd.VertexArrayHandle = mesh->VertexArrayHandle;
				cmd.IndexCount = mesh->SubMeshs[submeshIndex].IndexCount;
				cmd.BaseIndex = mesh->SubMeshs[submeshIndex].BaseIndex;
				cmd.BaseVertex = mesh->SubMeshs[submeshIndex].BaseVertex;
				cmd.BoneInverseMatrices = mesh->BoneInverseMatrices;
			}
			else
			{
				//Mesh 异常 → fallback 立方体
				cmd.VertexArrayHandle = globalRHI.FallbackVAOHandle;
				cmd.IndexCount = 36;
				cmd.BaseIndex = 0;
				cmd.BaseVertex = 0;
				//异常情况无需骨骼数据
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

				//纹理绑定
				for (uint32_t j = 0; j < static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Unknown); j++)
				{
					//无纹理
					if (!material->Textures[j]) { continue; }

					//纹理就绪
					if (material->Textures[j].IsReady() && 
						material->Textures[j]->Type == Pitaya::GPU::TextureType::Texture2D && 
						material->Textures[j]->Texture2DHandle != Pitaya::Core::SlotMap<Pitaya::GPU::Texture2D>::Handle::Invalid)
					{
						cmd.TextureHandles[j] = material->Textures[j]->Texture2DHandle;
					}
					else
					{
						//纹理未就绪
						cmd.TextureHandles[j] = globalRHI.FallbackTextureHandle;
					}
				}
			}
			else
			{
				//Material/Shader 异常 → fallback shader + texture
				cmd.ShaderHandle = globalRHI.FallbackShaderHandle;
				cmd.MaterialId = 0;
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

				//异常Shader只会使用这一个纹理
				cmd.TextureHandles[static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Albedo)] = globalRHI.FallbackTextureHandle;
			}

			renderPacket.PushDrawCommandToPass(std::move(cmd));
		}
		inline void EndPass(Pitaya::Core::PassKey<RenderPipeline>)
		{
			renderPacket.CompilePass();
		}
		inline void SubmitPostProcess(Pitaya::Core::PassKey<RenderPipeline>, RenderPass& pass)
		{
			bool firstPass = true;
			uint32_t pingpongIndex = 0;

			auto mainSceneSpec = Pitaya::Config::GetMainSceneSpec();
			auto sceneFboHandle = pass.RenderTarget ? pass.RenderTarget->SceneFrameBufferHandle : globalRHI.MainSceneFrameBufferHandle;
			auto finalFboHandle = pass.RenderTarget ? pass.RenderTarget->FinalFrameBufferHandle : globalRHI.MainFinalFrameBufferHandle;
			auto isMultisample = pass.RenderTarget ? pass.RenderTarget->SceneFrameBufferSpecification.Samples > 1 : mainSceneSpec.Samples > 1;
			auto size = pass.RenderTarget ? glm::uvec2(pass.RenderTarget->SceneFrameBufferSpecification.Width, pass.RenderTarget->SceneFrameBufferSpecification.Height) :
				glm::uvec2(mainSceneSpec.Width, mainSceneSpec.Height);
			auto currentReadFBOHandle = sceneFboHandle;

			// 迭代处理后处理步骤 Ping-Pong 
			for (uint32_t i = 0; i < pass.PostProcessSetting.StepCount; i++)
			{
				auto& currentStep = pass.PostProcessSetting.Steps[i];
				auto targetPingPongFboHandle = pass.RenderTarget ? pass.RenderTarget->PingPongFrameBufferHandles[pingpongIndex] : globalRHI.MainPingPongFrameBufferHandles[pingpongIndex];

				PostProcessCommand cmd;
				cmd.PostProcessStep = currentStep;
				switch (currentStep.Type)
				{
					case Pitaya::Render::PostProcessType::Bilt:
						cmd.ProcessShaderHandle = globalRHI.BlitShaderHandle;
						break;

					case Pitaya::Render::PostProcessType::GammaCorrection:
						cmd.ProcessShaderHandle = globalRHI.GammaCorrectionShaderHandle;
						break;

					case Pitaya::Render::PostProcessType::Unknown:
					default:
						cmd.ProcessShaderHandle = globalRHI.BlitShaderHandle;
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
				cmd.ProcessShaderHandle = globalRHI.BlitShaderHandle;
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
		inline void SubmitLight(Pitaya::Core::PassKey<RenderPipeline>, LightInfo& light)
		{
			renderPacket.front.Lights.emplace_back(light);
		}
		inline void SubmitBlitToScreen(Pitaya::Core::PassKey<RenderPipeline>)
		{
			Pitaya::Render::BlitToScreenCommand blitToScreenCommand { Pitaya::Window::GetWindowSize() };
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