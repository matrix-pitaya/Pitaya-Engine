#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aTangent;
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

    mat4 boneTransform = mat4(0.0);
    for (int i = 0; i < 4; ++i)
    {
        int boneId = int(aBoneIndices[i]);
        if (boneId >= 0)
        {
            boneTransform += Bones[boneId] * aBoneWeights[i];
        }
    }

    mat4 modelMat = InstanceInfos[index].Model;
    mat4 normalMat = InstanceInfos[index].Normal;
    mat4 boneModel = modelMat * boneTransform;
    mat3 boneNormal = mat3(normalMat) * mat3(boneTransform);

    vec4 worldPos = boneModel * vec4(aPos, 1.0f);
    gl_Position = ViewProjection * worldPos;

    v2f.texCoord = aTexCoord;
    v2f.fragPos = worldPos.xyz;

    v2f.normal = normalize(boneNormal * aNormal);
    v2f.tangent = vec4(normalize(boneNormal * aTangent.xyz), aTangent.w);
    v2f.receiveShadow = InstanceInfos[index].Params.x;
    v2f.materialByteOffset = InstanceInfos[index].Params.y;
}
