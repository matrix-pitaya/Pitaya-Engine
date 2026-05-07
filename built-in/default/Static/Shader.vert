#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

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

out V2F
{
	vec2 texCoord;
	vec3 fragPos;
	vec3 normal;  
} v2f;

void main()
{
	uint index = gl_BaseInstance + gl_InstanceID;
	mat4 modelMat = InstanceTransformInfos[index].Model;
	mat4 normalMat = InstanceTransformInfos[index].Normal;

	vec4 worldPos = modelMat * vec4(aPos, 1.0f);
	gl_Position = ViewProjection * worldPos;
	
	v2f.texCoord = aTexCoord;
	v2f.fragPos = worldPos.xyz;
	
	v2f.normal = mat3(normalMat) * aNormal; 
}