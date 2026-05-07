#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Asset/Asset.h>
#include<Core/PassKey/PassKey.h>

#include<Context/Common/Module.h>
#include<Context/Context.h>

#include<Render/Common/PostProcessType.h>
#include<Render/Common/LightInfo.h>
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
		struct RenderGraph
		{
			std::vector<RenderPass> Passes;
			std::vector<RenderItem> Items;
			std::vector<LightInfo> Lights;
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
		inline bool Initialize()
		{
			// reserve render graph
			graph.Passes.reserve(10);
			graph.Items.reserve(5000);
			graph.Lights.reserve(10);
			return true;
		}
		inline void Release()
		{
			// clear render graph
			graph.Passes.clear();
			graph.Items.clear();
			graph.Lights.clear();
		}

	public:
		inline void NewPipeline(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) noexcept
		{
			//清空上一帧残留数据
			graph.Passes.clear();
			graph.Items.clear();
			graph.Lights.clear();
		}
		inline void AddRenderPass(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, const Pitaya::Core::CameraSnapshot& cameraSnapshot,  const Pitaya::Render::PostProcessSetting& setting, Pitaya::Render::RenderLayer cullingMask, Pitaya::Asset::RenderTarget* rt)
		{
			graph.Passes.emplace_back(cameraSnapshot, setting, cullingMask, rt);
		}
		inline void AddRenderItem(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Asset::Mesh* mesh, Pitaya::Asset::Material* material, Pitaya::Render::RenderLayer layerMask, const glm::mat4& model, uint32_t subMeshIndex)
		{
			graph.Items.emplace_back(mesh, material, layerMask, model, subMeshIndex);
		}
		inline void AddSceneLight(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, const Pitaya::Render::LightInfo& light)
		{
			graph.Lights.emplace_back(light);
		}
		
	public:
		void Execute(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::Renderer*);

	private:
		void SubmitRenderGraph(Pitaya::Render::Renderer*);

	private:
		RenderGraph graph;
	};
}
