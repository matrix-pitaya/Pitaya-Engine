#pragma once

#include<Core/Camera/CameraType.h>
#include<Core/Camera/CameraSnapshot.h>

#include<gtx/euler_angles.hpp>

namespace Pitaya::Core
{
	struct CameraState
	{
        CameraType Type = CameraType::Perspective;

        // 透视投影参数
        float FovDegrees = 45.0f;         // 垂直视场角（度数）
        float AspectRatio = 16.0f / 9.0f; // 宽高比（窗口/视口）

        // 正交投影参数
        float OrthoSize = 5.0f;            // 正交投影垂直范围（上下各OrthoSize/2）

        // 通用裁剪面（近/远裁剪面）
        float NearClip = 0.1f;
        float FarClip = 100.0f;

        inline CameraSnapshot BuildSnapshot(glm::vec3 position, glm::vec3 forward, glm::vec3 up) const
        {
            CameraSnapshot snapshot;
            const glm::vec3 cameraPos = position;
            const glm::vec3 cameraForward = forward;
            const glm::vec3 cameraUp = up;
            snapshot.View = glm::lookAt(cameraPos,  //相机世界位置
                cameraPos + cameraForward,          //相机看向的目标点
                cameraUp);                          //相机上方向

            if (Type == CameraType::Perspective)
            {
                snapshot.Projection = glm::perspective(
                    glm::radians(FovDegrees), //弧度FOV
                    AspectRatio,              //宽高比
                    NearClip,                 //近裁剪面
                    FarClip);                 //远裁剪面
            }
            else
            {
                const float halfHeight = OrthoSize * 0.5f;
                const float halfWidth = halfHeight * AspectRatio;
                snapshot.Projection = glm::ortho(
                    -halfWidth, halfWidth,   //左右范围
                    -halfHeight, halfHeight, //上下范围
                    NearClip, FarClip);      //近远裁剪面
            }

            snapshot.ViewProjection = snapshot.Projection * snapshot.View;
            snapshot.Position = glm::vec4(cameraPos, 1.0f);
            return snapshot;
        }
	};
}
