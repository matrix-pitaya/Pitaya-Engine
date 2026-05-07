#pragma once

#include<Core/Layout/def.h>
#include<glm.hpp>

namespace Pitaya::Render
{
	struct STD140_LAYOUT LightInfo
	{
		glm::vec4 Position_Type;	// xyz: 位置或方向 w: 光源类型 (0:平行光, 1:点光源, 2:聚光灯)
		glm::vec4 Direction;		// xyz: 光源方向(平行光/聚光灯用) w: 预留
		glm::vec4 Color_Intensity;	// xyz: 颜色(RGB) w: 光源强度 (Intensity)
		glm::vec4 Params;			// x: 半径/衰减范围 y: 聚光灯内角 z: 聚光灯外角 w: ShadowSliceIndex (-1.0 = 无阴影, >=0 = slice 索引)
	};
}