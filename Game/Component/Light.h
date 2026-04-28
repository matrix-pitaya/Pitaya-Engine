#pragma once

#include<Game/Common/LightType.h>
#include<glm.hpp>

namespace Pitaya::Game
{
	struct Light
	{
	public:
		// 公共属性
		inline void SetType(LightType type) noexcept
		{ 
			this->type = type;
		}
		inline void SetColor(glm::vec3 color)
		{
			this->color = color;
		}
		inline void SetIntensity(float intensity) noexcept
		{ 
			this->intensity = intensity;
		}
		
		// 点光/聚光属性
		inline void SetRadius(float r) noexcept
		{
			this->radius = r;
		}

		// 聚光灯独占属性
		inline void SetInnerAngle(float innerAngle) noexcept	// 输入单位均为 角度degree
		{
			this->innerAngle = innerAngle;
		}
		inline void SetOuterAngle(float outerAngle) noexcept	// 输入单位均为 角度degree
		{
			this->outerAngle = outerAngle;
		}

	public:
		// 公共属性
		inline LightType GetType() const noexcept
		{ 
			return type;
		}
		inline glm::vec3 GetColor() const noexcept
		{ 
			return color; 
		}
		inline float GetIntensity() const noexcept
		{
			return intensity; 
		}

		// 点光/聚光属性
		inline float GetRadius() const noexcept
		{
			return radius;
		}
		
		// 聚光灯独占属性
		inline float GetInnerAngle() const noexcept
		{ 
			return innerAngle;
		}
		inline float GetOuterAngle() const noexcept
		{
			return outerAngle;
		}

	private:
		LightType type = LightType::Directional;
		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
		float radius = 10.0f;          // 最大衰减范围
		float innerAngle = 12.5f;      // 聚光灯内角(度数)
		float outerAngle = 17.5f;      // 聚光灯外角(度数)
	};
}