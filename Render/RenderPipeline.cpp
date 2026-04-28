#include<Render/RenderPipeline.h>
#include<Render/Frontend/Renderer.h>
#include<Asset/Common/FuncTable.h>
#include<Core/Utils/Console.h>
#include<Core/Asset/Asset.h>
#include<Hook/def.h>

bool Pitaya::Render::RenderPipeline::Initialize()
{
	graph.Passes.reserve(10);
	graph.Items.reserve(5000);
	graph.Lights.reserve(10);
	return true;
}
void Pitaya::Render::RenderPipeline::Release()
{
	graph.Passes.clear();
	graph.Items.clear();
	graph.Lights.clear();
}
void Pitaya::Render::RenderPipeline::Execute(Pitaya::Core::PassKey<Pitaya::Engine::Engine> passkey, Pitaya::Render::Renderer* renderer)
{
	INVOKE_PRERENDERPIPELINEEXECUTE_HOOK(passkey, this)
	Pitaya::Core::Print(Pitaya::Core::Color::Green, "Begin Render Frame");
	renderer->BeginRenderFrame(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>()); //清空fornt缓冲区残余数据

	//提交渲染视图
	SubmitRenderGraph(renderer);

	//交换缓冲区（front→back）并唤醒渲染线程根据back缓冲区命令、SSBO数据进行渲染
	renderer->EndRenderFrame(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>());
	Pitaya::Core::Print(Pitaya::Core::Color::Green, "End Render Frame\n");
}
void Pitaya::Render::RenderPipeline::SubmitRenderGraph(Pitaya::Render::Renderer* renderer)
{
	for (auto& pass : graph.Passes)	//处理所有渲染通道
	{
		Pitaya::Core::Print(Pitaya::Core::Color::Yellow, "Begin Pass");
		renderer->BeginPass(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), pass);	//提交BeginPass命令 并在命令中更新CameraSnapshotUBO数据（一个Camera对应一个Pass）
		Pitaya::Core::Frustum frustum = pass.CameraSnapshot.CreateFrustum();
		uint32_t submitCount = 0;
		for (auto& item : graph.Items)	//处理所有渲染对象
		{
			if (pass.CullingMask.HasBits(item.LayerMask.GetEnum()) && 
				frustum.IsVisible(item.Mesh ? item.Mesh->BoundingBox.ToWorld(item.Model) :
				Pitaya::Core::AABB({ glm::vec3(-0.5f), glm::vec3(0.5f) }).ToWorld(item.Model)))
			{
				renderer->Submit(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), item);
				++submitCount;
			}
		}
		Pitaya::Core::Print(Pitaya::Core::Color::Blue, "Commit Count:%d", submitCount);
		renderer->EndPass(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>()); //对当前Pass提交的Draw命令进行排序并和批处理 生成InstanceDraw命令写入front缓冲区
		renderer->SubmitPostProcess(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), pass);
		Pitaya::Core::Print(Pitaya::Core::Color::Yellow, "End Pass");
	}

	for (auto& light : graph.Lights) //处理所有光源信息
	{
		renderer->SubmitLight(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), light);
	}

	INVOKE_TERMINATERENDERPIPELINESUBMITFINALBLIT_HOOK
	renderer->SubmitBlitToScreen(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>());
	Pitaya::Core::Print(Pitaya::Core::Color::Red, "Blit Main To Screnn Back Buffer");
}
