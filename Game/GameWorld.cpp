#include<Game/GameWorld.h>
#include<Game/Scene/Scene.h>
#include<Game/Component/Camera.h>
#include<Game/Component/Parent.h>
#include<Game/Component/Light.h>

bool Pitaya::Game::GameWorld::Initialize()
{
	activeScene = PITAYA_NEW(Scene);
	activeScene->Setting.SkyBox = Pitaya::Asset::LoadAsset<Pitaya::Asset::SkyBox>(Pitaya::Asset::SkyBox::Default);
	activeScene->ECS.ReserveDestroyQueue(1024);

	//TODOD 序列化场景
	entt::entity light = activeScene->ECS.CreateEntity("Light");
	activeScene->ECS.AddComponent<Light>(light);
	activeScene->ECS.GetComponent<Transform>(light)->SetLocalEulerAngles(glm::radians(glm::vec3(-9.5f, 25.0f, 2.5f)));
	activeScene->ECS.AddComponent<Camera>(activeScene->ECS.CreateEntity("Main Camera"));

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
	if (auto* _activeScene = GetActiveScene()) { _activeScene->ECS.ProcessHierarchySystem(Pitaya::Core::PassKey<GameWorld>()); }
}
void Pitaya::Game::GameWorld::EndFrame()
{
	//帧末延迟销毁
	if (auto* _activeScene = GetActiveScene()) { _activeScene->ECS.ProcessDelayDestroyQueue(Pitaya::Core::PassKey<GameWorld>()); }
}
