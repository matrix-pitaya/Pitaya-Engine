#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in ivec4 aBoneIds;
layout (location = 4) in vec4 aBoneWeights;

layout(std140, binding = 0) uniform CameraSnapshot
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
    vec4 Position;
};

struct InstanceTransformInfo 
{ 
    mat4 Model; 
    mat4 Normal; 
};

layout(std430, binding = 0) readonly buffer InstanceTransform
{
    InstanceTransformInfo InstanceTransformInfos[];
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
        if (aBoneIds[i] >= 0)
        {
            boneTransform += Bones[aBoneIds[i]] * aBoneWeights[i];
        }
    }
    vec4 skinnedPos = boneTransform * vec4(aPos, 1.0);
    gl_Position = ViewProjection * InstanceTransformInfos[index].Model * skinnedPos;
}