#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aTangent;

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

out V2F
{
    vec2 texCoord;
    vec3 fragPos;
    vec3 normal;
    vec4 tangent;
    flat uint receiveShadow;
    flat uint materialByteOffset;
} v2f;

void main()
{
    uint index = gl_BaseInstance + gl_InstanceID;
    mat4 modelMat = InstanceInfos[index].Model;
    mat4 normalMat = InstanceInfos[index].Normal;

    vec4 worldPos = modelMat * vec4(aPos, 1.0f);
    gl_Position = ViewProjection * worldPos;

    v2f.texCoord = aTexCoord;
    v2f.fragPos = worldPos.xyz;

    v2f.normal = mat3(normalMat) * aNormal;
    v2f.tangent = vec4(mat3(normalMat) * aTangent.xyz, aTangent.w);
    v2f.receiveShadow = InstanceInfos[index].Params.x;
    v2f.materialByteOffset = InstanceInfos[index].Params.y;
}
