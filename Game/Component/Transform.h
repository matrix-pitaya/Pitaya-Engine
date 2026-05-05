#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include<gtx/euler_angles.hpp>
#include<gtc/quaternion.hpp>
#include<gtx/quaternion.hpp> 
#include<gtx/matrix_decompose.hpp>

#include<entt/entt.hpp>

namespace Pitaya::Game 
{
    struct Transform
    {
    public:
        inline glm::vec3 GetLocalPosition() const noexcept
        {
            return localPosition;
        }
        inline glm::quat GetLocalRotation() const noexcept
        {
            return localRotation;
        }
        inline glm::vec3 GetLocalEulerAngles() const noexcept
        {
            return localEulerAngles;
        }
        inline glm::vec3 GetLocalScale() const noexcept
        {
            return localScale;
        }
        inline glm::vec3 GetLocalForward() const noexcept
        {
            return localRotation * glm::vec3(0.0f, 0.0f, -1.0f);
        }
        inline glm::vec3 GetLocalUp() const noexcept
        {
            return localRotation * glm::vec3(0.0f, 1.0f, 0.0f);
        }
        inline glm::mat4 GetLocalMatrix() const noexcept
        {
            if (localdirty)
            {
                glm::mat4 t = glm::translate(glm::mat4(1.0f), localPosition);
                glm::mat4 r = glm::mat4_cast(localRotation);
                glm::mat4 s = glm::scale(glm::mat4(1.0f), localScale);
                localMatrix = t * r * s;
                localdirty = false;
            }
            return localMatrix;
        }
        inline glm::vec3 GetWorldPosition() const noexcept 
        {
            return glm::vec3(worldMatrix[3]);
        }
        inline glm::quat GetWorldRotation() const noexcept 
        {
            glm::vec3 s, p, sk; glm::vec4 pers; glm::quat r;
            glm::decompose(worldMatrix, s, r, p, sk, pers);
            return r;
        }
        inline glm::vec3 GetWorldScale() const noexcept 
        {
            return glm::vec3(glm::length(worldMatrix[0]), glm::length(worldMatrix[1]), glm::length(worldMatrix[2]));
        }
        inline glm::vec3 GetWorldForward() const noexcept 
        {
            return glm::normalize(-glm::vec3(worldMatrix[2]));
        }
        inline glm::vec3 GetWorldUp() const noexcept 
        {
            return glm::normalize(glm::vec3(worldMatrix[1]));
        }
        inline glm::vec3 GetWorldRight() const noexcept 
        { 
            return glm::normalize(glm::vec3(worldMatrix[0]));
        }
        inline glm::mat4 GetWorldMatrix() const noexcept 
        { 
            return worldMatrix;
        }

    public:
        inline void SetLocalPosition(glm::vec3 pos) noexcept
        {
            if (glm::all(glm::epsilonEqual(localPosition, pos, 0.0001f))) { return; }
            localPosition = pos;
            localdirty = true;
            worlddirty = true;
        }
        inline void SetLocalRotation(glm::quat rot) noexcept
        {
            if (glm::abs(glm::dot(localRotation, rot)) > 0.9999f) { return; }
            localRotation = rot;
            localEulerAngles = glm::eulerAngles(rot);   //旋转四元数 同步更新欧拉角
            localdirty = true;
            worlddirty = true;
        }
        inline void SetLocalEulerAngles(glm::vec3 eulerRadians) noexcept 
        {
            if (glm::all(glm::epsilonEqual(localEulerAngles, eulerRadians, 0.0001f))) { return; }
            localEulerAngles = eulerRadians;
            localRotation = glm::quat(eulerRadians);    // 设置欧拉角 同步更新四元数
            localdirty = true;
            worlddirty = true;
        }
        inline void SetLocalScale(glm::vec3 scale) noexcept
        {
            if (glm::all(glm::epsilonEqual(localScale, scale, 0.0001f))) { return; }
            localScale = scale;
            localdirty = true;
            worlddirty = true;
        }
        inline void SetLocalFromMatrix(const glm::mat4& matrix) noexcept
        {
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(matrix, localScale, localRotation, localPosition, skew, perspective);
            localEulerAngles = glm::eulerAngles(localRotation); // 同步更新缓存
            localdirty = true;
            worlddirty = true;
        }
        inline void SetWorldMatrix(const glm::mat4& matrix) noexcept
        {
            worldMatrix = matrix;
            worlddirty = false;
        }

    public:
        inline void MarkDirty() noexcept
        {
            worlddirty = true;
		}
        inline bool IsDirty() const noexcept
        {
            return worlddirty;
        }

    private:
        glm::vec3 localPosition = glm::vec3(0.0f);
        glm::quat localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 localEulerAngles = glm::vec3(0.0f);   //欧拉角 弧度制
        glm::vec3 localScale = glm::vec3(1.0f);

        mutable glm::mat4 localMatrix = glm::mat4(1.0f);
        glm::mat4 worldMatrix = glm::mat4(1.0f);

        mutable bool localdirty = true;
        bool worlddirty = true;
    };
}
