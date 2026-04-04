#include<Editor/Camera/Camera.h>
#include<Asset/Common/FuncTable.h>
#include<Input/Common/FuncTable.h>
#include<Time/Common/FuncTable.h>

bool Pitaya::Editor::Camera::Initialize()
{
	renderTarget = Pitaya::Asset::LoadAsset<Pitaya::Asset::RenderTarget>(Pitaya::Asset::RenderTarget::Editor);

	//TOOD REMOVE 改成Asset资产，然后根据资产去动态创建PostProcessSetting
	Pitaya::Render::PostProcessStep step;
	Pitaya::Render::GammaCorrectionParams gama;
	step.SetParams(gama);
	setting.AddPostProcessStep(step);
	//TODO END
	
	return true;
}
void Pitaya::Editor::Camera::Release()
{
	renderTarget = nullptr;
}
void Pitaya::Editor::Camera::Updata()
{
	if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::W)) { Move(Pitaya::Core::Direction::Forward); }
	if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::A)) { Move(Pitaya::Core::Direction::Left); }
	if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::S)) { Move(Pitaya::Core::Direction::Back); }
	if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::D)) { Move(Pitaya::Core::Direction::Right); }
	if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::UP)) { Move(Pitaya::Core::Direction::Up); }
	if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::Down)) { Move(Pitaya::Core::Direction::Down); }
}
void Pitaya::Editor::Camera::Move(Pitaya::Core::Direction dir)
{
	dirty = true;
	float delta = Pitaya::Time::delta();
	switch (dir)
	{
		case Pitaya::Core::Direction::Forward:	position += forward * speed * delta;  break;
		case Pitaya::Core::Direction::Back:		position -= forward * speed * delta;  break;
		case Pitaya::Core::Direction::Left:		position -= glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f))) * speed * delta; break;
		case Pitaya::Core::Direction::Right:	position += glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f))) * speed * delta; break;
		case Pitaya::Core::Direction::Up:		position.y += speed * delta;		  break;
		case Pitaya::Core::Direction::Down:		position.y -= speed * delta;		  break;
	}
}
void Pitaya::Editor::Camera::OnMouseScroll(const Pitaya::Event::MouseScrollEventArgs& args)
{
	dirty = true;
	state.FovDegrees -= args.yOffset;
	state.FovDegrees = glm::clamp(state.FovDegrees, 20.0f, 45.0f);
}
void Pitaya::Editor::Camera::OnMouseCurrsorMove(const Pitaya::Event::MouseCurrsorMoveEventArgs& args)
{
	glm::vec2 mousePosition = glm::vec2(args.xPosition, args.yPosition);
	if (firstMove || !Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::Mouse1))
	{
		lastMousePosition = mousePosition;
		firstMove = false;
		return;
	}

	dirty = true;
	glm::vec2 delta = (mousePosition - lastMousePosition) * sensitivity;
	lastMousePosition = mousePosition;

	yaw += delta.x;
	pitch = glm::clamp(pitch - delta.y, -89.0f, 89.0f);
	forward = glm::normalize(glm::vec3(
		glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
		glm::sin(glm::radians(pitch)),
		glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))));
}
