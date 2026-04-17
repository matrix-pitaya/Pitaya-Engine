#include<Editor/Camera/Camera.h>
#include<Asset/Common/FuncTable.h>
#include<Input/Common/FuncTable.h>
#include<Time/Common/FuncTable.h>

bool Pitaya::Editor::Camera::Initialize()
{
	renderTarget = Pitaya::Asset::LoadAsset<Pitaya::Asset::RenderTarget>(Pitaya::Asset::RenderTarget::Editor);

	//TOOD 改成Asset资产，然后根据资产去动态创建PostProcessSettin
	Pitaya::Render::PostProcessStep step;
	Pitaya::Render::GammaCorrectionParams gama;
	step.SetParams(gama);
	setting.AddStep(step);
	
	return true;
}
void Pitaya::Editor::Camera::Release()
{
	renderTarget = nullptr;
}
void Pitaya::Editor::Camera::Update()
{
	if (mode == CameraMode::Transitioning)	//过渡模式
	{
		transitionElapsed += Pitaya::Time::delta();
		float t = glm::clamp(transitionElapsed / TransitionDuration, 0.0f, 1.0f);
		if (t >= 1.0f) { t = 1.0f; mode = CameraMode::Orbit; }	//到达后自动进入轨道观察模式

		float blend = t * t * (3.0f - 2.0f * t);

		position = glm::mix(startPos, targetPos, blend);
		pivot = glm::mix(startPivot, targetPivot, blend);

		UpdateCameraVectors();
		return;
	}

	//按下左Control键的同时可以进行移动
	if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::LeftControl))
	{
		bool moved = false;
		if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::W)) { Move(Pitaya::Core::Direction::Forward); moved = true; }
		if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::A)) { Move(Pitaya::Core::Direction::Left); moved = true; }
		if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::S)) { Move(Pitaya::Core::Direction::Back); moved = true; }
		if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::D)) { Move(Pitaya::Core::Direction::Right); moved = true; }
		if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::UP)) { Move(Pitaya::Core::Direction::Up); moved = true; }
		if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::Down)) { Move(Pitaya::Core::Direction::Down); moved = true; }
		if (moved && mode == CameraMode::Orbit) { mode = CameraMode::Fly; }
	}
}
void Pitaya::Editor::Camera::UpdateCameraVectors() 
{
	forward = glm::normalize(glm::vec3(
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)), 
		sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
	if (mode == CameraMode::Orbit) { position = pivot - forward * distance; }
	else if (mode == CameraMode::Fly) { pivot = position + forward * distance; }
	dirty = true;
}
void Pitaya::Editor::Camera::Focus(const glm::vec3& targetPoint, float targetDist)
{
	startPos = position;
	startPivot = (mode == CameraMode::Orbit) ? pivot : (position + forward * 5.0f);

	mode = CameraMode::Transitioning;
	transitionElapsed = 0.0f;

	targetPivot = targetPoint;
	targetPos = targetPoint - forward * targetDist;
	distance = targetDist;

	dirty = true;
}
void Pitaya::Editor::Camera::Move(Pitaya::Core::Direction dir)
{
	dirty = true;
	switch (dir)
	{
		case Pitaya::Core::Direction::Forward:	position += forward * speed * Pitaya::Time::delta();  break;
		case Pitaya::Core::Direction::Back:		position -= forward * speed * Pitaya::Time::delta();  break;
		case Pitaya::Core::Direction::Left:		position -= glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f))) * speed * Pitaya::Time::delta(); break;
		case Pitaya::Core::Direction::Right:	position += glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f))) * speed * Pitaya::Time::delta(); break;
		case Pitaya::Core::Direction::Up:		position.y += speed * Pitaya::Time::delta();		  break;
		case Pitaya::Core::Direction::Down:		position.y -= speed * Pitaya::Time::delta();		  break;
	}
	if (mode == CameraMode::Fly) { pivot = position + forward * distance; }
}
void Pitaya::Editor::Camera::OnMouseScroll(const Pitaya::Event::MouseScrollEventArgs& args)
{
	mode = CameraMode::Orbit;
	distance -= args.yOffset;
	distance = glm::max(0.1f, distance);
	UpdateCameraVectors();
}
void Pitaya::Editor::Camera::OnMouseCurrsorMove(const Pitaya::Event::MouseCurrsorMoveEventArgs& args)
{
	glm::vec2 mousePosition = glm::vec2(args.xPosition, args.yPosition);
	if (firstMove) { lastMousePosition = mousePosition; firstMove = false; return; }

	glm::vec2 rawDelta = mousePosition - lastMousePosition;
	glm::vec2 delta = rawDelta * sensitivity;

	//普通的右键旋转
	if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::Mouse1))
	{
		mode = CameraMode::Fly; // 只要右键转动，就按飞行视角算
		yaw += delta.x;
		pitch = glm::clamp(pitch - delta.y, -89.0f, 89.0f);
		UpdateCameraVectors(); // 核心：计算 Forward，位置不变
	}
	//按住 Alt + 左键（轨道旋转 Orbit） 
	else if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::LeftControl) &&
		Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::Mouse0))
	{
		if (mode != CameraMode::Orbit)
		{
			mode = CameraMode::Orbit;
			pivot = position + forward * distance;
		}

		yaw += delta.x;
		pitch = glm::clamp(pitch - delta.y, -89.0f, 89.0f);

		UpdateCameraVectors();
	}
	// 中键平移
	else if (Pitaya::Input::GetKeyDown(Pitaya::Input::KeyCode::Mouse2))
	{
		if (mode == CameraMode::Transitioning) { mode = CameraMode::Orbit; }

		glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
		glm::vec3 cameraUp = glm::normalize(glm::cross(right, forward));

		float panMultiplier = 0.002f; // 拖拽速度基数
		if (mode == CameraMode::Orbit) { panMultiplier = distance * 0.002f; }
		else { panMultiplier = 0.01f; }

		glm::vec3 displacement = (-right * rawDelta.x + cameraUp * rawDelta.y) * panMultiplier;

		pivot += displacement;
		if (mode == CameraMode::Fly) { position += displacement; }
		else if (mode == CameraMode::Orbit) { UpdateCameraVectors(); }

		dirty = true;
	}

	lastMousePosition = mousePosition;
}
