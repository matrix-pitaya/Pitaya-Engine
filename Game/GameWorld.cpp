#include<Game/GameWorld.h>
#include<Game/Scene/Scene.h>
#include<Game/Component/Camera.h>
#include<Game/Component/Parent.h>

bool Pitaya::Game::GameWorld::Initialize()
{
	activeScene = PITAYA_NEW(Scene);

	//TODOD 序列化场景
	entt::entity entity = activeScene->CreateEntity("Main Camera", "MainCamera");
	activeScene->AddComponent<Camera>(entity).SetRenderTarget(Pitaya::Asset::RenderTarget::Game);

	return true;
}
void Pitaya::Game::GameWorld::Release()
{
	PITAYA_DELETE(activeScene);
	activeScene = nullptr;
}
void Pitaya::Game::GameWorld::LateUpdate()
{
	//渲染前调用
	if (activeScene) { activeScene->ProcessTransformSystem(); }
}
void Pitaya::Game::GameWorld::EndFrame()
{
	//帧末延迟销毁
	if (activeScene) { activeScene->ProcessDelayDestroyQueue(); }
}