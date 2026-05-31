#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Asset/AssetRef.h>
#include<Core/PassKey/PassKey.h>

#include<Context/Common/Module.h>
#include<Context/Context.h>

#include<Render/Common/PostProcessType.h>
#include<Render/Common/LightInfo.h>
#include<Render/Common/ShadowInfo.h>
#include<Render/Specific/RenderPass.h>
#include<Render/Specific/RenderItem.h>
#include<Render/Specific/SceneEnv.h>

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
            std::vector<ShadowCasterSlice> ShadowSlices;
            std::vector<CascadeSplitInfo> CascadeSplits;
            std::vector<glm::mat4> ShadowMatrices;
            glm::vec3 FrustumCorners[8] = {};
            SceneEnv SceneEnv;
            uint32_t DirLightShadowCount = 0;
            uint32_t SpotLightShadowCount = 0;
            uint32_t PointLightShadowCount = 0;
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
            graph.ShadowSlices.reserve(64);
            graph.ShadowMatrices.reserve(256);
            graph.CascadeSplits.reserve(16);
            return true;
        }
        inline void Release()
        {
            // clear render graph
            graph.Passes.clear();
            graph.Items.clear();
            graph.Lights.clear();
            graph.ShadowSlices.clear();
            graph.ShadowMatrices.clear();
            graph.CascadeSplits.clear();
        }

    public:
        inline void NewPipeline(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) noexcept
        {
            //清空上一帧残留数据
            graph.Passes.clear();
            graph.Items.clear();
            graph.Lights.clear();
            graph.ShadowSlices.clear();
            graph.ShadowMatrices.clear();
            graph.CascadeSplits.clear();
            graph.DirLightShadowCount = 0;
            graph.SpotLightShadowCount = 0;
            graph.PointLightShadowCount = 0;
            graph.SceneEnv = { };
        }
        inline void AddRenderPass(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, const Pitaya::Core::CameraSnapshot& cameraSnapshot,  const Pitaya::Render::PostProcessSetting& setting, Pitaya::Render::RenderLayer cullingMask, Pitaya::Asset::RenderTarget* rt, float nearClip, float farClip)
        {
            graph.Passes.emplace_back(cameraSnapshot, setting, cullingMask, rt, nearClip, farClip, 0);
        }
        inline void AddRenderItem(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Asset::Mesh* mesh, Pitaya::Asset::Material* material, Pitaya::Render::RenderLayer layerMask, glm::mat4 model, uint32_t subMeshIndex, bool enableShadowCast, bool receiveShadow)
        {
            graph.Items.emplace_back(mesh, material, layerMask, model, subMeshIndex, enableShadowCast, receiveShadow);
        }
        inline void AddSceneLight(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, const Pitaya::Render::LightInfo& light)
        {
            graph.Lights.emplace_back(light);
        }
        inline void OverrideSceneEnv(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, const Pitaya::Asset::SkyBox* skyBox, glm::vec3 ambientColor)
        {
            graph.SceneEnv = { skyBox, ambientColor };
        }

    public:
        void Execute(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::Renderer*);

    private:
        void SubmitRenderGraph(Pitaya::Render::Renderer*);
        void BuildShadowData(Pitaya::Render::Renderer*);

    private:
        void ComputeCSMCascades(const Pitaya::Core::CameraSnapshot& cameraSnap, float nearClip, float farClip, glm::vec3 lightDir, uint32_t matrixOffset);
        void ComputeSpotShadowMatrix(glm::vec3 lightPos, glm::vec3 lightDir, float outerAngleCos, float radius, uint32_t matrixOffset);
        void ComputePointShadowMatrices(glm::vec3 lightPos, float radius, uint32_t matrixOffset);
        void ExtractFrustumCorners(glm::mat4 invViewProj);

    private:
        RenderGraph graph;
    };
}
