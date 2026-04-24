#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Camera/CameraSnapshot.h>
#include<Core/Asset/Asset.h>
#include<Core/Utils/Console.h>
#include<Core/Utils/BuildInRC.h>
#include<Hook/def.h>
#include<Thread/Common/FuncTable.h>
#include<Log/Common/FuncTable.h>
#include<Config/Common/FunctionTable.h>
#include<Window/Common/FuncTable.h>

#include<Render/Common/API.h>
#include<Render/Common/RenderCommandType.h>
#include<Render/Common/RenderQueue.h>
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

#include<Application/resource.h>

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
			static Renderer* Create(Pitaya::Render::API);
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

	protected:
		struct GlobalRHI
		{
			//Specific
			Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> EmptyVAO;

			//Fallback
			Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> FallbackVAO;
			Pitaya::GPU::Identifier<Pitaya::GPU::Shader> FallbackShader;
			Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> FallbackTexture;

			//PostProcess Shader
			Pitaya::GPU::Identifier<Pitaya::GPU::Shader> BlitShader;
			Pitaya::GPU::Identifier<Pitaya::GPU::Shader> GammaCorrectionShader;

			//Uniform Buffer
			Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> CameraSnapshotUBO;
			Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> PostProcessUBO;

			//ShaderStorageBuffer
			Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> InstanceModelTransformSSBO;
			Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> BoneInverseMatriceSSBO;
			size_t TransformSSBOCapacity = 0;	//记录当前显存缓冲区的大小
			size_t BoneSSBOCapacity = 0;

			//MainDisplayRT
			Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> MainSceneFrameBuffer = 0;
			Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> MainSceneInternalFrameBuffer = 0; //用于多采用解析
			Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> MainSceneColorAttachment = 0;
			Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> MainPingPongFrameBuffers[2] = { 0, 0 };
			Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> MainPingPongColorAttachments[2] = { 0, 0 };
			Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> MainFinalFrameBuffer = 0;
			Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> MainFinalColorAttachment = 0;

			inline void Create(Pitaya::Core::PassKey<Pitaya::Render::Renderer>)
			{
				EmptyVAO = Pitaya::GPU::CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
				CameraSnapshotUBO = Pitaya::GPU::CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					sizeof(Pitaya::Core::CameraSnapshot), static_cast<uint32_t>(Pitaya::GPU::UBOBindPoint::CameraSnapshot));
				PostProcessUBO = Pitaya::GPU::CreateUniformBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					Pitaya::Render::PostProcessStep::UniformBufferBytes, static_cast<uint32_t>(Pitaya::GPU::UBOBindPoint::PostProcessUBO));
				
				// 初始分配1024个位置
				TransformSSBOCapacity = 1024 * sizeof(glm::mat4);	
				InstanceModelTransformSSBO = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					TransformSSBOCapacity, static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform));
				
				// 初始分配一段骨骼容量
				BoneSSBOCapacity = 4096 * sizeof(glm::mat4);	
				BoneInverseMatriceSSBO = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					BoneSSBOCapacity, static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice));
				
				BlitShader = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					Pitaya::Core::LoadBuildInRC(IDR_BLIT_VERTEX_SHADER).c_str(),
					Pitaya::Core::LoadBuildInRC(IDR_BLIT_FRAGMENT_SHADER).c_str());
				GammaCorrectionShader = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					Pitaya::Core::LoadBuildInRC(IDR_GAMMA_CORRECTION_VERTEX_SHADER).c_str(),
					Pitaya::Core::LoadBuildInRC(IDR_GAMMA_CORRECTION_FRAGMENT_SHADER).c_str());

				FallbackVAO = Pitaya::GPU::CreateVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
				std::string fallbackVboData = Pitaya::Core::LoadBuildInRC(IDR_ERROR_VERTICES);
				Pitaya::GPU::Identifier fallbackVBO = Pitaya::GPU::CreateVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					reinterpret_cast<float*>(fallbackVboData.data()), fallbackVboData.size());
				std::string fallbackIboData = Pitaya::Core::LoadBuildInRC(IDR_ERROR_INDICES);
				Pitaya::GPU::Identifier fallbackIBO = Pitaya::GPU::CreateIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					reinterpret_cast<uint32_t*>(fallbackIboData.data()), 36);
				Pitaya::GPU::VertexArray* fallbackVaoPtr = Pitaya::GPU::GetVertexArray(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), FallbackVAO);
				Pitaya::GPU::VertexBuffer* fallbackVboPtr = Pitaya::GPU::GetVertexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), fallbackVBO);
				Pitaya::GPU::IndexBuffer* fallbackIboPtr = Pitaya::GPU::GetIndexBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), fallbackIBO);
				if (!fallbackVaoPtr || !fallbackVboPtr || !fallbackIboPtr)
				{
					// TODO 考虑一下渲染线程如何通知主线程失败
					MessageBoxA(NULL, "Create Global RHI Failed! Check Log for Details.", "Error", MB_OK);
					exit(-1);
				}
				fallbackVboPtr->SetLayout({ { Pitaya::GPU::ShaderVariableType::Float3, 0 } });
				fallbackVaoPtr->AddVertexBuffer(fallbackVboPtr);
				fallbackVaoPtr->SetIndexBuffer(fallbackIboPtr);

				FallbackTexture = Pitaya::GPU::CreateTexture2D(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					reinterpret_cast<unsigned char*>(Pitaya::Core::LoadBuildInRC(IDR_ERROR_TEXTURE).data()),
					32, 32, 4, false, false, true);
				FallbackShader = Pitaya::GPU::CreateShader(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(),
					Pitaya::Core::LoadBuildInRC(IDR_ERROR_VERTEX_SHADER).c_str(),
					Pitaya::Core::LoadBuildInRC(IDR_ERROR_FRAGMENT_SHADER).c_str());

				//MainRT
				Pitaya::GPU::FrameBufferSpecification mainSceneSpec = Pitaya::Config::GetMainSceneSpec();
				Pitaya::GPU::FrameBufferSpecification mainPingPongSpec = Pitaya::Config::GetMainPingPongSpec();
				Pitaya::GPU::FrameBufferSpecification mainFinalSpec = Pitaya::Config::GetMainFinalSpec();
				Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> mainSceneGPUIdentifier = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainSceneSpec);
				Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> mainPingPongGPUIdentifier[2] = { 
					Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainPingPongSpec),
					Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainPingPongSpec) };
				Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> mainFinalGPUIdentifier = Pitaya::GPU::CreateFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainFinalSpec);
				Pitaya::GPU::FrameBuffer* sceneFrambuffer = Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainSceneGPUIdentifier);
				Pitaya::GPU::FrameBuffer* pingPongFrambuffer[2] = { Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainPingPongGPUIdentifier[0]), Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainPingPongGPUIdentifier[1]) };
				Pitaya::GPU::FrameBuffer* finalFrambuffer = Pitaya::GPU::GetFrameBuffer(Pitaya::Core::PassKey<Pitaya::Render::Renderer>(), mainFinalGPUIdentifier);
				if (!sceneFrambuffer || !pingPongFrambuffer[0] || !pingPongFrambuffer[1] || !finalFrambuffer)
				{
					// TODO 考虑一下渲染线程如何通知主线程失败
					MessageBoxA(NULL, "Create Global RHI Failed! Check Log for Details.", "Error", MB_OK);
					exit(-1);
				}
				MainSceneFrameBuffer = mainSceneGPUIdentifier;
				MainSceneInternalFrameBuffer = sceneFrambuffer->GetInternalGPUIdentifier();
				MainSceneColorAttachment = sceneFrambuffer->GetColorAttachmentGPUIdentifier();
				MainPingPongFrameBuffers[0] = mainPingPongGPUIdentifier[0];
				MainPingPongColorAttachments[0] = pingPongFrambuffer[0]->GetColorAttachmentGPUIdentifier();
				MainPingPongFrameBuffers[1] = mainPingPongGPUIdentifier[1];
				MainPingPongColorAttachments[1] = pingPongFrambuffer[1]->GetColorAttachmentGPUIdentifier();
				MainFinalFrameBuffer = mainFinalGPUIdentifier;
				MainFinalColorAttachment = finalFrambuffer->GetColorAttachmentGPUIdentifier();
			}
		};

	protected:
		class RenderPacket
		{
			friend class Renderer;
		public:
			struct Buffer
			{
				std::vector<std::byte> CommandBuffer;
				std::vector<glm::mat4> InstanceModelTransforms;
				std::vector<glm::mat4> BoneMatrices;

				inline void Clear() noexcept
				{
					CommandBuffer.clear();
					CommandBuffer.reserve(64 * 1024);

					InstanceModelTransforms.clear();
					InstanceModelTransforms.reserve(1024);

					BoneMatrices.clear();
					BoneMatrices.reserve(1024);
				}
			};

		private:
			struct CommandHeader
			{
				Pitaya::Render::RenderCommandType type = Pitaya::Render::RenderCommandType::Unknown;
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
			inline void PushDrawCommandToPass(Pitaya::Render::DrawCommand& cmd)
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
				uint32_t beforeSize = skinnedPass.size() + staticPass.size();
				if (beforeSize == 0) { return; }
				uint32_t debug_drawtimes = 0;
				// 处理命令队列
				auto ProcessQueue = [&](std::vector<Pitaya::Render::DrawCommand>& currentPass, bool isSkinnedBatch)
					{
						if (currentPass.empty()) { return; }

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
						Pitaya::Render::InstancedDrawCommand currentBatch;

						for (uint32_t idx : sortedIndices)
						{
							const auto& cmd = currentPass[idx];
							bool canBatch = isBatching &&
								cmd.VertexArray == currentBatch.VertexArray &&
								cmd.BaseIndex == currentBatch.BaseIndex &&
								cmd.BaseVertex == currentBatch.BaseVertex &&
								cmd.IndexCount == currentBatch.IndexCount &&
								cmd.MaterialId == currentBatch.MaterialId &&
								cmd.DepthTest == currentBatch.DepthTest &&
								cmd.Blend == currentBatch.Blend;

							if (!canBatch)
							{
								if (isBatching) { PushCommand(currentBatch); debug_drawtimes++; }
								isBatching = true;

								// 复制状态
								currentBatch.VertexArray = cmd.VertexArray;
								currentBatch.MaterialId = cmd.MaterialId;
								currentBatch.IndexCount = cmd.IndexCount;
								currentBatch.BaseIndex = cmd.BaseIndex;
								currentBatch.BaseVertex = cmd.BaseVertex;
								currentBatch.DepthTest = cmd.DepthTest;
								currentBatch.Blend = cmd.Blend;
								currentBatch.Shader = cmd.Shader;
								for (uint32_t i = 0; i < static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Unknown); i++)
								{
									currentBatch.Textures[i] = cmd.Textures[i];
								}

								currentBatch.InstanceCount = 0;
								currentBatch.BaseInstance = static_cast<uint32_t>(front.InstanceModelTransforms.size()); // 不论是哪个队列 BaseInstance 永远递增
							}

							// Transform SSBO
							front.InstanceModelTransforms.push_back(cmd.ModelMatrix);

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
						if (isBatching) { PushCommand(currentBatch); debug_drawtimes++; }
						currentPass.clear();
					};

				// 严格控制调用顺序 先骨骼网格 后静态网格
				ProcessQueue(skinnedPass, true);   // 先处理骨骼队列
				ProcessQueue(staticPass, false);   // 后处理无骨骼的静态物体队列

				Core::Print(Core::Color::Red, "[Batch] Before:%d to After:%d", beforeSize, debug_drawtimes);
			}
			inline void SwapBuffer()
			{
				std::swap(front, back);
				INVOKE_POSTRENDERERSWAPBUFFER_HOOK
			}

		public:
			inline Buffer& GetFrontBuffer() noexcept
			{
				return front;
			}
			inline const Buffer& GetBackBuffer() const noexcept
			{
				return back;
			}

		private:
			Buffer front;												//主线程写入渲染命令、实例化Models、骨骼动画
			Buffer back;												//渲染线程执行渲染命令
			std::vector<Pitaya::Render::DrawCommand> skinnedPass;		//用于对DrawCommand进行排序
			std::vector<Pitaya::Render::DrawCommand> staticPass;
		};

	protected:
		Renderer() = default;
		virtual ~Renderer() = default;

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

	protected:
		virtual bool InitializeRenderContext(void* nativeWindow) = 0;
		virtual void ReleaseRenderContext() = 0;

	private:
		inline void RenderThread(void* nativeWindow)
		{
			InitializeRenderContext(nativeWindow);
			globalRHI.Create(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
			INVOKE_POSTRENDERCONTEXTINITIALIZED_HOOK(globalRHI.MainFinalColorAttachment)

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
					ParseCommand();
					SwapBuffer();
				}
			}

			INVOKE_PRERENDERCONTEXTINRELEASED_HOOK
			Pitaya::GPU::DestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
			ReleaseRenderContext();
		}
		inline void ParseCommand()
		{
			renderPacket.ParseCommand(this);
		}

	protected:
		virtual void NewRenderFrame() = 0;
		virtual void SwapBuffer() const = 0;

	protected:
		virtual void ExecuteCommand(const Pitaya::Render::BeginPassCommand* command) const = 0;
		virtual void ExecuteCommand(const Pitaya::Render::InstancedDrawCommand* command) const = 0;
		virtual void ExecuteCommand(const Pitaya::Render::PostProcessCommand* command) const = 0;
		virtual void ExecuteCommand(const Pitaya::Render::BlitToScreenCommand* command) const = 0;

	public:
		inline void BeginRenderFrame(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>)
		{
			renderPacket.GetFrontBuffer().Clear();
			INVOKE_POSTRENDERERBEGINRENDERFRAME_HOOK
		}
		inline void BeginPass(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>, RenderPass& pass)
		{
			Pitaya::Render::BeginPassCommand beginPassCommand;
			beginPassCommand.CameraSnapshot = pass.CameraSnapshot;
			if (pass.RenderTarget)
			{
				beginPassCommand.SceneFrameBuffer = pass.RenderTarget->SceneFrameBuffer;
				beginPassCommand.ClearColor = pass.RenderTarget->ClearColor;
				beginPassCommand.Rect = { {0.0f, 0.0f}, { pass.RenderTarget->SceneFrameBufferSpecification.Width, pass.RenderTarget->SceneFrameBufferSpecification.Height } };
				beginPassCommand.ClearDepth = pass.RenderTarget->ClearDepth;
				beginPassCommand.ClearStencil = pass.RenderTarget->ClearStencil;
			}
			else
			{
				Pitaya::GPU::FrameBufferSpecification mainSceneSpec = Pitaya::Config::GetMainSceneSpec();
				beginPassCommand.SceneFrameBuffer = globalRHI.MainSceneFrameBuffer;
				beginPassCommand.ClearColor = Pitaya::Core::Color::SkyBlue;
				beginPassCommand.Rect = { {0.0f, 0.0f}, { mainSceneSpec.Width, mainSceneSpec.Height } };
				beginPassCommand.ClearDepth = true;
				beginPassCommand.ClearStencil = true;
			}
			renderPacket.PushCommand(beginPassCommand);
		}
		inline void Submit(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>, RenderItem& item)
		{
			auto* mesh = item.Mesh;
			auto* material = item.Material;
			uint32_t submeshIndex = item.SubMeshIndex;

			Pitaya::Render::DrawCommand cmd;
			cmd.ModelMatrix = item.Model;
			if (mesh && mesh->VertexArray != Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray>::Invalid && 
				submeshIndex < mesh->SubMeshs.size())
			{
				cmd.VertexArray = mesh->VertexArray;
				cmd.IndexCount = mesh->SubMeshs[submeshIndex].IndexCount;
				cmd.BaseIndex = mesh->SubMeshs[submeshIndex].BaseIndex;
				cmd.BaseVertex = mesh->SubMeshs[submeshIndex].BaseVertex;
				cmd.BoneInverseMatrices = mesh->BoneInverseMatrices;
			}
			else
			{
				//Mesh 异常 → fallback 立方体
				cmd.VertexArray = globalRHI.FallbackVAO;
				cmd.IndexCount = 36;
				cmd.BaseIndex = 0;
				cmd.BaseVertex = 0;
				//异常情况无需骨骼数据
			}

			if (material && material->Shader.IsReady() && 
				material->Shader->ID != Pitaya::GPU::Identifier<Pitaya::GPU::Shader>::Invalid)
			{
				cmd.Shader = material->Shader->ID;
				cmd.MaterialId = material->SortId;
				cmd.SortKey = Pitaya::Render::GenerateSortKey(
					material->RenderQueue,
					material->DrawOrder,
					cmd.Shader,
					cmd.MaterialId,
					(cmd.VertexArray << 8) + submeshIndex,
					0);

				//纹理绑定
				for (uint32_t j = 0; j < static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Unknown); j++)
				{
					//无纹理
					if (!material->Textures[j]) { continue; }

					//纹理就绪
					if (material->Textures[j].IsReady() && 
						material->Textures[j]->Type == Pitaya::GPU::TextureType::Texture2D && 
						material->Textures[j]->Id.template As<Pitaya::GPU::Texture2D>() != Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D>::Invalid)
					{
						cmd.Textures[j] = material->Textures[j]->Id.template As<Pitaya::GPU::Texture2D>();
					}
					else
					{
						//纹理未就绪
						cmd.Textures[j] = globalRHI.FallbackTexture;
					}
				}
			}
			else
			{
				//Material/Shader 异常 → fallback shader + texture
				cmd.Shader = globalRHI.FallbackShader;
				cmd.MaterialId = 0;
				cmd.SortKey = Pitaya::Render::GenerateSortKey(
					Pitaya::Render::RenderQueue::Geometry,
					0,
					cmd.Shader,
					0,
					(cmd.VertexArray << 8) + submeshIndex,
					0);

				//异常Shader只会使用这一个纹理
				cmd.Textures[static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Albedo)] = globalRHI.FallbackTexture;
			}

			renderPacket.PushDrawCommandToPass(cmd);
		}
		inline void EndPass(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>)
		{
			renderPacket.CompilePass();
		}
		inline void SubmitPostProcess(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>, RenderPass& pass)
		{
			bool firstPass = true;
			uint32_t pingpongIndex = 0;
			uint32_t currentReadTexture = pass.RenderTarget ? pass.RenderTarget->SceneColorAttachment : globalRHI.MainSceneColorAttachment; //如果是多采样 这里得到的实际是内部的颜色纹理

			auto mainSceneSpec = Pitaya::Config::GetMainSceneSpec();
			auto isMultisample = pass.RenderTarget ? pass.RenderTarget->SceneFrameBufferSpecification.Samples > 1 : mainSceneSpec.Samples > 1;
			auto sceneFrameBuffer = pass.RenderTarget ? pass.RenderTarget->SceneFrameBuffer : globalRHI.MainSceneFrameBuffer;
			auto sceneInternalFrameBuffer = pass.RenderTarget ? pass.RenderTarget->SceneInternalFrameBuffer : globalRHI.MainSceneInternalFrameBuffer;
			auto sceneColorAttachment = pass.RenderTarget ? pass.RenderTarget->SceneColorAttachment : globalRHI.MainSceneColorAttachment;
			auto finalFrameBuffer = pass.RenderTarget ? pass.RenderTarget->FinalFrameBuffer : globalRHI.MainFinalFrameBuffer;
			auto size = pass.RenderTarget ? glm::ivec2(pass.RenderTarget->SceneFrameBufferSpecification.Width, pass.RenderTarget->SceneFrameBufferSpecification.Height) :
				glm::ivec2(mainSceneSpec.Width, mainSceneSpec.Height);

			for (uint32_t i = 0; i < pass.PostProcessSetting.StepCount; i++)
			{
				auto& currentStep = pass.PostProcessSetting.Steps[i];
				auto pingPongFrameBuffers = pass.RenderTarget ? pass.RenderTarget->PingPongFrameBuffers[pingpongIndex] : globalRHI.MainPingPongFrameBuffers[pingpongIndex];
				auto pingPongColorAttachments = pass.RenderTarget ? pass.RenderTarget->PingPongColorAttachments[pingpongIndex] : globalRHI.MainPingPongColorAttachments[pingpongIndex];
				PostProcessCommand cmd;
				cmd.PostProcessStep = currentStep;
				switch (currentStep.Type)
				{
				case Pitaya::Render::PostProcessType::Bilt:
					cmd.ProcessShader = globalRHI.BlitShader;
					break;

				case Pitaya::Render::PostProcessType::GammaCorrection:
					cmd.ProcessShader = globalRHI.GammaCorrectionShader;
					break;

				case Pitaya::Render::PostProcessType::Unknown:
				default:
					cmd.ProcessShader = globalRHI.BlitShader;	// 防止Ping-Pong链截断
					break;
				}
				if (firstPass && isMultisample)
				{
					cmd.ResolveMSAA = true;
					cmd.ResolveReadFrameBuffer = sceneFrameBuffer;
					cmd.ResolveWriteFrameBuffer = sceneInternalFrameBuffer;
					cmd.ResolveSize = size;
				}

				cmd.ReadTexture = currentReadTexture;
				cmd.WriteFrameBuffer = (i == pass.PostProcessSetting.StepCount - 1) ? finalFrameBuffer : pingPongFrameBuffers;
				currentReadTexture = pingPongColorAttachments;
				pingpongIndex = 1 - pingpongIndex;
				firstPass = false;

				renderPacket.PushCommand(cmd);
				Pitaya::Core::Print(Pitaya::Core::Color::Purple, "Post Process: %s", Pitaya::Render::ToString(currentStep.Type).data());
			}

			//没有后处理则直接Scene帧缓冲区 Blit到 Final帧缓冲区
			if (firstPass)
			{
				PostProcessCommand cmd;
				cmd.ProcessShader = globalRHI.BlitShader;
				cmd.ReadTexture = sceneColorAttachment;
				cmd.WriteFrameBuffer = finalFrameBuffer;
				if (isMultisample)
				{
					cmd.ResolveMSAA = true;
					cmd.ResolveReadFrameBuffer = sceneFrameBuffer;
					cmd.ResolveWriteFrameBuffer = sceneInternalFrameBuffer;
					cmd.ResolveSize = size;
				}

				renderPacket.PushCommand(cmd);
				Pitaya::Core::Print(Pitaya::Core::Color::Purple, "Post Process Resolve To Final (Bypass)");
			}
		}
		inline void SubmitBlitToScreen(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>)
		{
			Pitaya::Render::BlitToScreenCommand blitToScreenCommand { Pitaya::Window::GetWindowSize() };
			renderPacket.PushCommand(blitToScreenCommand);
		}
		inline void EndRenderFrame(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>)
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
	
	protected:
		GlobalRHI globalRHI;
		RenderPacket renderPacket;

	private:
		std::mutex mutex;
		std::condition_variable cond;
		std::atomic<bool> isRunning = false;
		Pitaya::Core::Thread::Identifier renderThread;
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