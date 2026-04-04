#include<Game/GameWorld.h>

bool Pitaya::Game::GameWorld::Initialize()
{
	return true;
}
void Pitaya::Game::GameWorld::Release()
{

}
void Pitaya::Game::GameWorld::BeginFrame()
{

}
void Pitaya::Game::GameWorld::FixedUpdate()
{

}
void Pitaya::Game::GameWorld::Update()
{

}
void Pitaya::Game::GameWorld::LateUpdate()
{


	//渲染之前调用
	if (activeScene) 
	{ 
		activeScene->ProcessTransformSystem(Pitaya::Core::PassKey<Pitaya::Game::GameWorld>()); 
	}
}
void Pitaya::Game::GameWorld::EndFrame()
{

}