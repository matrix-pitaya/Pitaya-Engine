#include<Render/RenderPipeline.h>
#include<Render/Frontend/Renderer.h>
#include<Asset/Common/FuncTable.h>
#include<Core/Utils/Console.h>
#include<Core/Asset/Asset.h>
#include<Hook/def.h>

bool Pitaya::Render::RenderPipeline::Initialize()
{
	graph.Passes.reserve(2);
	graph.Items.reserve(100);

	chain.BlitShader = Pitaya::Asset::LoadAsset<Pitaya::Asset::Shader>(Pitaya::Asset::Shader::Blit);
	chain.PostProcessShaders[static_cast<uint8_t>(Pitaya::Render::PostProcessType::GammaCorrection)] = Pitaya::Asset::LoadAsset<Pitaya::Asset::Shader>(Pitaya::Asset::Shader::GammaCorrection);
	return true;
}
void Pitaya::Render::RenderPipeline::Release()
{
	graph.Passes.clear();
	graph.Items.clear();
	
	chain.BlitShader = nullptr;
	for (auto& shader : chain.PostProcessShaders)
	{
		shader = nullptr;
	}
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
		Pitaya::Core::Print(Pitaya::Core::Color::Yellow, "End Pass");

		//Pass后处理
		bool firstPass = true;
		uint32_t pingpongIndex = 0;
		uint32_t currentReadTexture = pass.RenderTargetSnapshot.SceneColorAttachment; //如果是多采样 这里得到的实际是内部的颜色纹理

		//找出实际执行的后处理步骤
		uint32_t validStepCount = 0;
		uint8_t validSteps[Pitaya::Render::PostProcessSetting::MAX_STEPS];
		for (uint32_t i = 0; i < pass.PostProcessSetting.StepCount; i++) 
		{
			uint8_t typeIndex = static_cast<uint8_t>(pass.PostProcessSetting.Steps[i].Type);
			if (chain.PostProcessShaders[typeIndex].IsReady()) { validSteps[validStepCount++] = i; }
		}

		//提交后处理
		for (uint32_t v = 0; v < validStepCount; v++)
		{
			uint8_t originalIndex = validSteps[v]; //映射回原本的下标
			auto& currentStep = pass.PostProcessSetting.Steps[originalIndex];
			uint8_t typeIndex = static_cast<uint8_t>(currentStep.Type);

			PostProcessCommand cmd;
			cmd.PostProcessStep = currentStep;
			cmd.PostProcessShader = chain.PostProcessShaders[typeIndex]->ID;

			if (firstPass && pass.RenderTargetSnapshot.Multisample)
			{
				cmd.ResolveMSAA = true;
				cmd.ResolveReadFrameBuffer = pass.RenderTargetSnapshot.SceneFrameBuffer;
				cmd.ResolveWriteFrameBuffer = pass.RenderTargetSnapshot.SceneInternalFrameBuffer;
				cmd.ResolveSize = pass.RenderTargetSnapshot.Rect.Size;
			}

			cmd.ReadTexture = currentReadTexture;
			cmd.WriteFrameBuffer = (v == validStepCount - 1) ? pass.RenderTargetSnapshot.FinalFrameBuffer : pass.RenderTargetSnapshot.PingPongFrameBuffers[pingpongIndex];

			currentReadTexture = pass.RenderTargetSnapshot.PingPongColorAttachments[pingpongIndex];
			pingpongIndex = 1 - pingpongIndex;
			firstPass = false;

			renderer->SubmitPostProcess(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), cmd);
			Pitaya::Core::Print(Pitaya::Core::Color::Purple, "Post Process: %s", Pitaya::Render::ToString(currentStep.Type).data());
		}

		//没有后处理则直接Scene帧缓冲区 Blit到 Final帧缓冲区
		if (firstPass && chain.BlitShader.IsReady())
		{
			PostProcessCommand cmd;
			cmd.PostProcessShader = chain.BlitShader->ID;
			cmd.ReadTexture = pass.RenderTargetSnapshot.SceneColorAttachment;
			cmd.WriteFrameBuffer = pass.RenderTargetSnapshot.FinalFrameBuffer; 
			if (pass.RenderTargetSnapshot.Multisample)
			{
				cmd.ResolveMSAA = true;
				cmd.ResolveReadFrameBuffer = pass.RenderTargetSnapshot.SceneFrameBuffer;
				cmd.ResolveWriteFrameBuffer = pass.RenderTargetSnapshot.SceneInternalFrameBuffer;
				cmd.ResolveSize = pass.RenderTargetSnapshot.Rect.Size;
			}

			renderer->SubmitPostProcess(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), cmd);
			Pitaya::Core::Print(Pitaya::Core::Color::Purple, "Post Process Resolve To Final (Bypass)");
		}
	}
}
