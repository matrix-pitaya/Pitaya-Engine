#include<Render/RenderPipeline.h>
#include<Render/Frontend/Renderer.h>

#include<Core/Utils/Console.h>
#include<Core/Asset/Asset.h>

#include<Hook/def.h>

void Pitaya::Render::RenderPipeline::Execute(Pitaya::Core::PassKey<Pitaya::Engine::Engine> passkey, Pitaya::Render::Renderer* renderer)
{
	INVOKE_PRERENDERPIPELINEEXECUTE_HOOK(passkey, this)
	Pitaya::Core::Print(Pitaya::Core::Color::Green, "Begin Render Frame");
	renderer->BeginRenderFrame(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>()); //清空fornt缓冲区残余数据

	//构建阴影数据
	BuildShadowData(renderer);

	//提交渲染视图
	SubmitRenderGraph(renderer);

	//交换缓冲区（front→back）并唤醒渲染线程根据back缓冲区命令、SSBO数据进行渲染
	renderer->EndRenderFrame(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>());
	Pitaya::Core::Print(Pitaya::Core::Color::Green, "End Render Frame\n");
}
void Pitaya::Render::RenderPipeline::BuildShadowData(Pitaya::Render::Renderer* renderer)
{
	// 统计投射阴影的光源数量，重置 Params.w = -1（无阴影）
	uint32_t dirCount = 0, spotCount = 0, pointCount = 0;
	for (auto& light : graph.Lights)
	{
		light.Params.w = -1.0f;
		if (light.Direction.w <= 0.0f) { continue; }
		switch (static_cast<uint32_t>(light.Position_Type.w))
		{
			case 0: ++dirCount;   break;
			case 1: ++pointCount; break;
			case 2: ++spotCount;  break;
		}
	}

	graph.DirLightShadowCount = dirCount;
	graph.SpotLightShadowCount = spotCount;
	graph.PointLightShadowCount = pointCount;

	// 无 Pass 或无光源 → 不需要阴影
	if ((graph.Passes.empty()) || (dirCount == 0 && spotCount == 0 && pointCount == 0)) { return; }

	// 聚光灯 [与 Camera 无关 只计算一次]
	uint32_t spotSoFar = 0;
	for (uint32_t i = 0; i < graph.Lights.size(); ++i)
	{
		auto& light = graph.Lights[i];
		if (static_cast<uint32_t>(light.Position_Type.w) != 2 ||
			light.Direction.w <= 0.0f) { continue; }

		ShadowCasterSlice slice;
		slice.LightType = 2;
		slice.LightIndex = i;
		slice.PassIndex = UINT32_MAX;
		slice.MatrixOffset = graph.ShadowMatrices.size();
		slice.MatrixCount = 1;
		slice.LayerOffset = spotSoFar;
		slice.NearPlane = 0.1f;
		slice.FarPlane = light.Params.x;

		graph.ShadowMatrices.emplace_back(1.0f);
		ComputeSpotShadowMatrix(glm::vec3(light.Position_Type), glm::normalize(glm::vec3(light.Direction)),
			light.Params.z, light.Params.x, slice.MatrixOffset);

		light.Params.w = static_cast<float>(spotSoFar);
		graph.ShadowSlices.emplace_back(slice);
		++spotSoFar;
	}

	// 点光源 [与 Camera 无关 只计算一次]
	uint32_t pointSoFar = 0;
	for (uint32_t i = 0; i < static_cast<uint32_t>(graph.Lights.size()); ++i)
	{
		auto& light = graph.Lights[i];
		if (static_cast<uint32_t>(light.Position_Type.w) != 1 ||
			light.Direction.w <= 0.0f) { continue; }

		ShadowCasterSlice slice;
		slice.LightType = 1;
		slice.LightIndex = i;
		slice.PassIndex = UINT32_MAX;
		slice.MatrixOffset = static_cast<uint32_t>(graph.ShadowMatrices.size());
		slice.MatrixCount = POINT_SHADOW_FACE_COUNT;
		slice.LayerOffset = pointSoFar * POINT_SHADOW_FACE_COUNT;
		slice.NearPlane = 0.1f;
		slice.FarPlane = light.Params.x;

		for (uint32_t f = 0; f < POINT_SHADOW_FACE_COUNT; ++f)
		{
			graph.ShadowMatrices.emplace_back(1.0f);
		}

		ComputePointShadowMatrices(glm::vec3(light.Position_Type), light.Params.x, slice.MatrixOffset);
		light.Params.w = static_cast<float>(pointSoFar);
		graph.ShadowSlices.emplace_back(slice);
		++pointSoFar;
	}

	// 平行光 CSM [每个 Pass × 每个平行光]
	for (uint32_t passIdx = 0; passIdx < graph.Passes.size(); ++passIdx)
	{
		const auto& pass = graph.Passes[passIdx];
		uint32_t dirSoFar = 0;

		for (uint32_t i = 0; i < static_cast<uint32_t>(graph.Lights.size()); ++i)
		{
			const auto& light = graph.Lights[i];
			if (static_cast<uint32_t>(light.Position_Type.w) != 0 ||
				light.Direction.w <= 0.0f) { continue; }

			ShadowCasterSlice slice;
			slice.LightType = 0;
			slice.LightIndex = i;
			slice.PassIndex = passIdx;
			slice.MatrixOffset = static_cast<uint32_t>(graph.ShadowMatrices.size());
			slice.MatrixCount = CSM_CASCADE_COUNT;
			slice.NearPlane = pass.NearClip;
			slice.FarPlane = pass.FarClip;
			slice.LayerOffset = (passIdx * dirCount + dirSoFar) * CSM_CASCADE_COUNT;

			for (uint32_t c = 0; c < CSM_CASCADE_COUNT; ++c)
			{
				graph.ShadowMatrices.emplace_back(1.0f);
			}

			ComputeCSMCascades(pass.CameraSnapshot, pass.NearClip, pass.FarClip, glm::normalize(glm::vec3(light.Direction)), slice.MatrixOffset);

			graph.ShadowSlices.emplace_back(slice);
			++dirSoFar;
		}
	}
}
void Pitaya::Render::RenderPipeline::SubmitRenderGraph(Pitaya::Render::Renderer* renderer)
{
	//提交场景环境数据
	renderer->SubmitSceneEnv(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), graph.SceneEnv);

	//提交光源阴影数据
	renderer->SubmitLightShadow(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), { graph.ShadowSlices, graph.ShadowMatrices,
		graph.CascadeSplits, graph.DirLightShadowCount, graph.SpotLightShadowCount, graph.PointLightShadowCount });

	//提交阴影投射物
	for (const auto& slice : graph.ShadowSlices)
	{
		for (uint32_t m = 0; m < slice.MatrixCount; ++m)
		{
			uint32_t layer = slice.LayerOffset + m;
			const glm::mat4& shadowVP = graph.ShadowMatrices[slice.MatrixOffset + m];
			Pitaya::Core::Frustum lightFrustum{ shadowVP };
			Pitaya::Core::Print(Pitaya::Core::Color::Yellow, "Begin Shadow Pass");
			renderer->BeginShadowPass(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), { slice.LightType, layer, shadowVP });
			uint32_t submitCount = 0;
			for (const auto& item : graph.Items)
			{
				if (item.EnableShadowCast && lightFrustum.IsVisible(item.Mesh ? item.Mesh->BoundingBox.ToWorld(item.Model)
					: Pitaya::Core::AABB({ glm::vec3(-0.5f), glm::vec3(0.5f) }).ToWorld(item.Model)))
				{
					renderer->SubmitCastShadowItem(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), item);
					++submitCount;
				}
			}
			Pitaya::Core::Print(Pitaya::Core::Color::Blue, "Shadow Cast Commit Count:%d", submitCount);
			renderer->EndShadowPass(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>());
			Pitaya::Core::Print(Pitaya::Core::Color::Yellow, "End Shadow Pass");
		}
	}

	// 索引基准计算
	uint32_t spotCount = graph.SpotLightShadowCount;
	uint32_t pointCount = graph.PointLightShadowCount;
	uint32_t dirCount = graph.DirLightShadowCount;

	//处理所有渲染通道
	for (uint32_t passIdx = 0; passIdx < static_cast<uint32_t>(graph.Passes.size()); ++passIdx)
	{
		Pitaya::Core::Print(Pitaya::Core::Color::Yellow, "Begin Pass");
		auto& pass = graph.Passes[passIdx];
		pass.LightCount = static_cast<uint32_t>(graph.Lights.size());	// 记录该 Pass 的光源数量
		uint32_t dirSoFar = 0, spotSoFar = 0, pointSoFar = 0;			// 为每个 Pass 生成带有正确 Params.w 索引的光源快照
		for (const auto& sceneLight : graph.Lights)
		{
			LightInfo localLight = sceneLight; // 栈上拷贝
			if (sceneLight.Direction.w > 0.0f)	//开启阴影投射
			{
				uint32_t type = static_cast<uint32_t>(localLight.Position_Type.w);
				switch (type)
				{
					case 0:	// 平行光：CSM — Params.w 存储 ShadowSlices 索引
						localLight.Params.w = static_cast<float>(spotCount + pointCount + passIdx * dirCount + dirSoFar);
						++dirSoFar;
						break;

					case 1:	// 点光源：全 Pass 共用 — Params.w 存储 ShadowSlices 索引
						localLight.Params.w = static_cast<float>(spotCount + pointSoFar);
						++pointSoFar;
						break;

					case 2:	// 聚光灯：全 Pass 共用索引
						localLight.Params.w = static_cast<float>(spotSoFar);
						++spotSoFar;
						break;
				}
			}
			renderer->SubmitLight(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), localLight);
		}
		renderer->BeginPass(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>(), pass);	//提交BeginPass命令 并在命令中更新CameraSnapshotUBO数据（一个Camera对应一个Pass）
		if (graph.SceneEnv.SkyBox) { renderer->SubmitDrawSkybox(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>()); }
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

	INVOKE_TERMINATERENDERPIPELINESUBMITFINALBLIT_HOOK
	renderer->SubmitBlitToScreen(Pitaya::Core::PassKey<Pitaya::Render::RenderPipeline>());
	Pitaya::Core::Print(Pitaya::Core::Color::Red, "Blit Main To Screnn Back Buffer");
}
void Pitaya::Render::RenderPipeline::ComputeCSMCascades(const Pitaya::Core::CameraSnapshot& cameraSnap, float nearClip, float farClip, glm::vec3 lightDir, uint32_t matrixOffset)
{
	// 级联分割（对数 + 线性混合）
	constexpr const float lambda = 0.75f;
	float splits[CSM_CASCADE_COUNT + 1] = {};
	splits[0] = nearClip;
	for (uint32_t i = 1; i <= CSM_CASCADE_COUNT; ++i)
	{
		float p = static_cast<float>(i) / static_cast<float>(CSM_CASCADE_COUNT);
		float logSplit = nearClip * std::pow(farClip / nearClip, p);
		float linSplit = nearClip + (farClip - nearClip) * p;
		splits[i] = lambda * logSplit + (1.0f - lambda) * linSplit;
	}

	// 记录分割距离
	CascadeSplitInfo splitInfo;
	splitInfo.Distances = glm::vec4(splits[1], splits[2], splits[3], splits[4]);
	graph.CascadeSplits.emplace_back(splitInfo);

	// 提取完整视锥角点
	glm::mat4 invCamVP = glm::inverse(cameraSnap.ViewProjection);
	ExtractFrustumCorners(invCamVP);

	for (uint32_t c = 0; c < CSM_CASCADE_COUNT; ++c)
	{
		// 子视锥插值参数
		float tNear = (splits[c] - nearClip) / (farClip - nearClip);
		float tFar = (splits[c + 1] - nearClip) / (farClip - nearClip);

		glm::vec3 cascadeCorners[8];
		for (uint32_t i = 0; i < 4; ++i)
		{
			glm::vec3 ray = graph.FrustumCorners[i + 4] - graph.FrustumCorners[i];
			cascadeCorners[i] = graph.FrustumCorners[i] + ray * tNear;
			cascadeCorners[i + 4] = graph.FrustumCorners[i] + ray * tFar;
		}

		// 子视锥中心
		glm::vec3 center(0.0f);
		for (uint32_t i = 0; i < 8; ++i)
		{
			center += cascadeCorners[i];
		}
		center /= 8.0f;

		// 光源 View
		glm::vec3 up = (std::abs(glm::dot(glm::normalize(lightDir), glm::vec3(0, 1, 0))) > 0.999f)
			? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
		glm::mat4 lightView = glm::lookAt(center - lightDir, center, up);

		// 光源空间包围盒
		glm::vec3 minBB(std::numeric_limits<float>::max());
		glm::vec3 maxBB(std::numeric_limits<float>::lowest());
		for (uint32_t i = 0; i < 8; ++i)
		{
			glm::vec3 ls = glm::vec3(lightView * glm::vec4(cascadeCorners[i], 1.0f));
			minBB = glm::min(minBB, ls);
			maxBB = glm::max(maxBB, ls);
		}

		// 像素对齐（防止阴影边缘闪烁）
		float texelSize = (maxBB.x - minBB.x) / static_cast<float>(Pitaya::Render::Renderer::RenderKit::ShadowAtlas::CSMResolution);
		minBB.x = std::floor(minBB.x / texelSize) * texelSize;
		maxBB.x = std::floor(maxBB.x / texelSize) * texelSize;
		minBB.y = std::floor(minBB.y / texelSize) * texelSize;
		maxBB.y = std::floor(maxBB.y / texelSize) * texelSize;

		// 扩展后方（确保投射物不被裁掉）
		float zExtend = maxBB.z - minBB.z;
		float zNear = -maxBB.z - zExtend * 5.0f;  // 向光源方向后方扩展
		float zFar = -minBB.z;

		// 正交投影
		glm::mat4 lightProj = glm::ortho(minBB.x, maxBB.x, minBB.y, maxBB.y, zNear, zFar);
		graph.ShadowMatrices[matrixOffset + c] = lightProj * lightView;
	}
}
void Pitaya::Render::RenderPipeline::ComputeSpotShadowMatrix(glm::vec3 lightPos, glm::vec3 lightDir, float outerAngleCos, float radius, uint32_t matrixOffset)
{
	float outerAngle = glm::acos(glm::clamp(outerAngleCos, -1.0f, 1.0f));
	float fov = outerAngle * 2.0f;
	fov = glm::clamp(fov, glm::radians(1.0f), glm::radians(179.0f));

	glm::mat4 proj = glm::perspective(fov, 1.0f, 0.1f, radius);
	glm::vec3 up = (std::abs(glm::dot(lightDir, glm::vec3(0, 1, 0))) > 0.999f)
		? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
	glm::mat4 view = glm::lookAt(lightPos, lightPos + lightDir, up);

	graph.ShadowMatrices[matrixOffset] = proj * view;
}
void Pitaya::Render::RenderPipeline::ComputePointShadowMatrices(glm::vec3 lightPos, float radius, uint32_t matrixOffset)
{
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, radius);

	// 6 面方向 + up（对应 cubemap face 顺序）
	static constexpr const glm::vec3 dirs[6] = { { 1,0,0}, {-1,0,0}, {0, 1,0}, {0,-1,0}, {0,0, 1}, {0,0,-1} };
	static constexpr const glm::vec3 ups[6] = { {0,-1,0}, { 0,-1,0}, {0,0,1}, { 0,0,-1}, {0,-1,0}, {0,-1,0} };

	for (uint32_t f = 0; f < 6; ++f)
	{
		glm::mat4 view = glm::lookAt(lightPos, lightPos + dirs[f], ups[f]);
		graph.ShadowMatrices[matrixOffset + f] = proj * view;
	}
}
void Pitaya::Render::RenderPipeline::ExtractFrustumCorners(glm::mat4 invViewProj)
{
	static constexpr const glm::vec4 ndcCorners[8] =
	{
		{ -1, -1, -1, 1 }, {  1, -1, -1, 1 },
		{  1,  1, -1, 1 }, { -1,  1, -1, 1 },
		{ -1, -1,  1, 1 }, {  1, -1,  1, 1 },
		{  1,  1,  1, 1 }, { -1,  1,  1, 1 },
	};

	for (uint32_t i = 0; i < 8; ++i)
	{
		glm::vec4 world = invViewProj * ndcCorners[i];
		graph.FrustumCorners[i] = glm::vec3(world) / world.w;
	}
}
