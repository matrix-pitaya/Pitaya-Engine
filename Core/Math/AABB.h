#pragma once

#include<glm.hpp>

namespace Pitaya::Core
{
	struct AABB
	{
		glm::vec3 Min = glm::vec3(0.0f);
		glm::vec3 Max = glm::vec3(0.0f);

		inline AABB ToWorld(const glm::mat4& model) const noexcept
		{
            glm::vec3 worldMin = glm::vec3(model[3]);
            glm::vec3 worldMax = glm::vec3(model[3]);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    float e = model[i][j] * Min[i];
                    float f = model[i][j] * Max[i];
                    if (e < f) 
                    {
                        worldMin[j] += e;
                        worldMax[j] += f;
                    }
                    else 
                    {
                        worldMin[j] += f;
                        worldMax[j] += e;
                    }
                }
            }
            return { worldMin, worldMax };
		}
	};
}
