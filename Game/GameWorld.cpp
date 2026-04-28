#include<Game/GameWorld.h>
#include<Game/Scene/Scene.h>
#include<Game/Component/Camera.h>
#include<Game/Component/Parent.h>
#include<Game/Component/Light.h>

bool Pitaya::Game::GameWorld::Initialize()
{
	activeScene = PITAYA_NEW(Scene);

	//TODOD 序列化场景
	activeScene->AddComponent<Camera>(activeScene->CreateEntity("Main Camera"));
	activeScene->AddComponent<Light>(activeScene->CreateEntity("Light"));

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
	if (activeScene) { activeScene->ProcessHierarchySystem(); }
}
void Pitaya::Game::GameWorld::EndFrame()
{
	//帧末延迟销毁
	if (activeScene) { activeScene->ProcessDelayDestroyQueue(); }
}