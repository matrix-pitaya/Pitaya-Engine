#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 4) in vec4 aBoneWeights;
layout (location = 5) in vec4 aBoneIndices;

layout(std140, binding = 0) uniform CameraSnapshot
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
    vec4 Position;
};

struct InstanceInfo
{
    mat4 Model;
    mat4 Normal;
	uvec4 Params;
};

layout(std430, binding = 0) readonly buffer InstanceInfoSSBO
{
    InstanceInfo InstanceInfos[];
};

layout(std430, binding = 1) readonly buffer BoneMatrices
{
    mat4 Bones[];
};

void main()
{
    uint index = gl_BaseInstance + gl_InstanceID;
    mat4 boneTransform = mat4(0.0);
    for (int i = 0; i < 4; ++i)
    {
        int boneId = int(aBoneIndices[i]);
        if (boneId >= 0)
        {
            boneTransform += Bones[boneId] * aBoneWeights[i];
        }
    }
    vec4 skinnedPos = boneTransform * vec4(aPos, 1.0);
    gl_Position = ViewProjection * InstanceInfos[index].Model * skinnedPos;
}