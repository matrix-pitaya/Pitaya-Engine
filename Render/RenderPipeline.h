#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Asset/Asset.h>
#include<Core/PassKey/PassKey.h>
#include<Context/Common/Module.h>
#include<Context/Context.h>
#include<Asset/Common/Shader.h>
#include<Render/Common/PostProcessType.h>
#include<Render/Specific/RenderPass.h>
#include<Render/Specific/RenderItem.h>

#include<glm.hpp>

namespace Pitaya::Render
{
	class RenderPipeline
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<RenderPipeline>;
		private:
			static RenderPipeline* Create()
			{
				return PITAYA_NEW(RenderPipeline);
			}
			static void Destroy(RenderPipeline* renderPipeline)
			{
				PITAYA_DELETE(renderPipeline);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<RenderPipeline>;
		private:
			static bool Initialize(RenderPipeline* renderPipeline)
			{
				return renderPipeline->Initialize();
			}
			static void Release(RenderPipeline* renderPipeline)
			{
				renderPipeline->Release();
			}
		};

	private:
		struct PostProcessChain
		{
			Pitaya::Core::Asset<Pitaya::Asset::Shader> BlitShader = nullptr;
			Pitaya::Core::Asset<Pitaya::Asset::Shader> PostProcessShaders[static_cast<uint8_t>(Pitaya::Render::PostProcessType::Unknown)] = {};
		};
		struct RenderGraph
		{
			std::vector<RenderPass> Passes;
			std::vector<RenderItem> Items;
		};

	private:
		RenderPipeline() = default;
		~RenderPipeline() = default;

	public:
		RenderPipeline(const RenderPipeline&) = delete;
		RenderPipeline& operator=(const RenderPipeline&) = delete;
		RenderPipeline(RenderPipeline&&) = delete;
		RenderPipeline& operator=(RenderPipeline&&) = delete;

	private:
		bool Initialize();
		void Release();

	public:
		inline void NewPipeline(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) noexcept
		{
			//清空上一帧残留数据
			graph.Passes.clear();
			graph.Items.clear();
		}
		inline void AddRenderPass(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, const Pitaya::Core::CameraSnapshot& cameraSnapshot, const Pitaya::Render::RenderTargetSnapshot& renderTargetSnapshot, const Pitaya::Render::PostProcessSetting& setting, Pitaya::Render::RenderLayer cullingMask)
		{
			graph.Passes.emplace_back(cameraSnapshot, renderTargetSnapshot, setting, cullingMask);
		}
		inline void AddRenderItem(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Asset::Mesh* mesh, Pitaya::Asset::Material* material, Pitaya::Render::RenderLayer layerMask, const glm::mat4& model, uint32_t subMeshIndex)
		{
			graph.Items.emplace_back(mesh, material, layerMask, model, subMeshIndex);
		}

	public:
		void Execute(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::Renderer* renderer);

	private:
		void SubmitRenderGraph(Pitaya::Render::Renderer* renderer);

	private:
		RenderGraph graph;
		PostProcessChain chain;
	};
}