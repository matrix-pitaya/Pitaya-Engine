#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Camera/CameraSnapshot.h>
#include<Core/Asset/Asset.h>
#include<Core/Utils/Console.h>
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
#include<Render/Command/PostProcessCommand.h>
#include<Render/Specific/RenderPass.h>
#include<Render/Specific/RenderItem.h>

#include<GPU/Common/FuncTable.h>
#include<GPU/Common/TextureType.h>
#include<GPU/Common/BindPoint.h>
#include<GPU/Frontend/Buffer/VertexBuffer.h>
#include<GPU/Frontend/Buffer/VertexArray.h>

#include<Asset/Common/FuncTable.h>
#include<Asset/Common/Shader.h>
#include<Asset/Common/Texture.h>
#include<Asset/Common/Material.h>

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
			Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> EmptyVAO;
			Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> CameraSnapshotUBO;
			Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer> PostProcessUBO;
			Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> InstanceModelTransformSSBO;
			Pitaya::GPU::Identifier<Pitaya::GPU::ShaderStorageBuffer> BoneInverseMatriceSSBO;

			inline bool CreateGlobalRHI()
			{
				EmptyVAO = Pitaya::GPU::CreateVertexArray(); if (EmptyVAO == Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray>::Invalid) { Pitaya::Log::Error("create global RHI error, from create empty vao!"); return false; }
				CameraSnapshotUBO = Pitaya::GPU::CreateUniformBuffer(sizeof(Pitaya::Core::CameraSnapshot), static_cast<uint32_t>(Pitaya::GPU::UBOBindPoint::CameraSnapshot)); if (CameraSnapshotUBO == Pitaya::GPU::Identifier<Pitaya::GPU::UniformBuffer>::Invalid) { Pitaya::Log::Error("create global RHI error, from create camera snapshot UBO!"); return false; }
				PostProcessUBO = Pitaya::GPU::CreateUniformBuffer(Pitaya::Render::PostProcessStep::UniformBufferBytes, static_cast<uint32_t>(Pitaya::GPU::UBOBindPoint::PostProcessUBO));
				InstanceModelTransformSSBO = Pitaya::GPU::CreateShaderStorageBuffer(Pitaya::Config::GetMaxInstancesCount() * sizeof(glm::mat4), static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::InstanceModelTransform));
				BoneInverseMatriceSSBO = Pitaya::GPU::CreateShaderStorageBuffer(1000 * Pitaya::Config::GetMaxBonesPerInstance() * sizeof(glm::mat4), static_cast<uint32_t>(Pitaya::GPU::SSBOBindPoint::BoneInverseMatrice));
				return true;
			}
		};
		struct FallbackRHI
		{
			Pitaya::GPU::Identifier<Pitaya::GPU::VertexArray> VAO;
			Pitaya::GPU::Identifier<Pitaya::GPU::Shader> Shader;
			Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> Texture;
			uint32_t IndexCount = 36;
			uint32_t BaseIndex = 0;
			uint32_t BaseVertex = 0;

			inline bool CreateFallbackRHI()
			{
				float ERROR_VERTICES[] = {
					-0.5f, -0.5f, -0.5f,	 //左下后
					 0.5f, -0.5f, -0.5f,	 //右下后
					 0.5f,  0.5f, -0.5f,	 //右上后
					-0.5f,  0.5f, -0.5f,	 //左上后
					-0.5f, -0.5f,  0.5f,	 //左下前
					 0.5f, -0.5f,  0.5f,	 //右下前
					 0.5f,  0.5f,  0.5f,	 //右上前
					-0.5f,  0.5f,  0.5f };   //左上前
				uint32_t ERROR_INDICES[] = {
					0, 1, 2,  2, 3, 0,		//后面
					1, 5, 6,  6, 2, 1,		//右面
					5, 4, 7,  7, 6, 5,		//前面
					4, 0, 3,  3, 7, 4,		//左面
					4, 5, 1,  1, 0, 4,		//底面
					3, 2, 6,  6, 7, 3 };	//顶面
				VAO = Pitaya::GPU::CreateVertexArray();
				Pitaya::GPU::Identifier VBO = Pitaya::GPU::CreateVertexBuffer(ERROR_VERTICES, sizeof(ERROR_VERTICES));
				Pitaya::GPU::Identifier IBO = Pitaya::GPU::CreateIndexBuffer(ERROR_INDICES, IndexCount);
				Pitaya::GPU::VertexArray* vao = Pitaya::GPU::GetVertexArray(VAO);
				Pitaya::GPU::VertexBuffer* vbo = Pitaya::GPU::GetVertexBuffer(VBO);
				Pitaya::GPU::IndexBuffer* ibo = Pitaya::GPU::GetIndexBuffer(IBO);
				if (!vao || !vbo || !ibo)
				{
					Pitaya::Log::Error("create global RHI error, from error vao or vbo or ibo is empty!");
					if (!Pitaya::GPU::DestroyVertexArray(VAO)) { Pitaya::Log::Error("destroy error VAO fail!"); }
					if (!Pitaya::GPU::DestroyVertexBuffer(VBO)) { Pitaya::Log::Error("destroy error VBO fail!"); }
					if (!Pitaya::GPU::DestroyIndexBuffer(IBO)) { Pitaya::Log::Error("destroy error IBO fail!"); }
					return false;
				}
				vbo->SetLayout({ { Pitaya::GPU::ShaderVariableType::Float3, 0 } });
				vao->AddVertexBuffer(vbo);
				vao->SetIndexBuffer(ibo);

				constexpr int SIZE = 32;
				unsigned char ERROR_TEXTURE_DATA[SIZE * SIZE * 4] = {};
				for (int y = 0; y < SIZE; y++)
				{
					for (int x = 0; x < SIZE; x++)
					{
						int i = (y * SIZE + x) * 4;
						bool checker = ((x + y) & 1) == 0;
						ERROR_TEXTURE_DATA[i + 0] = checker ? 255 : 0;	//R
						ERROR_TEXTURE_DATA[i + 1] = checker ? 255 : 0;	//G
						ERROR_TEXTURE_DATA[i + 2] = checker ? 255 : 0;	//B
						ERROR_TEXTURE_DATA[i + 3] = 255;				//A
					}
				}
				Texture = Pitaya::GPU::CreateTexture2D(ERROR_TEXTURE_DATA, SIZE, SIZE, 4, false, false, true);

				const char* ERROR_VERTEX_SHADER =
					"#version 460 core\n"
					R"(
						layout (location = 0) in vec3 aPos;
						
						layout(std140, binding = 0) uniform CameraSnapshot
						{
						    mat4 View;
						    mat4 Projection;
						    mat4 ViewProjection;
						    vec4 Position;
						};
						
						layout(std430, binding = 0) readonly buffer InstanceModelTransform
						{
						    mat4 Models[];
						};
						
						void main()
						{
						    uint index = gl_BaseInstance + gl_InstanceID;
						    gl_Position = ViewProjection * Models[index] * vec4(aPos,1.0f);
						}
					)";
				const char* ERROR_FRAGMENT_SHADER =
					"#version 460 core\n"
					R"(
						out vec4 FragColor;

						void main()
						{
							FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
						}
					)";
				Shader = Pitaya::GPU::CreateShader(ERROR_VERTEX_SHADER, ERROR_FRAGMENT_SHADER);
				return true;
			}
		};

	protected:
		class RenderPacket
		{
			friend class Renderer;
		public:
			struct alignas(16) Buffer
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
				CommandHeader(Pitaya::Render::RenderCommandType type = Pitaya::Render::RenderCommandType::Unknown, uint32_t size = 0)
					:type(type), size(size) {}

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

				CommandHeader header(T::Type, static_cast<uint32_t>(sizeof(T)));
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
				pass.emplace_back(std::move(cmd));
			}
			inline void CompilePass()
			{
				if (pass.empty()) { return; }

				//通过索引间接排序DrawcallCommand
				static std::vector<uint32_t> sortedIndices;
				uint32_t drawCommandCount = pass.size();
				sortedIndices.resize(drawCommandCount);
				std::iota(sortedIndices.begin(), sortedIndices.end(), 0);

				//通过SortKey对DrawComamnd进行排序
				std::sort(sortedIndices.begin(), sortedIndices.end(),
					[&](uint32_t a, uint32_t b) { return pass[a].SortKey < pass[b].SortKey; });

				//和批处理
				bool isBatching = false;
				uint32_t debug_drawtimes = 0;
				Pitaya::Render::InstancedDrawCommand currentBatch;
				for (uint32_t idx : sortedIndices)
				{
					const auto& cmd = pass[idx];
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
						currentBatch.BaseInstance = static_cast<uint32_t>(front.InstanceModelTransforms.size());
					}

					front.InstanceModelTransforms.push_back(cmd.ModelMatrix);

					size_t maxBones = static_cast<size_t>(Pitaya::Config::GetMaxBonesPerInstance());
					size_t bonesToCopy = std::min(cmd.BoneInverseMatrices.size(), maxBones);
					size_t paddingBones = maxBones - bonesToCopy;
					if (bonesToCopy > 0)
					{
						front.BoneMatrices.insert(
							front.BoneMatrices.end(),
							cmd.BoneInverseMatrices.begin(),
							cmd.BoneInverseMatrices.begin() + bonesToCopy);
					}
					if (paddingBones > 0)
					{
						front.BoneMatrices.insert(
							front.BoneMatrices.end(),
							paddingBones,
							glm::mat4(1.0f));
					}
					currentBatch.InstanceCount++;
				}

				//提交最后一个批次
				if (isBatching) { PushCommand(currentBatch); debug_drawtimes++; }
				Core::Print(Core::Color::Red, "[Batch] Before:%d to After:%d", pass.size(), debug_drawtimes);
				pass.clear();
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
			std::vector<Pitaya::Render::DrawCommand> pass;				//用于对DrawCommand进行排序
		};

	protected:
		Renderer() = default;
		virtual ~Renderer() = default;

	public:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	protected:
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
		inline void RenderThread(void* nativeWindow)
		{
			InitializeRenderContext(nativeWindow);
			globalRHI.CreateGlobalRHI();
			fallbackRHI.CreateFallbackRHI();
			INVOKE_POSTRENDERCONTEXTINITIALIZED_HOOK

			while (true)
			{
				std::unique_lock<std::mutex> lock(mutex);
				cond.wait(lock, [this] { return renderPacket.IsRemain() || INVOKE_SHOULDWAKEUPRENDERTHREAD_HOOK ||
					Pitaya::Asset::IsUploadedToGPU() || !isRunning.load(std::memory_order_acquire); });
				if (!isRunning.load(std::memory_order_acquire)) { break; }

				ManageGPUMemory();
				NewRenderFrame();
				ParseCommand();
				SwapBuffer();
			}

			INVOKE_PRERENDERCONTEXTINRELEASED_HOOK
			Pitaya::GPU::DestroyAllGPUResource(Pitaya::Core::PassKey<Pitaya::Render::Renderer>());
			ReleaseRenderContext();
		}
		virtual bool InitializeRenderContext(void* nativeWindow) = 0;
		virtual void ReleaseRenderContext() = 0;

	protected:
		inline void ManageGPUMemory()
		{
			Pitaya::Asset::SyncAssetToGPU();
		}
		inline void ParseCommand()
		{
			renderPacket.ParseCommand(this);
		}
		virtual void NewRenderFrame() = 0;
		virtual void SwapBuffer() const = 0;

	protected:
		virtual void ExecuteCommand(const Pitaya::Render::BeginPassCommand* command) const = 0;
		virtual void ExecuteCommand(const Pitaya::Render::InstancedDrawCommand* command) const = 0;
		virtual void ExecuteCommand(const Pitaya::Render::PostProcessCommand* command) const = 0;

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
			beginPassCommand.RenderTargetSnapshot = pass.RenderTargetSnapshot;
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
				cmd.VertexArray = fallbackRHI.VAO;
				cmd.IndexCount = fallbackRHI.IndexCount;
				cmd.BaseIndex = fallbackRHI.BaseIndex;
				cmd.BaseVertex = fallbackRHI.BaseVertex;
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
						cmd.Textures[j] = fallbackRHI.Texture;
					}
				}
			}
			else
			{
				//Material/Shader 异常 → fallback shader + texture
				cmd.Shader = fallbackRHI.Shader;
				cmd.MaterialId = 0;
				cmd.SortKey = Pitaya::Render::GenerateSortKey(
					Pitaya::Render::RenderQueue::Geometry,
					0,
					cmd.Shader,
					0,
					(cmd.VertexArray << 8) + submeshIndex,
					0);

				//异常Shader只会使用这一个纹理
				cmd.Textures[static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Albedo)] = fallbackRHI.Texture;
			}

			renderPacket.PushDrawCommandToPass(cmd);
		}
		inline void SubmitPostProcess(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>, const PostProcessCommand& cmd)
		{
			renderPacket.PushCommand(cmd);
		}
		inline void EndPass(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>)
		{
			renderPacket.CompilePass();
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

	protected:
		inline static void BootstrapRenderThread(void* renderer, void* nativeWindow)
		{
			static_cast<Pitaya::Render::Renderer*>(renderer)->RenderThread(nativeWindow);
		}

	protected:
		std::mutex mutex;
		std::condition_variable cond;
		std::atomic<bool> isRunning = false;
		Pitaya::Core::Thread::Identifier renderThread;
		Pitaya::Render::Renderer::RenderPacket renderPacket;
		Pitaya::Render::Renderer::GlobalRHI globalRHI;
		Pitaya::Render::Renderer::FallbackRHI fallbackRHI;
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